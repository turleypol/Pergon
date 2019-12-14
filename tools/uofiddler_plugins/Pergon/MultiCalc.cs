using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using Ultima;

namespace Pergon
{
    public partial class MultiCalc : Form
    {
        private List<Group> Groups;
        private string MultiXMLFileName = Path.Combine(FiddlerControls.Options.AppDataPath, "Multilist.xml");
        private XmlDocument xDom = null;
        private XmlElement xMultis = null;
        private bool Loaded = false;
        private int GroupNameLen = 0;
        public MultiCalc()
        {
            this.Icon = FiddlerControls.Options.GetFiddlerIcon();
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.UserPaint, true);
            InitializeComponent();
            if ((File.Exists(MultiXMLFileName)))
            {
                xDom = new XmlDocument();
                xDom.Load(MultiXMLFileName);
                xMultis = xDom["Multis"];
            }
        }

        private void Reload()
        {
            if (Loaded)
                OnLoad(this, EventArgs.Empty);
        }
        private void OnLoad(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            FiddlerControls.Options.LoadedUltimaClass["TileData"] = true;
            FiddlerControls.Options.LoadedUltimaClass["Art"] = true;
            FiddlerControls.Options.LoadedUltimaClass["Multis"] = true;
            FiddlerControls.Options.LoadedUltimaClass["Hues"] = true;

            LoadXML();

            Multi_treeView.BeginUpdate();
            Multi_treeView.Nodes.Clear();
            List<TreeNode> cache = new List<TreeNode>();
            for (int i = 0; i < 0x2000; ++i)
            {
                MultiComponentList multi = Ultima.Multis.GetComponents(i);
                if (multi != MultiComponentList.Empty)
                {
                    TreeNode node = null;
                    if (xDom == null)
                    {
                        node = new TreeNode(String.Format("{0,5} (0x{0:X})", i));
                    }
                    else
                    {
                        XmlNodeList xMultiNodeList = xMultis.SelectNodes("/Multis/Multi[@id='" + i + "']");
                        string j = "";
                        foreach (XmlNode xMultiNode in xMultiNodeList)
                        {
                            j = xMultiNode.Attributes["name"].Value;
                        }
                        node = new TreeNode(String.Format("{0,5} (0x{0:X}) {1}", i, j));
                    }
                    node.Tag = multi;
                    node.Name = i.ToString();
                    cache.Add(node);
                }
            }
            Multi_treeView.Nodes.AddRange(cache.ToArray());
            Multi_treeView.EndUpdate();
            if (Multi_treeView.Nodes.Count > 0)
                Multi_treeView.SelectedNode = Multi_treeView.Nodes[0];
            if (!Loaded)
            {
                FiddlerControls.Events.FilePathChangeEvent += new FiddlerControls.Events.FilePathChangeHandler(OnFilePathChangeEvent);
                FiddlerControls.Events.MultiChangeEvent += new FiddlerControls.Events.MultiChangeHandler(OnMultiChangeEvent);
            }
            Loaded = true;
            Cursor.Current = Cursors.Default;
        }

        private void OnFilePathChangeEvent()
        {
            Reload();
        }

        private void OnMultiChangeEvent(object sender, int id)
        {
            if (!Loaded)
                return;
            if (sender.Equals(this))
                return;
            MultiComponentList multi = Ultima.Multis.GetComponents(id);
            if (multi != MultiComponentList.Empty)
            {
                bool done = false;
                for (int i = 0; i < Multi_treeView.Nodes.Count; ++i)
                {
                    if (id == int.Parse(Multi_treeView.Nodes[i].Name))
                    {
                        Multi_treeView.Nodes[i].Tag = multi;
                        Multi_treeView.Nodes[i].ForeColor = Color.Black;
                        if (i == Multi_treeView.SelectedNode.Index)
                            AfterSelect_MultiTreeView(this, null);
                        done = true;
                        break;
                    }
                    else if (id < int.Parse(Multi_treeView.Nodes[i].Name))
                    {
                        TreeNode node = new TreeNode(String.Format("{0,5} (0x{0:X})", id));
                        node.Tag = multi;
                        node.Name = id.ToString();
                        Multi_treeView.Nodes.Insert(i, node);
                        done = true;
                        break;
                    }
                }
                if (!done)
                {
                    TreeNode node = new TreeNode(String.Format("{0,5} (0x{0:X})", id));
                    node.Tag = multi;
                    node.Name = id.ToString();
                    Multi_treeView.Nodes.Add(node);
                }
            }
        }

        private void LoadXML()
        {
            Groups = new List<Group>();

            string path = FiddlerControls.Options.AppDataPath;
            string FileName = Path.Combine(path, @"plugins/pergon_multicalc.xml");
            if (!File.Exists(FileName))
                return;

            XmlDocument dom = new XmlDocument();
            dom.Load(FileName);
            XmlElement xTiles = dom["Groups"];

            foreach (XmlNode xRootGroup in xTiles.ChildNodes)
            {
                if (xRootGroup.NodeType == XmlNodeType.Comment)
                    continue;
                XmlElement elem = (XmlElement)xRootGroup;
                Group group = FindOrCreateGroup(elem.GetAttribute("name"),
                    double.Parse(elem.GetAttribute("price")));

                foreach (XmlNode node in xRootGroup.ChildNodes)
                {
                    if (node.NodeType == XmlNodeType.Comment)
                        continue;
                    XmlElement childelem = (XmlElement)node;

                    string strindex = childelem.GetAttribute("index");
                    double factor = double.Parse(childelem.GetAttribute("factor"));

                    foreach (string part in strindex.Split(';'))
                    {
                        if (part.Contains("-"))
                        {
                            string[] delims = part.Split('-');
                            int elem1 = Int32.Parse(delims[0]);
                            int elem2 = Int32.Parse(delims[1]);
                            for (; elem1 <= elem2; ++elem1)
                            {
                                group.Entries.Add(new Entry(elem1, factor));
                            }
                        }
                        else
                            group.Entries.Add(new Entry(Int32.Parse(part), factor));
                    }
                }

                Groups.Add(group);
            }
        }



        private void AfterSelect_MultiTreeView(object sender, TreeViewEventArgs e)
        {
            MultiComponentList multi = (MultiComponentList)Multi_treeView.SelectedNode.Tag;
            pictureBox.Invalidate();
            CalcStuff(multi, Int32.Parse(Multi_treeView.SelectedNode.Name));

        }

        private void onPaintMultiPicbox(object sender, PaintEventArgs e)
        {
            if (Multi_treeView.SelectedNode == null)
                return;
            if ((MultiComponentList)Multi_treeView.SelectedNode.Tag == MultiComponentList.Empty)
            {
                e.Graphics.Clear(Color.White);
                return;
            }
            Bitmap m_MainPicture_Multi = ((MultiComponentList)Multi_treeView.SelectedNode.Tag).GetImage();
            if (m_MainPicture_Multi == null)
            {
                e.Graphics.Clear(Color.White);
                return;
            }
            Point location = Point.Empty;
            Size size = pictureBox.Size;
            Rectangle destRect = Rectangle.Empty;
            if ((m_MainPicture_Multi.Height < size.Height) && (m_MainPicture_Multi.Width < size.Width))
            {
                location.X = (pictureBox.Width - m_MainPicture_Multi.Width) / 2;
                location.Y = (pictureBox.Height - m_MainPicture_Multi.Height) / 2;
                destRect = new Rectangle(location, m_MainPicture_Multi.Size);
            }
            else if (m_MainPicture_Multi.Height < size.Height)
            {
                location.X = 0;
                location.Y = (pictureBox.Height - m_MainPicture_Multi.Height) / 2;
                if (m_MainPicture_Multi.Width > size.Width)
                    destRect = new Rectangle(location, new Size(size.Width, m_MainPicture_Multi.Height));
                else
                    destRect = new Rectangle(location, m_MainPicture_Multi.Size);
            }
            else if (m_MainPicture_Multi.Width < size.Width)
            {
                location.X = (pictureBox.Width - m_MainPicture_Multi.Width) / 2;
                location.Y = 0;
                if (m_MainPicture_Multi.Height > size.Height)
                    destRect = new Rectangle(location, new Size(m_MainPicture_Multi.Width, size.Height));
                else
                    destRect = new Rectangle(location, m_MainPicture_Multi.Size);
            }
            else
                destRect = new Rectangle(new Point(0, 0), size);


            e.Graphics.DrawImage(m_MainPicture_Multi, destRect, 0, 0, m_MainPicture_Multi.Width, m_MainPicture_Multi.Height, System.Drawing.GraphicsUnit.Pixel);
        }

        private void CalcStuff(MultiComponentList multi, int id)
        {
            richTextBoxContents.Clear();
            richTextBoxCalc.Clear();
            foreach (Group group in Groups)
            {
                group.Count = 0;
                group.SumPrice = 0.0;
            }

            Hashtable tiletable = new Hashtable();
            Hashtable leftovers = new Hashtable();
            int surfacewalkable = 0;
            if (multi == MultiComponentList.Empty)
                return;

            for (int x = 0; x < multi.Width; ++x)
            {
                for (int y = 0; y < multi.Height; ++y)
                {
                    MTile[] tiles = multi.Tiles[x][y];
                    for (int i = 0; i < tiles.Length; ++i)
                    {
                        if (tiles[i].ID == 0x1)
                            continue;
                        if (tiletable.ContainsKey(tiles[i].ID))
                            tiletable[tiles[i].ID] = (int)tiletable[tiles[i].ID] + 1;
                        else
                            tiletable.Add(tiles[i].ID, 1);
                    }
                }
            }


            ArrayList aKeys = new ArrayList(tiletable.Keys);
            aKeys.Sort();
            int itemcount = 0;
            foreach (ushort key in aKeys)
            {
                richTextBoxContents.AppendText(String.Format("0x{0:X4} ({0}) : {1}\n", key, tiletable[key]));
                itemcount += (int)tiletable[key];
                bool found = false;
                foreach (Group group in Groups)
                {
                    if (group.FindEntry((int)key, (int)tiletable[key])) //multidefinitions of one item _no_ break
                        found = true;
                }
                if (!found)
                    leftovers.Add(key, (int)tiletable[key]);
            }

            //walkable surface
            for (int x = 0; x < multi.Width; ++x)
            {
                for (int y = 0; y < multi.Height; ++y)
                {
                    MTile[] tiles = multi.Tiles[x][y];
                    int lastz = -100;
                    foreach (MTile tile in tiles)
                    {
                        ItemData itemdata = TileData.ItemTable[tile.ID];
                        if ((itemdata.Flags & TileFlag.Surface) != 0)
                        {
                            if (Math.Abs(lastz - tile.Z) <= 2)
                                continue;
                            lastz = tile.Z;
                            int Start = tile.Z + itemdata.CalcHeight;
                            AddSurface(Start, tiles, ref surfacewalkable);
                        }
                    }
                }
            }

            richTextBoxContents.AppendText(String.Format("\nItemCount: {0}", itemcount));

            System.Drawing.Font font = new System.Drawing.Font(richTextBoxCalc.Font.FontFamily, richTextBoxCalc.Font.Size, System.Drawing.FontStyle.Bold);
            string topic = String.Format("{0,-" + GroupNameLen + "}: {1,-7}: Price\n", "Name", "Count");
            richTextBoxCalc.AppendText(topic);
            richTextBoxCalc.Select(0, topic.Length);
            richTextBoxCalc.SelectionFont = font;
            foreach (Group group in Groups)
            {
                richTextBoxCalc.AppendText(String.Format("{0,-" + GroupNameLen + "}: {1,-7}: {2}\n", group.Name, group.Count, Math.Floor(group.SumPrice)));
            }
            richTextBoxCalc.AppendText(String.Format("WalkableSurface : {0}\n", surfacewalkable));
            richTextBoxCalc.AppendText("\nLeftOvers:\n");
            richTextBoxCalc.Select(richTextBoxCalc.Text.IndexOf("\nLeftOvers:\n"), 10);
            richTextBoxCalc.SelectionFont = font;
            foreach (DictionaryEntry de in leftovers)
            {
                richTextBoxCalc.AppendText(String.Format("0x{0:X4} ({0,5}) : {1}\n", (ushort)de.Key, de.Value));
            }

            richTextBoxCalc.AppendText("\nCfg Entry:\n");
            richTextBoxCalc.Select(richTextBoxCalc.Text.IndexOf("\nCfg Entry:\n"), 10);
            richTextBoxCalc.SelectionFont = font;
            richTextBoxCalc.AppendText(String.Format("MultiID     0x{0:X4}\n", id));
            richTextBoxCalc.AppendText("Item 0x\n{\n");
            richTextBoxCalc.AppendText("    Name                \n");
            richTextBoxCalc.AppendText("    Desc                Baupla%ene/n% fuer ein \n");
            richTextBoxCalc.AppendText("    Graphic             0x14f0\n");
            richTextBoxCalc.AppendText("    Script              housedeed\n");
            richTextBoxCalc.AppendText(String.Format("    VendorBuysFor       {0}\n", surfacewalkable * 10000));
            richTextBoxCalc.AppendText(String.Format("    VendorSellsFor      {0}\n", surfacewalkable * 20000));
            richTextBoxCalc.AppendText("    Weight              1/20\n\n");

            richTextBoxCalc.AppendText("    HouseObjType        \n");
            richTextBoxCalc.AppendText("    HouseType           \n");
            richTextBoxCalc.AppendText("    NumLockDowns        500\n");
            richTextBoxCalc.AppendText("    NumSecure           6\n\n");
            foreach (Group group in Groups)
            {
                if (!group.Name.Equals("IGNORED"))
                    richTextBoxCalc.AppendText(String.Format("    {0,-" + GroupNameLen + "}{1}\n", group.Name, Math.Floor(group.SumPrice)));
            }
            richTextBoxCalc.AppendText("}\n");
        }

        private const TileFlag ImpassableOrSurface = TileFlag.Impassable | TileFlag.Surface;
        private void AddSurface(int Start, MTile[] tiles, ref int surface)
        {
            int Top = Start + 16; // Playerheight
            foreach (MTile tile in tiles)
            {
                ItemData itemdata = TileData.ItemTable[tile.ID];
                if ((itemdata.Flags & ImpassableOrSurface) != 0)
                {
                    if ((itemdata.Flags & TileFlag.Door) != 0)
                        continue;
                    int checkTop = tile.Z + itemdata.CalcHeight;
                    if ((checkTop > Start) && (tile.Z < Top))
                        return;
                }
            }
            ++surface;
        }

        private Group FindOrCreateGroup(string name, double price)
        {
            foreach (Group group in Groups)
            {
                if ((group.Name.Equals(name)) && (group.Price == price))
                    return group;
            }
            if (name.Length > GroupNameLen)
                GroupNameLen = name.Length + 1;
            return new Group(name, price);
        }

        private void OnClickReloadXML(object sender, EventArgs e)
        {
            LoadXML();
            AfterSelect_MultiTreeView(null, null);
        }

        private void OnClickSaveAllImages(object sender, EventArgs e)
        {
            using (FolderBrowserDialog dialog = new FolderBrowserDialog())
            {
                dialog.Description = "Select directory";
                dialog.ShowNewFolderButton = true;
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < Multi_treeView.Nodes.Count; i++)
                    {
                        int index = int.Parse(Multi_treeView.Nodes[i].Name);
                        if (index >= 0)// && index ==0x10a1)
                        {
                            string FileName = Path.Combine(dialog.SelectedPath, String.Format("house_0x{0:X}.png", index));
                            int h = 120;
                            Bitmap bit = ((MultiComponentList)Multi_treeView.Nodes[i].Tag).GetImage(h,true);
                            if (bit != null)
                                bit.Save(FileName, ImageFormat.Png);
                            bit.Dispose();

                            List<int> z_list = getAllStandableZ((MultiComponentList)Multi_treeView.Nodes[i].Tag);
                            int last_z = 120;
                            int counter = 1;
                            foreach (int z in z_list)
                            {
                                if (z + 16 <= last_z)
                                {
                                    last_z = z;
                                    FileName = Path.Combine(dialog.SelectedPath, String.Format("house_0x{0:X}_{1}.png", index, counter));
                                    bit = ((MultiComponentList)Multi_treeView.Nodes[i].Tag).GetImage(z+16, true);
                                    if (bit != null)
                                        bit.Save(FileName, ImageFormat.Png);
                                    bit.Dispose();
                                    counter++;
                                }
                            }

                        }
                    }
                    MessageBox.Show(String.Format("All Multis saved to {0}", dialog.SelectedPath), "Saved", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
                }
            }
        }

        private List<int> getAllStandableZ(MultiComponentList multi)
        {
            List<int> z_list = new List<int>();
            for (int x = 0; x < multi.Width; ++x)
            {
                for (int y = 0; y < multi.Height; ++y)
                {
                    MTile[] tiles = multi.Tiles[x][y];
                    int lastz = -100;
                    foreach (MTile tile in tiles)
                    {
                        ItemData itemdata = TileData.ItemTable[tile.ID];
                        if ((itemdata.Flags & TileFlag.Surface) != 0)
                        {
                            if (Math.Abs(lastz - tile.Z) <= 2)
                                continue;
                            lastz = tile.Z;
                            int Start = tile.Z + itemdata.CalcHeight;

                            int Top = Start + 16; // Playerheight
                            bool add = true;
                            bool highest = true;
                            foreach (MTile _tile in tiles)
                            {
                                ItemData _itemdata = TileData.ItemTable[_tile.ID];
                                if ((_itemdata.Flags & ImpassableOrSurface) != 0)
                                {
                                    if ((_itemdata.Flags & TileFlag.Door) != 0)
                                        continue;
                                    int checkTop = _tile.Z + _itemdata.CalcHeight;
                                    if ((checkTop > Start) && (_tile.Z < Top))
                                    {
                                        add = false;
                                        break;
                                    }
                                }
                                if (_tile.Z > tile.Z)
                                    highest = false;
                            }
                            if (!highest && add && !z_list.Contains(tile.Z))
                                z_list.Add(tile.Z);
                        }
                    }
                }
            }
            z_list.Sort();
            z_list.Reverse();
            return z_list;
        }

        private void OnClickSaveAllHeight(object sender, EventArgs e)
        {
            using (FolderBrowserDialog dialog = new FolderBrowserDialog())
            {
                dialog.Description = "Select directory";
                dialog.ShowNewFolderButton = true;
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    if ((MultiComponentList)Multi_treeView.SelectedNode.Tag != null)
                    {
                        int index = int.Parse(Multi_treeView.SelectedNode.Name);
                        string FileName = Path.Combine(dialog.SelectedPath, String.Format("house_0x{0:X}.png", index));
                        int h = 120;
                        Bitmap bit = ((MultiComponentList)Multi_treeView.SelectedNode.Tag).GetImage(h, true);
                        if (bit != null)
                            bit.Save(FileName, ImageFormat.Png);
                        bit.Dispose();

                        List<int> z_list = getAllStandableZ((MultiComponentList)Multi_treeView.SelectedNode.Tag);
                        int last_z = 120;
                        int counter = 1;
                        foreach (int z in z_list)
                        {
                            last_z = z;
                            FileName = Path.Combine(dialog.SelectedPath, String.Format("house_0x{0:X}_{1}.png", index, counter));
                            bit = ((MultiComponentList)Multi_treeView.SelectedNode.Tag).GetImage(z + 16, true);
                            if (bit != null)
                                bit.Save(FileName, ImageFormat.Png);
                            bit.Dispose();
                            counter++;
                        }
                    }
                    MessageBox.Show(String.Format("All Multis saved to {0}", dialog.SelectedPath), "Saved", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
                }
            }
        }

        private void OnClickCalculateAll(object sender, EventArgs e)
        {
            string contents = Path.Combine(FiddlerControls.Options.OutputPath, "multicalc_contents.txt");
            string cfg = Path.Combine(FiddlerControls.Options.OutputPath, "multicalc_cfg.txt");
            string msg = Path.Combine(FiddlerControls.Options.OutputPath, "multicalc_msgs.txt");
            richTextBoxCalc.SuspendLayout();
            richTextBoxContents.SuspendLayout();
            using (StreamWriter tex_contents = new StreamWriter(new FileStream(contents, FileMode.Create, FileAccess.Write), System.Text.Encoding.GetEncoding(1252)),
                tex_cfg = new StreamWriter(new FileStream(cfg, FileMode.Create, FileAccess.Write), System.Text.Encoding.GetEncoding(1252)),
                tex_msg = new StreamWriter(new FileStream(msg, FileMode.Create, FileAccess.Write), System.Text.Encoding.GetEncoding(1252)))
            {
                for (int i = 0; i < Multi_treeView.Nodes.Count; i++)
                {
                    MultiComponentList multi = (MultiComponentList)Multi_treeView.Nodes[i].Tag;
                    if (multi != null)
                    {
                        CalcStuff(multi, Int32.Parse(Multi_treeView.Nodes[i].Name));
                        tex_contents.WriteLine("MultiID - " + Multi_treeView.Nodes[i].Name);
                        tex_cfg.WriteLine("MultiID - " + Multi_treeView.Nodes[i].Name);
                        tex_msg.WriteLine("MultiID - " + Multi_treeView.Nodes[i].Name);
                        foreach (string str in richTextBoxContents.Lines )
                        {
                            tex_contents.WriteLine(str);
                        }
                        tex_contents.Write("\n");
                        tex_contents.Write("\n");
                        bool in_cfg = false;
                        foreach (string str in richTextBoxCalc.Lines)
                        {
                            if (str.StartsWith("Cfg Entry:"))
                            {
                                in_cfg = true;
                                continue;
                            }
                            if (in_cfg)
                                tex_cfg.WriteLine(str);
                            else
                                tex_msg.WriteLine(str);
                        }
                        tex_cfg.Write("\n");
                        tex_cfg.Write("\n");
                        tex_msg.Write("\n");
                        tex_msg.Write("\n");
                    }
                }
            }
            richTextBoxCalc.Clear();
            richTextBoxContents.Clear();
            richTextBoxCalc.ResumeLayout();
            richTextBoxContents.ResumeLayout();
            MessageBox.Show(String.Format("All calculations saved to {0}", FiddlerControls.Options.OutputPath), "Saved", MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
        }
    }

    public class Entry
    {
        public int Index { get; private set; }
        public double Factor { get; private set; }
        public Entry(int index, double factor)
        {
            Index = index;
            Factor = factor;
        }

    }
    public class Group
    {
        public string Name { get; set; }
        public double Price { get; set; }
        public List<Entry> Entries { get; set; }
        public int Count { get; set; }
        public double SumPrice { get; set; }

        public Group(string name, double price)
        {
            Name = name;
            Price = price;
            Entries = new List<Entry>();
            Count = 0;
            SumPrice = 0;
        }

        public bool FindEntry(int entry, int count)
        {
            foreach (Entry e in Entries)
            {
                if (e.Index == entry)
                {
                    Count += count;
                    SumPrice += count * e.Factor * Price;
                    return true;
                }
            }
            return false;
        }
        public bool FindEntry(int entry)
        {
            return Entries.Find(delegate(Entry e) { return (e.Index == entry); }) != null;
        }
    }
}

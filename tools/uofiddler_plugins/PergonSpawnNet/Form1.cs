using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Ultima;

namespace PergonSpawnNet
{
    public partial class SpawnViewer : Form
    {
        private SpawnViewer refmarker;
        private Bitmap map;
        private Ultima.Map currmap;
        private int currmapint = 0;
        private Point currPoint;
        private Point lastHoverPoint;
        private double zoom = 1;
        private bool moving = false;
        private Point movingpoint;
        private bool Loaded;

        public int HScrollBar { get { return refmarker.hScrollBar.Value; } }
        public int VScrollBar { get { return refmarker.vScrollBar.Value; } }
        public Ultima.Map CurrMap { get { return refmarker.currmap; } }
        public double Zoom { get { return refmarker.zoom; } }
        public bool PaintRanges { get { return refmarker.showRangesToolStripMenuItem.Checked; } }

        private ClientObject clientcursor;

        public SpawnViewer()
        {
            refmarker = this;
            this.Icon = FiddlerControls.Options.GetFiddlerIcon();
            InitializeComponent();
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.UserPaint, true);
            clientcursor = new ClientObject(this);
            clientcursor.Visible = false;
        }

        private void Reload()
        {
            if (!Loaded)
                return;
            zoom = 1;
            moving = false;
            OnLoad(this, EventArgs.Empty);
        }

        private void OnLoad(object sender, EventArgs e)
        {
            FiddlerControls.Options.LoadedUltimaClass["Map"] = true;
            FiddlerControls.Options.LoadedUltimaClass["RadarColor"] = true;
            currmap = Ultima.Map.Felucca;
            ZoomLabel.Text = String.Format("Zoom: {0}", Zoom);
            SetScrollBarValues();
            feluccaToolStripMenuItem.Checked = true;
            trammelToolStripMenuItem.Checked = false;
            ilshenarToolStripMenuItem.Checked = false;
            malasToolStripMenuItem.Checked = false;
            tokunoToolStripMenuItem.Checked = false;
            ChangeMapNames();
            Refresh();
            if (!Loaded)
            {
                FiddlerControls.Events.MapDiffChangeEvent += new FiddlerControls.Events.MapDiffChangeHandler(OnMapDiffChangeEvent);
                FiddlerControls.Events.MapNameChangeEvent += new FiddlerControls.Events.MapNameChangeHandler(OnMapNameChangeEvent);
                FiddlerControls.Events.MapSizeChangeEvent += new FiddlerControls.Events.MapSizeChangeHandler(OnMapSizeChangeEvent);
                FiddlerControls.Events.FilePathChangeEvent += new FiddlerControls.Events.FilePathChangeHandler(OnFilePathChangeEvent);
                LoadData();
                clientcursor.X = -1;
                clientcursor.Y = -1;
                clientcursor.Z = -1;
                clientcursor.Map = Map.Felucca;
                clientcursor.IntMap = 0;
                clientcursor.Visible = showClientLocToolStripMenuItem1.Checked;

            }
            toolTip.Tag = null;
            Loaded = true;
        }

        #region Grafikstuff
        private void OnMapDiffChangeEvent()
        {
            pictureBox.Invalidate();
        }
        private void OnMapNameChangeEvent()
        {
            ChangeMapNames();
        }
        private void OnMapSizeChangeEvent()
        {
            Reload();
            pictureBox.Invalidate();
        }
        private void OnFilePathChangeEvent()
        {
            Reload();
            pictureBox.Invalidate();
        }

        private void HandleScroll(object sender, ScrollEventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void OnPaint(object sender, PaintEventArgs e)
        {
            map = currmap.GetImage(hScrollBar.Value >> 3, vScrollBar.Value >> 3,
                (int)((e.ClipRectangle.Width / zoom) + 8) >> 3, (int)((e.ClipRectangle.Height / zoom) + 8) >> 3,
                true);
            ZoomMap(ref map);
            e.Graphics.DrawImageUnscaledAndClipped(map, e.ClipRectangle);

            if (AllRunes.Count > 0)
            {
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (Rune rune in AllRunes)
                    {
                        if (rune.isVisible(e.ClipRectangle, currmap))
                            rune.Draw(e.Graphics);
                    }
                }
            }
            if (AllRegion.Count > 0)
            {
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (Region region in AllRegion)
                    {
                        if (region.isVisible(e.ClipRectangle, currmap))
                            region.Draw(e.Graphics);
                    }
                }
            }
            if (clientcursor.isVisible(e.ClipRectangle, currmap))
                clientcursor.Draw(e.Graphics);
        }

        private void OnResizeMap(object sender, EventArgs e)
        {
            if (Loaded)
            {
                ChangeScrollBar();
                pictureBox.Invalidate();
            }
        }

        private void OnMouseDownMap(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                moving = true;
                movingpoint.X = e.X;
                movingpoint.Y = e.Y;
                this.Cursor = Cursors.Hand;
            }
            else if (e.Button == MouseButtons.Right)
                return;
            else if (e.Button != MouseButtons.None)
            {
                moving = false;
                this.Cursor = Cursors.Default;
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (OverlayObject rune in AllRunes)
                    {
                        if (rune.isHovered(e, currmap))
                        {
                            listBox.SelectedItem = rune;
                            return;
                        }
                    }
                }
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (OverlayObject region in AllRegion)
                    {
                        if (region.isHovered(e, currmap))
                        {
                            listBox.SelectedItem = region;
                            return;
                        }
                    }
                }
                listBox.SelectedItem = null;
            }
        }

        private void OnMouseMoveMap(object sender, MouseEventArgs e)
        {
            int xDelta = Math.Min(currmap.Width, (int)(e.X / zoom) + Round(hScrollBar.Value));
            int yDelta = Math.Min(currmap.Height, (int)(e.Y / zoom) + Round(vScrollBar.Value));
            CoordsLabel.Text = String.Format("Coords: {0},{1}", xDelta, yDelta);
            Point oldhover = lastHoverPoint;
            //lastHoverPoint.X = e.X;
            //lastHoverPoint.Y = e.Y;
            if (moving)
            {
                int deltax = (int)(-1 * (e.X - movingpoint.X) / zoom);
                int deltay = (int)(-1 * (e.Y - movingpoint.Y) / zoom);
                movingpoint.X = e.X;
                movingpoint.Y = e.Y;
                hScrollBar.Value = Math.Max(0, Math.Min(hScrollBar.Maximum, hScrollBar.Value + deltax));
                vScrollBar.Value = Math.Max(0, Math.Min(vScrollBar.Maximum, vScrollBar.Value + deltay));
                pictureBox.Invalidate();
            }
            else if (e.Button == MouseButtons.None)
            {
                Rune curr_rune = toolTip.Tag as Rune;
                Region curr_region = toolTip.Tag as Region;
                ClientObject curr_client = toolTip.Tag as ClientObject;
                if (showRunesToolStripMenuItem.Checked)
                {
                    if (curr_rune != null)
                    {
                        if (curr_rune.isHovered(e, currmap) && (Math.Abs(oldhover.X - e.X) < 5 && Math.Abs(oldhover.Y - e.Y) < 5))
                              return;
                    }
                    foreach (OverlayObject rune in AllRunes)
                    {
                        if (rune.isHovered(e, currmap))
                        {
                            toolTip.RemoveAll();
                            toolTip.SetToolTip(pictureBox, rune.ToString());
                            toolTip.Tag = rune;
                            pictureBox.Invalidate();
                            lastHoverPoint.X = e.X;
                            lastHoverPoint.Y = e.Y;
                            return;
                        }
                    }
                }

                if (clientcursor.isHovered(e, currmap))
                {
                    if ((curr_client != null) && (Math.Abs(oldhover.X - e.X) < 5 && Math.Abs(oldhover.Y - e.Y) < 5))
                    {
                        return;
                    }
                    toolTip.RemoveAll();
                    toolTip.SetToolTip(pictureBox, clientcursor.ToString());
                    toolTip.Tag = clientcursor;
                    pictureBox.Invalidate();
                    lastHoverPoint.X = e.X;
                    lastHoverPoint.Y = e.Y;
                    return;
                }

                if (showRegionsToolStripMenuItem.Checked)
                {
                    if (curr_region != null)
                    {
                        if (curr_region.isHovered(e, currmap) && (Math.Abs(oldhover.X - e.X) < 5 && Math.Abs(oldhover.Y - e.Y) < 5))
                            return;
                    }
                    foreach (OverlayObject region in AllRegion)
                    {
                        if (region.isHovered(e, currmap))
                        {
                            toolTip.SetToolTip(pictureBox, region.ToString());
                            toolTip.Tag = region;
                            pictureBox.Invalidate();
                            lastHoverPoint.X = e.X;
                            lastHoverPoint.Y = e.Y;
                            return;
                        }
                    }
                }
                

                if (curr_rune!=null || curr_region!=null || curr_client !=null)
                {
                    toolTip.Tag = null;
                    toolTip.RemoveAll();
                    pictureBox.Invalidate();
                    lastHoverPoint.X = e.X;
                    lastHoverPoint.Y = e.Y;
                }
            }
        }

        private void OnMouseUpMap(object sender, MouseEventArgs e)
        {
            moving = false;
            this.Cursor = Cursors.Default;
        }

        private void SetScrollBarValues()
        {
            vScrollBar.Minimum = 0;
            hScrollBar.Minimum = 0;
            ChangeScrollBar();
            hScrollBar.LargeChange = 40;
            hScrollBar.SmallChange = 8;
            vScrollBar.LargeChange = 40;
            vScrollBar.SmallChange = 8;
            vScrollBar.Value = 0;
            hScrollBar.Value = 0;
        }

        private void ChangeScrollBar()
        {
            hScrollBar.Maximum = (int)(currmap.Width);
            hScrollBar.Maximum -= Round((int)(pictureBox.ClientSize.Width / zoom) - 8);
            if (zoom >= 1)
                hScrollBar.Maximum += (int)(40 * zoom);
            else if (zoom < 1)
                hScrollBar.Maximum += (int)(40 / Zoom);
            hScrollBar.Maximum = Math.Max(0, Round(hScrollBar.Maximum));
            vScrollBar.Maximum = (int)(currmap.Height);
            vScrollBar.Maximum -= Round((int)(pictureBox.ClientSize.Height / zoom) - 8);
            if (zoom >= 1)
                vScrollBar.Maximum += (int)(40 * zoom);
            else if (zoom < 1)
                vScrollBar.Maximum += (int)(40 / zoom);
            vScrollBar.Maximum = Math.Max(0, Round(vScrollBar.Maximum));
        }

        public int Round(int x)
        {
            return (int)((x >> 3) << 3);
        }

        private void ZoomMap(ref Bitmap bmp0)
        {
            Bitmap bmp1 = new Bitmap((int)(map.Width * zoom), (int)(map.Height * zoom));
            Graphics graph = Graphics.FromImage(bmp1);
            graph.InterpolationMode = InterpolationMode.NearestNeighbor;
            graph.PixelOffsetMode = PixelOffsetMode.Half;
            graph.DrawImage(bmp0, new Rectangle(0, 0, bmp1.Width, bmp1.Height));
            graph.Dispose();
            bmp0 = bmp1;
        }

        private void OnClickSyncButton(object sender, EventArgs e)
        {
            return; // TODO erstmal wieder ausgebaut
            if (MessageBox.Show("Sicher das die Daten aus dem Netz gezogen werden sollen?", "Sync", MessageBoxButtons.OKCancel) == DialogResult.Cancel)
                return;

            // used to build entire input
            StringBuilder sb = new StringBuilder();

            // used on each read operation
            byte[] buf = new byte[8192];

            // prepare the web page we will be asking for
            HttpWebRequest request = (HttpWebRequest)
                WebRequest.Create("daten_spawninfos.xml");

            // execute the request
            HttpWebResponse response = (HttpWebResponse)
                request.GetResponse();

            // we will read data via the response stream
            Stream resStream = response.GetResponseStream();

            string tempString = null;
            int count = 0;

            do
            {
                // fill the buffer with data
                count = resStream.Read(buf, 0, buf.Length);

                // make sure we read some data
                if (count != 0)
                {
                    // translate from bytes to ASCII text
                    tempString = Encoding.ASCII.GetString(buf, 0, count);

                    // continue building the string
                    sb.Append(tempString);
                }
            }
            while (count > 0); // any more data to read?

            string path = FiddlerControls.Options.AppDataPath;
            string FileName = Path.Combine(path, @"plugins/PergonSpawnNet.xml");
            string FileNameBak = Path.Combine(path, @"plugins/PergonSpawnNet.xml.bak");
            if (File.Exists(FileName))
            {
                if (File.Exists(FileNameBak))
                    File.Delete(FileNameBak);
                File.Move(FileName, FileNameBak);
            }
            using (StreamWriter sw = new StreamWriter(FileName))
            {
                sw.Write(sb.ToString());
            }
            LoadData();
        }

        private void OnZoomPlus(object sender, EventArgs e)
        {
            zoom *= 2;
            DoZoom();
        }

        private void OnZoomMinus(object sender, EventArgs e)
        {
            zoom /= 2;
            DoZoom();
        }

        private void DoZoom()
        {
            ChangeScrollBar();
            ZoomLabel.Text = String.Format("Zoom: {0}", zoom);
            int x, y;
            x = Math.Max(0, currPoint.X - (int)(pictureBox.ClientSize.Width / zoom) / 2);
            y = Math.Max(0, currPoint.Y - (int)(pictureBox.ClientSize.Height / zoom) / 2);
            x = Math.Min(x, hScrollBar.Maximum);
            y = Math.Min(y, vScrollBar.Maximum);
            hScrollBar.Value = Round(x);
            vScrollBar.Value = Round(y);
            pictureBox.Invalidate();
        }

        private void ChangeMapNames()
        {
            if (!Loaded)
                return;
            feluccaToolStripMenuItem.Text = FiddlerControls.Options.MapNames[0];
            trammelToolStripMenuItem.Text = FiddlerControls.Options.MapNames[1];
            ilshenarToolStripMenuItem.Text = FiddlerControls.Options.MapNames[2];
            malasToolStripMenuItem.Text = FiddlerControls.Options.MapNames[3];
            tokunoToolStripMenuItem.Text = FiddlerControls.Options.MapNames[4];
            terMurToolStripMenuItem.Text = FiddlerControls.Options.MapNames[5];
        }

        private void OnChangeMapFelucca(object sender, EventArgs e)
        {
            if (!feluccaToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                feluccaToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.Felucca;
                currmapint = 0;
                ChangeMap();
            }
        }

        private void OnChangeMapTrammel(object sender, EventArgs e)
        {
            if (!trammelToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                trammelToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.Trammel;
                currmapint = 1;
                ChangeMap();
            }
        }

        private void OnChangeMapIlshenar(object sender, EventArgs e)
        {
            if (!ilshenarToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                ilshenarToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.Ilshenar;
                currmapint = 2;
                ChangeMap();
            }
        }

        private void OnChangeMapMalas(object sender, EventArgs e)
        {
            if (!malasToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                malasToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.Malas;
                currmapint = 3;
                ChangeMap();
            }
        }

        private void OnChangeMapTokuno(object sender, EventArgs e)
        {
            if (!tokunoToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                tokunoToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.Tokuno;
                currmapint = 4;
                ChangeMap();
            }
        }

        private void OnChangeMapTerMur(object sender, EventArgs e)
        {
            if (!terMurToolStripMenuItem.Checked)
            {
                ResetCheckedMap();
                terMurToolStripMenuItem.Checked = true;
                currmap = Ultima.Map.TerMur;
                currmapint = 5;
                ChangeMap();
            }
        }

        private void ChangeMap()
        {
            SetScrollBarValues();
            pictureBox.Invalidate();
        }

        private void ResetCheckedMap()
        {
            feluccaToolStripMenuItem.Checked = false;
            trammelToolStripMenuItem.Checked = false;
            malasToolStripMenuItem.Checked = false;
            ilshenarToolStripMenuItem.Checked = false;
            tokunoToolStripMenuItem.Checked = false;
            terMurToolStripMenuItem.Checked = false;
        }

        private void OnContextClosed(object sender, ToolStripDropDownClosedEventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void OnDropDownClosed(object sender, EventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void onKeyDownGoto(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                string line = TextBoxGoto.Text.Trim();
                if (line.Length > 0)
                {
                    string[] args = line.Split(' ');
                    if (args.Length != 2)
                        args = line.Split(',');
                    if (args.Length == 2)
                    {
                        int x, y;
                        if (int.TryParse(args[0], out x) && (int.TryParse(args[1], out y)))
                        {
                            if ((x >= 0) && (y >= 0))
                            {
                                if ((x <= currmap.Width) && (x <= currmap.Height))
                                {
                                    contextMenuStrip1.Close();
                                    hScrollBar.Value = (int)Math.Max(0, x - pictureBox.Width / zoom / 2);
                                    vScrollBar.Value = (int)Math.Max(0, y - pictureBox.Height / zoom / 2);
                                }
                            }
                        }
                    }
                }
                pictureBox.Invalidate();
            }
        }


        #endregion

        #region Runestuff
        private enum Sorting
        {
            Koords = 0,
            Template = 1
        }
        public static List<Rune> AllRunes = new List<Rune>();
        public static List<Region> AllRegion = new List<Region>();

        private int curr_sorting;
        public int Curr_Sorting
        {
            get { return curr_sorting; }
            set
            {
                if (curr_sorting == value)
                    return;
                curr_sorting = value;
                refmarker.listBox.BeginUpdate();
                refmarker.listBox.Items.Clear();
                AllRunes.Sort();
                refmarker.listBox.Items.AddRange(AllRunes.ToArray());
                refmarker.listBox.Items.AddRange(AllRegion.ToArray());
                refmarker.listBox.EndUpdate();
            }
        }
        private OverlayObject Selected;

        private void LoadData()
        {
            string path = FiddlerControls.Options.AppDataPath;

            string FileName = Path.Combine(path, @"plugins/PergonSpawnNet.xml");
            if (!(File.Exists(FileName)))
                return;

            Rune r = null;
            Region re = null;
            AllRunes = new List<Rune>();

            XmlDocument document = new XmlDocument();
            document.Load(FileName);

            string s;
            XmlNodeList list = null;
            list = document.GetElementsByTagName("rune");
            foreach (XmlNode n in list)
            {
                r = new Rune(this);
                r.Map = Ultima.Map.Felucca;
                s = n["x"].InnerText;
                r.X = XmlConvert.ToInt32(s);
                s = n["y"].InnerText;
                r.Y = XmlConvert.ToInt32(s);
                s = n["template"].InnerText;
                r.Template = s;
                s = n["amount"].InnerText;
                r.Amount = XmlConvert.ToInt32(s);
                s = n["range"].InnerText;
                r.Range = XmlConvert.ToInt32(s);
                s = n["mintime"].InnerText;
                r.MinTime = XmlConvert.ToInt32(s);
                s = n["maxtime"].InnerText;
                r.MaxTime = XmlConvert.ToInt32(s);
                if (n["group"] != null)
                {
                    s = n["group"].InnerText;
                    if (s != null)
                        r.Group = s;
                }
                s = n["flags"].InnerText;
                r.Flags = s;
                s = n["spawntype"].InnerText;
                r.SpawnType = s;
                s = n["stackamount"].InnerText;
                r.StackAmount = XmlConvert.ToInt32(s);

                if (n["containertype"] != null)
                {
                    s = n["containertype"].InnerText;
                    if (s != null)
                        r.ContainerType = s;
                }
                if (n["note"] != null)
                {
                    s = n["note"].InnerText;
                    if (s != null)
                        r.Note = s;
                }
                s = n["questnr"].InnerText;
                r.QuestNr = XmlConvert.ToInt32(s);
                s = n["grouping"].InnerText;
                r.Grouping = XmlConvert.ToInt32(s);

                AllRunes.Add(r);
            }
            list = null;
            list = document.GetElementsByTagName("region");
            foreach (XmlNode n in list)
            {
                re = new Region(this);
                re.Map = Ultima.Map.Felucca;
                s = n["x1"].InnerText;
                re.X1 = XmlConvert.ToInt32(s);
                s = n["x2"].InnerText;
                re.X2 = XmlConvert.ToInt32(s);
                s = n["y1"].InnerText;
                re.Y1 = XmlConvert.ToInt32(s);
                s = n["y2"].InnerText;
                re.Y2 = XmlConvert.ToInt32(s);
                s = n["name"].InnerText;
                re.Name = s;
                s = n["mintime"].InnerText;
                re.MinTime = XmlConvert.ToInt32(s);
                s = n["maxtime"].InnerText;
                re.MaxTime = XmlConvert.ToInt32(s);
                foreach (XmlElement xmlgroup in n.SelectNodes("group"))
                {
                    Group gr = new Group();

                    gr.Name = xmlgroup.SelectSingleNode("name").InnerText;
                    gr.MaxAmount = Int32.Parse(xmlgroup.SelectSingleNode("max_amt").InnerText);
                    if (Int32.Parse(xmlgroup.SelectSingleNode("enabled").InnerText) == 1)
                        gr.Enabled = true;
                    else
                        gr.Enabled = false;
                    gr.Spawnfactor = Double.Parse(xmlgroup.SelectSingleNode("spawn_factor").InnerText);
                    gr.ContainerObjtype = Int32.Parse(xmlgroup.SelectSingleNode("containerobjtype").InnerText);
                    gr.Flags = xmlgroup.SelectSingleNode("flags").InnerText;
                    gr.GroupingAmt = Int32.Parse(xmlgroup.SelectSingleNode("groupingamt").InnerText);
                    gr.Spawnfactor = Int32.Parse(xmlgroup.SelectSingleNode("spawntype").InnerText);
                    gr.StackAmount = Int32.Parse(xmlgroup.SelectSingleNode("stackamount").InnerText);

                    gr.Templates = new List<Template>();
                    foreach (XmlElement xmltemplate in xmlgroup.SelectNodes("template"))
                    {
                        Template temp = new Template();
                        temp.name = xmltemplate.InnerText;
                        temp.procent = Int32.Parse(xmltemplate.GetAttribute("procent"));
                        gr.Templates.Add(temp);
                    }
                    //template

                    re.Spawns.Add(gr);
                }

                AllRegion.Add(re);
            }
            listBox.BeginUpdate();
            listBox.Items.Clear();
            AllRunes.Sort();

            foreach (Rune rune in AllRunes)
            {
                listBox.Items.Add(rune);
            }
            foreach (Region region in AllRegion)
            {
                listBox.Items.Add(region);
            }
            listBox.EndUpdate();
            pictureBox.Invalidate();
        }
        #endregion

        private void OnOpenContext(object sender, CancelEventArgs e)
        {
            currPoint = pictureBox.PointToClient(Control.MousePosition);
            currPoint.X = (int)(currPoint.X / Zoom);
            currPoint.Y = (int)(currPoint.Y / Zoom);
            currPoint.X += Round(hScrollBar.Value);
            currPoint.Y += Round(vScrollBar.Value);
        }

        private void OnSelectedChange(object sender, EventArgs e)
        {
            if (Selected != null)
                Selected.Selected = false;
            if (listBox.SelectedItem != null)
            {
                OverlayObject obj = (OverlayObject)listBox.SelectedItem;
                obj.Selected = true;
                Selected = obj;
                propertyGrid1.SelectedObject = obj;
            }
            else
                propertyGrid1.SelectedObject = null;
            listBox.Invalidate();
            pictureBox.Invalidate();

        }

        private void OnMouseDoubleClickListBox(object sender, MouseEventArgs e)
        {
            Rune rune = listBox.SelectedItem as Rune;
            if (rune != null)
            {
                hScrollBar.Value = (int)Math.Max(0, rune.X - pictureBox.Width / zoom / 2);
                vScrollBar.Value = (int)Math.Max(0, rune.Y - pictureBox.Height / zoom / 2);
            }
            else
            {
                Region region = listBox.SelectedItem as Region;
                if (region != null)
                {
                    hScrollBar.Value = (int)Math.Max(0, region.X1 + (region.X2 - region.X1) / 2 - pictureBox.Width / zoom / 2);
                    vScrollBar.Value = (int)Math.Max(0, region.Y1 + (region.Y2 - region.Y1) / 2 - pictureBox.Height / zoom / 2);
                }
            }
            pictureBox.Invalidate();

        }

        private void OnCheckedChangeRegions(object sender, EventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void OnCheckedChange(object sender, EventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void OnSortingChanged(object sender, EventArgs e)
        {
            Curr_Sorting = sortByNameToolStripMenuItem.Checked ? 1 : 0;
        }

        private Brush BrushNPC = Brushes.Gray;
        private Brush BrushItem = Brushes.Gold;
        private Brush BrushCont = Brushes.Green;
        private Brush BrushRegion = Brushes.Blue;
        private void ListBoxDrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index < 0)
                return;
            if (listBox.SelectedIndex == e.Index)
                e.Graphics.FillRectangle(Brushes.LightSteelBlue, e.Bounds.X, e.Bounds.Y, e.Bounds.Width, e.Bounds.Height);

            Rune temp = listBox.Items[e.Index] as Rune;
            if (temp == null)
            {
                Region tempre = listBox.Items[e.Index] as Region;
                e.Graphics.DrawString(tempre.ToString(), Font, BrushRegion, new PointF(0, e.Bounds.Y));
            }
            else if (temp.SpawnType == "NPC")
                e.Graphics.DrawString(temp.ToString(), Font, BrushNPC, new PointF(0, e.Bounds.Y));
            else if (temp.SpawnType == "Item")
                e.Graphics.DrawString(temp.ToString(), Font, BrushItem, new PointF(0, e.Bounds.Y));
            else if (temp.SpawnType == "Container")
                e.Graphics.DrawString(temp.ToString(), Font, BrushCont, new PointF(0, e.Bounds.Y));
        }

        private void OnClickShowRanges(object sender, EventArgs e)
        {
            pictureBox.Invalidate();
        }

        private void OnClickExtractBmp(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            string path = FiddlerControls.Options.OutputPath;
            string name = String.Format(@"plugins/{0}_{1}.bmp", "PergonSpawnNet", FiddlerControls.Options.MapNames[currmapint]);
            string FileName = Path.Combine(path, name);
            Bitmap extract = currmap.GetImage(0, 0, (currmap.Width >> 3), (currmap.Height >> 3), true);
            Graphics g = Graphics.FromImage(extract);

            double oldZoom = Zoom;
            int oldHvalue = hScrollBar.Value;
            int oldVvalue = vScrollBar.Value;
            zoom = 1;
            hScrollBar.Value = 0;
            vScrollBar.Value = 0;
            Rectangle rect = new Rectangle(0, 0, currmap.Width, currmap.Height);
            if (AllRunes.Count > 0)
            {
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (Rune rune in AllRunes)
                    {
                        if (rune.isVisible(rect, currmap))
                            rune.Draw(g);
                    }
                }
            }
            if (AllRegion.Count > 0)
            {
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (Region region in AllRegion)
                    {
                        if (region.isVisible(rect, currmap))
                            region.Draw(g);
                    }
                }
            }
            zoom = oldZoom;
            hScrollBar.Value = oldHvalue;
            vScrollBar.Value = oldVvalue;
            g.Save();
            extract.Save(FileName, ImageFormat.Bmp);
            Cursor.Current = Cursors.Default;
            MessageBox.Show(String.Format("Map saved to {0}", FileName), "Saved",
                MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
        }

        private void OnClickExtractTiff(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            string path = FiddlerControls.Options.OutputPath;
            string name = String.Format(@"plugins/{0}_{1}.tiff", "PergonSpawnNet", FiddlerControls.Options.MapNames[currmapint]);
            string FileName = Path.Combine(path, name);
            Bitmap extract = currmap.GetImage(0, 0, (currmap.Width >> 3), (currmap.Height >> 3), true);
            Graphics g = Graphics.FromImage(extract);

            double oldZoom = Zoom;
            int oldHvalue = hScrollBar.Value;
            int oldVvalue = vScrollBar.Value;
            zoom = 1;
            hScrollBar.Value = 0;
            vScrollBar.Value = 0;
            Rectangle rect = new Rectangle(0, 0, currmap.Width, currmap.Height);
            if (AllRunes.Count > 0)
            {
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (Rune rune in AllRunes)
                    {
                        if (rune.isVisible(rect, currmap))
                            rune.Draw(g);
                    }
                }
            }
            if (AllRegion.Count > 0)
            {
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (Region region in AllRegion)
                    {
                        if (region.isVisible(rect, currmap))
                            region.Draw(g);
                    }
                }
            }
            zoom = oldZoom;
            hScrollBar.Value = oldHvalue;
            vScrollBar.Value = oldVvalue;
            g.Save();
            extract.Save(FileName, ImageFormat.Tiff);
            Cursor.Current = Cursors.Default;
            MessageBox.Show(String.Format("Map saved to {0}", FileName), "Saved",
                MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
        }

        private void OnClickExtractJpg(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            string path = FiddlerControls.Options.OutputPath;
            string name = String.Format(@"plugins/{0}_{1}.jpg", "PergonSpawnNet", FiddlerControls.Options.MapNames[currmapint]);
            string FileName = Path.Combine(path, name);
            Bitmap extract = currmap.GetImage(0, 0, (currmap.Width >> 3), (currmap.Height >> 3), true);
            Graphics g = Graphics.FromImage(extract);

            double oldZoom = Zoom;
            int oldHvalue = hScrollBar.Value;
            int oldVvalue = vScrollBar.Value;
            zoom = 1;
            hScrollBar.Value = 0;
            vScrollBar.Value = 0;
            Rectangle rect = new Rectangle(0, 0, currmap.Width, currmap.Height);
            if (AllRunes.Count > 0)
            {
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (Rune rune in AllRunes)
                    {
                        if (rune.isVisible(rect, currmap))
                            rune.Draw(g);
                    }
                }
            }
            if (AllRegion.Count > 0)
            {
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (Region region in AllRegion)
                    {
                        if (region.isVisible(rect, currmap))
                            region.Draw(g);
                    }
                }
            }
            zoom = oldZoom;
            hScrollBar.Value = oldHvalue;
            vScrollBar.Value = oldVvalue;
            g.Save();
            extract.Save(FileName, ImageFormat.Jpeg);
            Cursor.Current = Cursors.Default;
            MessageBox.Show(String.Format("Map saved to {0}", FileName), "Saved",
                MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
        }

        private void OnClickExtractPng(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            string path = FiddlerControls.Options.OutputPath;
            string name = String.Format(@"plugins/{0}_{1}.png", "PergonSpawnNet", FiddlerControls.Options.MapNames[currmapint]);
            string FileName = Path.Combine(path, name);
            Bitmap extract = currmap.GetImage(0, 0, (currmap.Width >> 3), (currmap.Height >> 3), true);
            Graphics g = Graphics.FromImage(extract);

            double oldZoom = Zoom;
            int oldHvalue = hScrollBar.Value;
            int oldVvalue = vScrollBar.Value;
            zoom = 1;
            hScrollBar.Value = 0;
            vScrollBar.Value = 0;
            Rectangle rect = new Rectangle(0, 0, currmap.Width, currmap.Height);
            if (AllRunes.Count > 0)
            {
                if (showRunesToolStripMenuItem.Checked)
                {
                    foreach (Rune rune in AllRunes)
                    {
                        if (rune.isVisible(rect, currmap))
                            rune.Draw(g);
                    }
                }
            }
            if (AllRegion.Count > 0)
            {
                if (showRegionsToolStripMenuItem.Checked)
                {
                    foreach (Region region in AllRegion)
                    {
                        if (region.isVisible(rect, currmap))
                            region.Draw(g);
                    }
                }
            }
            zoom = oldZoom;
            hScrollBar.Value = oldHvalue;
            vScrollBar.Value = oldVvalue;
            g.Save();
            extract.Save(FileName, ImageFormat.Png);
            Cursor.Current = Cursors.Default;
            MessageBox.Show(String.Format("Map saved to {0}", FileName), "Saved",
                MessageBoxButtons.OK, MessageBoxIcon.Information, MessageBoxDefaultButton.Button1);
        }

        private void SyncClientTimer(object sender, EventArgs e)
        {
            if (showClientLocToolStripMenuItem1.Checked)
            {
                int x = 0;
                int y = 0;
                int z = 0;
                int mapClient = 0;
                string mapname = "";
                if (Ultima.Client.Running)
                {
                    Ultima.Client.Calibrate();
                    if (Ultima.Client.FindLocation(ref x, ref y, ref z, ref mapClient))
                    {
                        if ((clientcursor.X == x) && (clientcursor.Y == y) && (clientcursor.Z == z) && (clientcursor.IntMap == mapClient))
                            return;
                        clientcursor.X = x;
                        clientcursor.Y = y;
                        clientcursor.Z = z;
                        clientcursor.IntMap = mapClient;
                        switch (mapClient)
                        {
                            case 0:
                                clientcursor.Map = Ultima.Map.Felucca;
                                break;
                            case 1:
                                clientcursor.Map = Ultima.Map.Trammel;
                                break;
                            case 2:
                                clientcursor.Map = Ultima.Map.Ilshenar;
                                break;
                            case 3:
                                clientcursor.Map = Ultima.Map.Malas;
                                break;
                            case 4:
                                clientcursor.Map = Ultima.Map.Tokuno;
                                break;
                            case 5:
                                clientcursor.Map = Ultima.Map.TerMur;
                                break;
                        }
                        mapname = FiddlerControls.Options.MapNames[mapClient];
                    }
                }

                ClientLocLabel.Text = String.Format("ClientLoc: {0},{1},{2},{3}", x, y, z, mapname);
                pictureBox.Invalidate();
            }
        }

        private void OnClickGotoClient(object sender, EventArgs e)
        {
            int x = 0;
            int y = 0;
            int z = 0;
            int mapClient = 0;
            if (!Ultima.Client.Running)
                return;
            Ultima.Client.Calibrate();
            if (!Ultima.Client.FindLocation(ref x, ref y, ref z, ref mapClient))
                return;
            if (currmapint != mapClient)
            {
                ResetCheckedMap();
                switch (mapClient)
                {
                    case 0:
                        feluccaToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.Felucca;
                        break;
                    case 1:
                        trammelToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.Trammel;
                        break;
                    case 2:
                        ilshenarToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.Ilshenar;
                        break;
                    case 3:
                        malasToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.Malas;
                        break;
                    case 4:
                        tokunoToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.Tokuno;
                        break;
                    case 5:
                        terMurToolStripMenuItem.Checked = true;
                        currmap = Ultima.Map.TerMur;
                        break;
                }
                currmapint = mapClient;
            }
            clientcursor.X = x;
            clientcursor.Y = y;
            clientcursor.Z = z;
            clientcursor.IntMap = mapClient;
            clientcursor.Map = currmap;
            SetScrollBarValues();
            hScrollBar.Value = (int)Math.Max(0, x - pictureBox.Width / Zoom / 2);
            vScrollBar.Value = (int)Math.Max(0, y - pictureBox.Height / Zoom / 2);
            pictureBox.Invalidate();
            ClientLocLabel.Text = String.Format("ClientLoc: {0},{1},{2},{3}", x, y, z, FiddlerControls.Options.MapNames[mapClient]);
        }

        private void SendCharTo(int x, int y)
        {
            string format = "{0} " + FiddlerControls.Options.MapArgs;
            int z = currmap.Tiles.GetLandTile(x, y).Z;
            Client.SendText(String.Format(format, FiddlerControls.Options.MapCmd, x, y, z, currmapint, FiddlerControls.Options.MapNames[currmapint]));
        }

        private void OnClientLocCheck(object sender, EventArgs e)
        {
            clientcursor.Visible = showClientLocToolStripMenuItem1.Checked;
        }

        private void onClickSendClientPos(object sender, EventArgs e)
        {
            if (Client.Running)
                SendCharTo(currPoint.X, currPoint.Y);
        }
    }

    #region Overlays
    public class OverlayObject
    {
        public virtual bool isVisible(Rectangle bounds, Map m) { return false; }
        public virtual bool isHovered(MouseEventArgs e, Map m) { return false; }
        public virtual void Draw(Graphics g) { }
        [Browsable(false)]
        public bool Visible { get; set; }
        [Browsable(false)]
        public bool Selected { get; set; }
    }
    #endregion

    public class ClientObject : OverlayObject
    {
        public int X { get; set; }
        public int Y { get; set; }
        public int Z { get; set; }
        public Map Map { get; set; }
        public int IntMap { get; set; }

        private SpawnViewer refmarker;
        private static Pen DrawPen;

        public ClientObject(SpawnViewer refer)
        {
            refmarker = refer;
            Visible = true;
            Selected = false;
            if (DrawPen == null)
                DrawPen = new Pen(new SolidBrush(Color.Yellow));
        }

        public override bool isVisible(Rectangle bounds, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            if ((X > refmarker.HScrollBar) &&
                (X < refmarker.HScrollBar + bounds.Width / refmarker.Zoom) &&
                (Y > refmarker.VScrollBar) &&
                (Y < refmarker.VScrollBar + bounds.Height / refmarker.Zoom))
                return true;
            return false;
        }

        public override bool isHovered(MouseEventArgs e, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            int x_ = (int)((X - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            if (((x_ - 4) < e.X) && ((x_ + 4) > e.X))
            {
                int y_ = (int)((Y - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
                if (((y_ - 4) < e.Y) && ((y_ + 4) > e.Y))
                    return true;
            }
            return false;
        }

        public override void Draw(Graphics g)
        {
            int x_ = (int)((X - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            int y_ = (int)((Y - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);

            g.DrawLine(DrawPen, x_ - 4, y_, x_ + 4, y_);
            g.DrawLine(DrawPen, x_, y_ - 4, x_, y_ + 4);
            g.DrawEllipse(DrawPen, x_ - 2, y_ - 2, 2 * 2, 2 * 2);
        }

        public override string ToString()
        {
            return String.Format("Client:{0},{1},{2}", X, Y, Z);
        }

    }
}

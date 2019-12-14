using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Ultima;

namespace PergonSpawnNet
{
    public class Rune : OverlayObject, IComparable
    {
        public enum FlagsEnum
        {
            GROUPING = 0x01,
            SAVE_OLD_ITEMS = 0x02,
            NPC_ANCHOR = 0x04,
            NPC_FROZEN = 0x08,
            ITEM_IN_CONTAINER_SPAWN = 0x10,
            CONTAINER_MOVING_SPAWN = 0x20,
            CONTAINER_FLUSH = 0x40,
            CONTAINER_TRAP = 0x80
        }
        private int x;
        private int y;
        private string template;
        private int amount;
        private int range;
        private int mintime;
        private int maxtime;
        private string group;
        private byte flags;
        private int spawntype;
        private string containertype;
        private string note;
        private int questnr;
        private int grouping;
        private int stackamount;

        [Category("Rune")]
        public string SpawnType
        {
            set { spawntype = Int32.Parse(value); }
            get
            {
                if (spawntype == 1)
                    return "NPC";
                if (spawntype == 2)
                    return "Item";
                if (spawntype == 3)
                    return "Container";
                return "";
            }
        }
        [Category("Rune")]
        public int X
        {
            set { x = value; }
            get { return x; }
        }
        [Category("Rune")]
        public int Y
        {
            set { y = value; }
            get { return y; }
        }
        [Category("Rune")]
        public string Template
        {
            set { template = value; }
            get { return template; }
        }
        [Category("Rune")]
        public int Range
        {
            set { range = value; }
            get { return range; }
        }
        [Category("Rune")]
        public int Amount
        {
            set { amount = value; }
            get { return amount; }
        }
        [Category("Rune")]
        public string Group
        {
            set { group = value; }
            get { return group; }
        }
        [Category("Rune")]
        public int MinTime
        {
            set { mintime = value; }
            get { return mintime; }
        }
        [Category("Rune")]
        public int MaxTime
        {
            set { maxtime = value; }
            get { return maxtime; }
        }

        private string temp;
        [Category("Rune")]
        public string Flags
        {
            set { flags = Byte.Parse(value); }
            get
            {
                temp = "";
                if ((flags & (byte)FlagsEnum.GROUPING) == 1)
                    temp += "Grouping,";
                if ((flags & (byte)FlagsEnum.SAVE_OLD_ITEMS) == 1)
                    temp += "SaveOldItems,";
                if ((flags & (byte)FlagsEnum.NPC_ANCHOR) == 1)
                    temp += "Anker,";
                if ((flags & (byte)FlagsEnum.NPC_FROZEN) == 1)
                    temp += "Frozen,";
                if ((flags & (byte)FlagsEnum.ITEM_IN_CONTAINER_SPAWN) == 1)
                    temp += "ItemInContainer,";
                if ((flags & (byte)FlagsEnum.CONTAINER_MOVING_SPAWN) == 1)
                    temp += "MovingSpawn,";
                if ((flags & (byte)FlagsEnum.CONTAINER_FLUSH) == 1)
                    temp += "Flush,";
                if ((flags & (byte)FlagsEnum.CONTAINER_TRAP) == 1)
                    temp += "Trap,";
                if (temp != "")
                    temp = temp.Remove(temp.Length - 1, 1);
                return temp;
            }
        }
        [Category("Rune")]
        public string ContainerType
        {
            set { containertype = value; }
            get { return containertype; }
        }
        [Category("Rune")]
        public string Note
        {
            set { note = value; }
            get { return note; }
        }
        [Category("Rune")]
        public int QuestNr
        {
            set { questnr = value; }
            get { return questnr; }
        }
        [Category("Rune")]
        public int Grouping
        {
            set { grouping = value; }
            get { return grouping; }
        }
        [Category("Rune")]
        public int StackAmount
        {
            set { stackamount = value; }
            get { return stackamount; }
        }
        [Browsable(false)]
        public Map Map { get; set; }

        public override string ToString()
        {
            if (refmarker.Curr_Sorting == 0)
                return String.Format("{0},{1} - {2}", x.ToString(), y.ToString(), template);
            else
                return String.Format("{0} - {1},{2}", template, x.ToString(), y.ToString());
        }

        #region IComparable Members

        public int CompareTo(object obj)
        {
            Rune g = obj as Rune;

            if (g == null)
                return 0;
            int compare;
            if (refmarker.Curr_Sorting == 0)
            {
                compare = X.CompareTo(g.X);
                if (compare == 0)
                {
                    compare = Y.CompareTo(g.Y);
                    if (compare == 0)
                        return Template.CompareTo(g.Template);
                    else
                        return compare; //y
                }
                return compare; //x
            }
            else
            {
                compare = Template.CompareTo(g.Template);
                if (compare == 0)
                {
                    compare = X.CompareTo(g.X);
                    if (compare == 0)
                        return Y.CompareTo(g.Y);
                    else
                        return compare;  //x
                }
                int int_;
                int int__;
                if (Int32.TryParse(Template, out int_))
                {
                    if (Int32.TryParse(g.Template, out int__))
                        return int_.CompareTo(int__);
                }
                return compare; //template
            }
        }

        #endregion


        private static Pen NPCPen;
        private static Pen ItemPen;
        private static Pen ContainerPen;
        private static Pen SelectedPen;
        private static SpawnViewer refmarker;

        public Rune(SpawnViewer refer)
        {
            refmarker = refer;
            Visible = true;
            Selected = false;
            if (NPCPen == null)
            {
                NPCPen = new Pen(new SolidBrush(Color.Gray));

            }
            if (ItemPen == null)
                ItemPen = new Pen(new SolidBrush(Color.Gold));
            if (ContainerPen == null)
                ContainerPen = new Pen(new SolidBrush(Color.Green));
            if (SelectedPen == null)
                SelectedPen = new Pen(new SolidBrush(Color.Red));
        }

        public override bool isVisible(Rectangle bounds, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            if ((x > refmarker.HScrollBar) &&
                (x < refmarker.HScrollBar + bounds.Width / refmarker.Zoom) &&
                (y > refmarker.VScrollBar) &&
                (y < refmarker.VScrollBar + bounds.Height / refmarker.Zoom))
                return true;
            return false;
        }

        public override bool isHovered(MouseEventArgs e, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            int x_ = (int)((x - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            if (((x_ - 4) < e.X) && ((x_ + 4) > e.X))
            {
                int y_ = (int)((y - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
                if (((y_ - 8) < e.Y) && ((y_ + 8) > e.Y))
                    return true;

            }
            return false;
        }

        public override void Draw(Graphics g)
        {
            int x_ = (int)((x - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            int y_ = (int)((y - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
            Pen pen;
            if (!Selected)
            {
                switch (spawntype)
                {
                    case 1: pen = NPCPen;
                        break;
                    case 2: pen = ItemPen;
                        break;
                    case 3: pen = ContainerPen;
                        break;
                    default: pen = NPCPen;
                        break;
                }
            }
            else
            {
                pen = SelectedPen;

            }
            if ((Selected) || (refmarker.PaintRanges))
            {
                pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
                g.DrawRectangle(pen, (float)(x_ - range * refmarker.Zoom),
                                     (float)(y_ - range * refmarker.Zoom),
                                     (float)(2 * range * refmarker.Zoom),
                                     (float)(2 * range * refmarker.Zoom));
                pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
            }


            g.DrawPolygon(pen, new Point[] { new Point(x_, y_ - 8), 
                                             new Point(x_ + 4, y_), 
                                             new Point(x_, y_ + 8), 
                                             new Point(x_ - 4, y_) });
            g.DrawLine(pen, x_ - 4, y_, x_ + 4, y_);
            g.DrawLine(pen, x_, y_ - 6, x_, y_ + 6);
        }
    }
}

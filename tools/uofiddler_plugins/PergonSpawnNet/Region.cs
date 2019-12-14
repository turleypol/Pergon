using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Ultima;

namespace PergonSpawnNet
{
    public class Region : OverlayObject, ICustomTypeDescriptor
    {
        public Region(SpawnViewer refer)
        {
            group = new List<Group>();
            refmarker = refer;
            Visible = true;
            Selected = false;
            if (RegionPen == null)
            {
                RegionBrush = new SolidBrush(Color.FromArgb(80, Color.Blue));
                RegionPen = new Pen(new SolidBrush(Color.Blue));
            }
            if (SelectedPen == null)
            {
                SelectedBrush = new SolidBrush(Color.FromArgb(80, Color.Red));
                SelectedPen = new Pen(new SolidBrush(Color.Red));
            }
        }
        private static SpawnViewer refmarker;
        private static Pen RegionPen;
        private static Pen SelectedPen;
        private static Brush RegionBrush;
        private static Brush SelectedBrush;
        private int x1;
        private int x2;
        private int y1;
        private int y2;
        private List<Group> group;
        private string name;
        private int mintime;
        private int maxtime;

        public int X1
        {
            set { x1 = value; }
            get { return x1; }
        }
        public int X2
        {
            set { x2 = value; }
            get { return x2; }
        }
        public int Y1
        {
            set { y1 = value; }
            get { return y1; }
        }
        public int Y2
        {
            set { y2 = value; }
            get { return y2; }
        }

        public override bool isVisible(Rectangle bounds, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            if ((x1 > refmarker.HScrollBar + bounds.Width / refmarker.Zoom) ||
                (x2 < refmarker.HScrollBar) ||
                (y1 > refmarker.VScrollBar + bounds.Height / refmarker.Zoom) ||
                (y2 < refmarker.VScrollBar))
                return false;
            return true;
        }

        public override bool isHovered(MouseEventArgs e, Map m)
        {
            if (!Visible)
                return false;
            if (Map != m)
                return false;
            int x_ = (int)((x1 - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            if (x_ < e.X)
            {
                x_ = (int)((x2 - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
                if (x_ > e.X)
                {
                    int y_ = (int)((y1 - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
                    if (y_ < e.Y)
                    {
                        y_ = (int)((y2 - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
                        if (y_ > e.Y)
                            return true;
                    }
                }
            }
            return false;
        }

        public override void Draw(Graphics g)
        {
            int x_ = (int)((x1 - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            int y_ = (int)((y1 - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
            int x2_ = (int)((x2 - refmarker.Round(refmarker.HScrollBar)) * refmarker.Zoom);
            int y2_ = (int)((y2 - refmarker.Round(refmarker.VScrollBar)) * refmarker.Zoom);
            Pen pen;
            Brush brush;
            if (!Selected)
            {
                pen = RegionPen;
                brush = RegionBrush;
            }
            else
            {
                pen = SelectedPen;
                brush = SelectedBrush;
            }
            g.DrawRectangle(pen, x_, y_, x2_ - x_, y2_ - y_);
            g.FillRectangle(brush, x_, y_, x2_ - x_, y2_ - y_);
        }
        public List<Group> Spawns
        {
            set { group = value; }
            get { return group; }
        }
        public string Name
        {
            set { name = value; }
            get { return name; }
        }
        public int MinTime
        {
            set { mintime = value; }
            get { return mintime; }
        }
        public int MaxTime
        {
            set { maxtime = value; }
            get { return maxtime; }
        }

        [Browsable(false)]
        public Rectangle rect
        {
            get { return new Rectangle(x1, y1, x2 - x1, y2 - y1); }
        }
        [Browsable(false)]
        public Map Map { get; set; }

        public override string ToString()
        {
            return String.Format("{0}", name);
        }

        // Implementation of interface ICustomTypeDescriptor 

        public String GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        public String GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(this, true);
        }

        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        public EventDescriptorCollection GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }


        /// <summary>
        /// Called to get the properties of this type. Returns properties with certain
        /// attributes. this restriction is not implemented here.
        /// </summary>
        /// <param name="attributes"></param>
        /// <returns></returns>
        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            return GetProperties();
        }

        /// <summary>
        /// Called to get the properties of this type.
        /// </summary>
        /// <returns></returns>
        public PropertyDescriptorCollection GetProperties()
        {

            List<PropertyDescriptor> properties = new List<PropertyDescriptor>();
            PropertyDescriptorCollection baseProps =
                                      TypeDescriptor.GetProperties(this, true);

            foreach (PropertyDescriptor oProp in baseProps)
            {
                if (oProp.Attributes[typeof(BrowsableAttribute)].Equals(BrowsableAttribute.No))
                    continue;
                if (oProp.Name != "Spawns")
                    properties.Add(new RegionPropertyDescriptor(oProp.Name, oProp.GetValue(this), "Region"));
            }

            for (int i = 0; i < this.group.Count; i++)
            {
                PropertyDescriptorCollection gProps =
                                      TypeDescriptor.GetProperties(this.group[i], true);
                foreach (PropertyDescriptor oProp in gProps)
                {
                    if (oProp.Name != "Templates")
                        properties.Add(new RegionPropertyDescriptor(oProp.Name, oProp.GetValue(this.group[i]), "Group" + i));
                    else
                    {
                        for (int j = 0; j < this.group[i].Templates.Count; j++)
                        {
                            properties.Add(new RegionPropertyDescriptor("Template" + j, this.group[i].Templates[j].ToString(), "Group" + i));
                        }
                    }
                }
            }
            PropertyDescriptor[] props = properties.ToArray();

            return new PropertyDescriptorCollection(props);
        }
    }

    public struct Group
    {
        public string Name { get; set; }
        public bool Enabled { get; set; }
        public int MaxAmount { get; set; }
        public double Spawnfactor { get; set; }
        public int ContainerObjtype { get; set; }
        private string temp;
        private byte flags;
        public string Flags
        {
            set { flags = Byte.Parse(value); }
            get
            {
                temp = "";
                if ((flags & (byte)Rune.FlagsEnum.GROUPING) == 1)
                    temp += "Grouping,";
                if ((flags & (byte)Rune.FlagsEnum.SAVE_OLD_ITEMS) == 1)
                    temp += "SaveOldItems,";
                if ((flags & (byte)Rune.FlagsEnum.NPC_ANCHOR) == 1)
                    temp += "Anker,";
                if ((flags & (byte)Rune.FlagsEnum.NPC_FROZEN) == 1)
                    temp += "Frozen,";
                if ((flags & (byte)Rune.FlagsEnum.ITEM_IN_CONTAINER_SPAWN) == 1)
                    temp += "ItemInContainer,";
                if ((flags & (byte)Rune.FlagsEnum.CONTAINER_MOVING_SPAWN) == 1)
                    temp += "MovingSpawn,";
                if ((flags & (byte)Rune.FlagsEnum.CONTAINER_FLUSH) == 1)
                    temp += "Flush,";
                if ((flags & (byte)Rune.FlagsEnum.CONTAINER_TRAP) == 1)
                    temp += "Trap,";
                if (temp != "")
                    temp = temp.Remove(temp.Length - 1, 1);
                return temp;
            }
        }
        public int GroupingAmt { get; set; }
        public int SpawnType { get; set; }
        public int StackAmount { get; set; }
        public List<Template> Templates { get; set; }
    }
    public struct Template
    {
        public string name;
        public int procent;
        public override string ToString()
        {
            return String.Format("{0} {1}%", name, procent);
        }
    }

    class RegionPropertyDescriptor : PropertyDescriptor
    {
        object value;
        object _key;
        string category;

        internal RegionPropertyDescriptor(object key, object d, string cat)
            : base(key.ToString(), null)
        {
            value = d;
            _key = key;
            category = cat;
        }

        public override string Category
        {
            get
            {
                return category;
            }
        }
        public override Type PropertyType
        {
            get { return typeof(string); }
        }

        public override void SetValue(object component, object value)
        {
            //_dictionary[_key] = value;
        }

        public override object GetValue(object component)
        {
            return value;
        }

        public override bool IsReadOnly
        {
            get { return false; }
        }

        public override Type ComponentType
        {

            get { return null; }
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }

        public override void ResetValue(object component)
        {
        }

        public override bool ShouldSerializeValue(object component)
        {
            return false;
        }

    }
}
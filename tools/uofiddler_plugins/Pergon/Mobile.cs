using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;
using ZedGraph;


namespace Pergon
{
    [XmlRoot("MobileList")]
    public class MobileList
    {
        private List<Mobile> listMobile;

        public MobileList()
        {
            listMobile = new List<Mobile>();
        }

        [XmlElement("mobile")]
        public Mobile[] mobile
        {
            get
            {
                Mobile[] mobiles = new Mobile[listMobile.Count];
                listMobile.CopyTo(mobiles);
                return mobiles;
            }
            set
            {
                if (value == null) return;
                Mobile[] mobiles = (Mobile[])value;
                listMobile.Clear();
                foreach (Mobile mobile in mobiles)
                    listMobile.Add(mobile);
            }
        }
        [XmlIgnore]
        public List<Mobile> mobileList
        {
            get { return listMobile; }
            set { listMobile = value; }
        }

        public void AddMobile(Mobile mobile)
        {
            listMobile.Add(mobile);
        }
    }

    public class Dice
    {
        public Dice() { }
        public Dice(int dmulti, int dtype, int dadd)
        {
            dicemulti = dmulti;
            dicetype = dtype;
            diceadd = dadd;
        }
        private int dicemulti;
        private int diceadd;
        private int dicetype;

        public int Dicemulti
        {
            get { return dicemulti; }
            set { dicemulti = value; }
        }
        public int Dicetype
        {
            get { return dicetype; }
            set { dicetype = value; }
        }
        public int Diceadd
        {
            get { return diceadd; }
            set { diceadd = value; }
        }

        public override string ToString()
        {
            return String.Format("{0}d{1}{2}", dicemulti, dicetype, (diceadd == 0) ? "" : "+" + diceadd.ToString());
        }
        public int Roll()
        {
            int roll = 0;
            for (int i = 0; i < dicemulti; ++i)
            {
                roll += Utility.Random(dicetype) + 1;
            }
            roll += diceadd;
            return roll;
        }
    }

    public class Mobile : IComparable
    {
        public Mobile()
        { dice = new Dice(); }

        private bool npc;
        //wer ist npc (anatomie/kritdmg zählen nicht)
        public bool Npc
        {
            get { return npc; }
            set { npc = value; }
        }
        private int hp;
        public int Hp
        {
            get { return hp; }
            set { hp = value; }
        }
        private int dex;
        public int Dex
        {
            get { return dex; }
            set { dex = value; }
        }
        private int ar;
        public int Ar
        {
            get { return ar; }
            set { ar = value; }
        }
        private int armod;
        public int ArMod
        {
            get { return armod; }
            set { armod = value; }
        }
        private int ausweichen;
        public int Ausweichen
        {
            get { return ausweichen; }
            set { ausweichen = value; }
        }
        private int shieldar;
        public int Shieldar
        {
            get { return shieldar; }
            set { shieldar = value; }
        }
        private int shieldskill;
        //0 kein schild
        public int Shieldskill
        {
            get { return shieldskill; }
            set { shieldskill = value; }
        }
        private int weaponskill;
        public int Weaponskill
        {
            get { return weaponskill; }
            set { weaponskill = value; }
        }
        private int str;
        public int Str
        {
            get { return str; }
            set { str = value; }
        }
        private int taktik;
        public int Taktik
        {
            get { return taktik; }
            set { taktik = value; }
        }
        private int anatomie;
        public int Anatomie
        {
            get { return anatomie; }
            set { anatomie = value; }
        }
        private int eledmg;
        public int Eledmg
        {
            get { return eledmg; }
            set { eledmg = value; }
        }
        private Dice dice;
        public Dice WeaponDice
        {
            get { return dice; }
            set { dice = value; }
        }
        private int weaponhp;
        //reale hp
        public int Weaponhp
        {
            get { return weaponhp; }
            set { weaponhp = value; }
        }
        private int weaponitemhp;
        //itemdesc hp
        public int Weaponitemhp
        {
            get { return weaponitemhp; }
            set { weaponitemhp = value; }
        }
        private int weaponcritchance;
        //wahrscheinlichkeit auf kritische Treffer CriticalHit in itemdesc
        public int Weaponcritchance
        {
            get { return weaponcritchance; }
            set { weaponcritchance = value; }
        }
        private int attackspeed;
        public int Attackspeed
        {
            get { return attackspeed; }
            set { attackspeed = value; }
        }
        private int eleresist;
        public int EResist
        {
            get { return eleresist; }
            set
            {
                eleresist = value;
                EleResist = 1 - (2.0 / (1 + Math.Exp(value / -50.0)) - 1);
            }
        }
        [XmlIgnore]
        public double EleResist { get; set; }

        private double meanDamage;
        [XmlIgnore]
        public double MeanDamage
        {
            get { return meanDamage; }
            set { meanDamage = value; }
        }

        public void CalcMeanDamage(PointPairList list)
        {
            meanDamage = 0;
            for (int i = 0; i < list.Count; ++i)
            {
                meanDamage += list[i].Y;
            }
            meanDamage /= list.Count;
        }

        private double hitchance;
        [XmlIgnore]
        public double Hitchance
        {
            get { return hitchance; }
            set { hitchance = value; }
        }

        public void CalcHitchance(int oppskill)
        {
            hitchance = (weaponskill + 50) / (2.0 * oppskill + 50);
        }

        private double swingspeed;
        [XmlIgnore]
        public double SwingSpeed
        {
            get { return swingspeed; }
            set { swingspeed = value; }
        }
        public void CalcSwingSpeed()
        {
            swingspeed = 15000.0 / ((dex + 100) * attackspeed);
        }

        private string name;
        [XmlIgnore]
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        private string xmlname;
        [XmlAttribute("name")]
        public string XMLName
        {
            get { return xmlname; }
            set { xmlname = value; }
        }

        public double StandardDeviation(PointPairList list)
        {
            double sd = 0.0;
            for (int i = 0; i < list.Count; ++i)
                sd += (list[i].Y - meanDamage) * (list[i].Y - meanDamage);
            sd /= list.Count;
            sd = Math.Sqrt(sd);
            return sd;
        }

        public override string ToString()
        {
            if (string.IsNullOrEmpty(xmlname))
                return "Mobile";
            return xmlname;
        }

        private int armorsetindex;
        public int ArmorSetIndex
        {
            get { return armorsetindex; }
            set { armorsetindex = value; }
        }

        private ArmorZone armorzone;
        [XmlIgnore]
        public ArmorZone Armorzone
        {
            get { return armorzone; }
            set { armorzone = value; }
        }

        public Zone ChooseArmor()
        {
            if (npc)
                return null;
            double f = Utility.RandomDouble() * Armorzone.ChanceSum;
            foreach (Zone z in Armorzone.Zones)
            {
                f -= z.Chance;
                if (f <= 0.0)
                    return z;
            }
            return null;
        }

        public PointPairList HitList()
        {
            PointPairList dmglist = new PointPairList();
            double time = 0;
            int count = 0;
            dmglist.Add(0, 0);
            while (time <= 60)
            {
                if (Utility.RandomDouble() < Hitchance)
                    dmglist.Add(time, dmglist[count++].Y + MeanDamage);
                else
                    dmglist.Add(time, dmglist[count++].Y);
                time += SwingSpeed;
            }
            return dmglist;
        }

        public int CompareTo(object x)
        {
            if (x == null)
                return 1;

            if (!(x is Mobile))
                throw new ArgumentNullException();

            Mobile a = (Mobile)x;
            return string.Compare(this.ToString(), a.ToString());
        }
    }

    public enum ZoneParts
    {
        BODY = 0,
        ARM,
        HEAD,
        LEGS,
        NECK,
        HANDS
    }

    public class ArmorZone
    {
        public Zone[] Zones { get; set; }
        public double ChanceSum { get; private set; }
        public bool HasArmor { get; set; }
        public ArmorZone()
        {
            HasArmor = true;
            Zones = new Zone[6];
            Zones[(int)ZoneParts.BODY] = new Zone();
            Zones[(int)ZoneParts.BODY].Chance = 0.44;
            Zones[(int)ZoneParts.BODY].Layers = new int[5] { 13, 20, 22, 5, 17 };
            Zones[(int)ZoneParts.ARM] = new Zone();
            Zones[(int)ZoneParts.ARM].Chance = 0.14;
            Zones[(int)ZoneParts.ARM].Layers = new int[1] { 19 };
            Zones[(int)ZoneParts.HEAD] = new Zone();
            Zones[(int)ZoneParts.HEAD].Chance = 0.14;
            Zones[(int)ZoneParts.HEAD].Layers = new int[1] { 6 };
            Zones[(int)ZoneParts.LEGS] = new Zone();
            Zones[(int)ZoneParts.LEGS].Chance = 0.14;
            Zones[(int)ZoneParts.LEGS].Layers = new int[3] { 4, 3, 24 };
            Zones[(int)ZoneParts.NECK] = new Zone();
            Zones[(int)ZoneParts.NECK].Chance = 0.07;
            Zones[(int)ZoneParts.NECK].Layers = new int[1] { 10 };
            Zones[(int)ZoneParts.HANDS] = new Zone();
            Zones[(int)ZoneParts.HANDS].Chance = 0.07;
            Zones[(int)ZoneParts.HANDS].Layers = new int[1] { 7 };
            ChanceSum = 0;
            foreach (Zone z in Zones)
            {
                ChanceSum += z.Chance;
            }
        }
    }
    public class Zone
    {
        public double Chance { get; set; }
        public int[] Layers { get; set; }
        public int Armor { get; set; }
        public Zone()
        {
            Armor = 0;
        }
    }

    [XmlRoot("ArmorSets")]
    public class ArmorSets
    {
        private List<ArmorSet> listSet;

        public ArmorSets()
        {
            listSet = new List<ArmorSet>();
        }

        [XmlElement("Set")]
        public ArmorSet[] sets
        {
            get
            {
                ArmorSet[] sets = new ArmorSet[listSet.Count];
                listSet.CopyTo(sets);
                return sets;
            }
            set
            {
                if (value == null) return;
                ArmorSet[] sets = (ArmorSet[])value;
                listSet.Clear();
                foreach (ArmorSet set in sets)
                    listSet.Add(set);
            }
        }
        [XmlIgnore]
        public List<ArmorSet> ArmorSetsList
        {
            get { return listSet; }
            set { listSet = value; }
        }

        public void AddSet(ArmorSet set)
        {
            listSet.Add(set);
        }
    }

    public class ArmorSet
    {
        [XmlAttribute("name")]
        public string Name { get; set; }
        private int body, arm, head, legs, neck, hands;
        public int Body { get { return body; } set { body = value; Armor[(int)ZoneParts.BODY] = value; } }
        public int Arm { get { return arm; } set { arm = value; Armor[(int)ZoneParts.ARM] = value; } }
        public int Head { get { return head; } set { head = value; Armor[(int)ZoneParts.HEAD] = value; } }
        public int Legs { get { return legs; } set { legs = value; Armor[(int)ZoneParts.LEGS] = value; } }
        public int Neck { get { return neck; } set { neck = value; Armor[(int)ZoneParts.NECK] = value; } }
        public int Hands { get { return hands; } set { hands = value; Armor[(int)ZoneParts.HANDS] = value; } }
        [XmlIgnore]
        public int[] Armor { get; set; }
        public ArmorSet()
        {
            Armor = new int[6];
        }

        public override string ToString()
        {
            if (Name == null)
                return "ArmorSet";
            return Name;
        }
    }
}

using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using System.Xml.Serialization;
using ZedGraph;

namespace Pergon
{
    public partial class CombatCalc : Form
    {
        public CombatCalc()
        {
            InitializeComponent();
            this.Icon = FiddlerControls.Options.GetFiddlerIcon();
            mobList = new MobileList();
            setList = new ArmorSets();
        }

        public static int Calculationtimes = 5000;
        private static string XMLpath = Path.Combine(FiddlerControls.Options.AppDataPath, @"plugins/pergon_combatcalcmobs.xml");
        private static string XMLpathSets = Path.Combine(FiddlerControls.Options.AppDataPath, @"plugins/pergon_combatcalcsets.xml");

        MobileList mobList;
        ArmorSets setList;
        XmlSerializer MobListSerializer;
        XmlSerializer ArmorSetSerializer;

        private void OnLoad(object sender, EventArgs e)
        {
            MobListSerializer = new XmlSerializer(typeof(MobileList));
            ArmorSetSerializer = new XmlSerializer(typeof(ArmorSets));
            if (File.Exists(XMLpath))
            {
                TextReader r = new StreamReader(XMLpath);
                mobList = (MobileList)MobListSerializer.Deserialize(r);
                mobList.mobileList.Sort();
                r.Close();
            }
            if (File.Exists(XMLpathSets))
            {
                TextReader r = new StreamReader(XMLpathSets);
                setList = (ArmorSets)ArmorSetSerializer.Deserialize(r);
                r.Close();
            }
            if (setList == null)
                setList = new ArmorSets();

            listView1.BeginUpdate();
            listView1.Items.Clear();
            foreach (Mobile mob in mobList.mobile)
            {
                ListViewItem item = new ListViewItem();
                item.Text = mob.ToString();
                listView1.Items.Add(item);
            }
            listView1.EndUpdate();
            comboBox1ArmorSet.BeginUpdate();
            comboBox2ArmorSet.BeginUpdate();
            comboBox1ArmorSet.Items.Clear();
            comboBox2ArmorSet.Items.Clear();
            foreach (ArmorSet aset in setList.sets)
            {
                comboBox1ArmorSet.Items.Add(aset);
                comboBox2ArmorSet.Items.Add(aset);
            }
            if (comboBox1ArmorSet.Items.Count > 0)
                comboBox1ArmorSet.SelectedIndex = 0;
            if (comboBox2ArmorSet.Items.Count > 0)
                comboBox2ArmorSet.SelectedIndex = 0;
            comboBox1ArmorSet.EndUpdate();
            comboBox2ArmorSet.EndUpdate();
            OnCheckedNPC1(this, EventArgs.Empty);
            OnCheckedNPC2(this, EventArgs.Empty);
        }

        private void Caluclate(object sender, EventArgs e)
        {
            Mobile mobile1 = new Mobile();
            Mobile mobile2 = new Mobile();
            try
            {
                mobile1.Npc = checkBox1NPC.Checked;
                mobile2.Npc = checkBox2NPC.Checked;
                mobile1.Hp = (int)numericUpDown1HP.Value;
                mobile2.Hp = (int)numericUpDown2HP.Value;
                mobile1.Dex = (int)numericUpDown1Dex.Value;
                mobile2.Dex = (int)numericUpDown2Dex.Value;
                mobile1.Ar = (int)numericUpDown1AR.Value;
                mobile2.Ar = (int)numericUpDown2AR.Value;
                mobile1.ArMod = (int)numericUpDown1ArMod.Value;
                mobile2.ArMod = (int)numericUpDown2ArMod.Value;
                mobile1.ArmorSetIndex = comboBox1ArmorSet.SelectedIndex;
                mobile2.ArmorSetIndex = comboBox2ArmorSet.SelectedIndex;
                mobile1.Ausweichen = (int)numericUpDown1Ausweichen.Value;
                mobile2.Ausweichen = (int)numericUpDown2Ausweichen.Value;
                mobile1.Shieldar = (int)numericUpDown1SchildAR.Value;
                mobile2.Shieldar = (int)numericUpDown2SchildAR.Value;
                mobile1.Shieldskill = (int)numericUpDown1SchildSkill.Value;
                mobile2.Shieldskill = (int)numericUpDown2SchildSkill.Value;
                mobile1.Weaponskill = (int)numericUpDown1WaffenSkill.Value;
                mobile2.Weaponskill = (int)numericUpDown2WaffenSkill.Value;
                mobile1.Str = (int)numericUpDown1Str.Value;
                mobile2.Str = (int)numericUpDown2Str.Value;
                mobile1.Taktik = (int)numericUpDown1Taktik.Value;
                mobile2.Taktik = (int)numericUpDown2Taktik.Value;
                mobile1.Anatomie = (int)numericUpDown1Anatomie.Value;
                mobile2.Anatomie = (int)numericUpDown2Anatomie.Value;
                mobile1.Eledmg = (int)numericUpDown1EleSchaden.Value;
                mobile2.Eledmg = (int)numericUpDown2EleSchaden.Value;
                mobile1.EResist = (int)numericUpDown1EleResist.Value;
                mobile2.EResist = (int)numericUpDown2EleResist.Value;
                mobile1.WeaponDice.Dicemulti = (int)numericUpDown1DiceMulti.Value;
                mobile1.WeaponDice.Dicetype = (int)numericUpDown1DiceType.Value;
                mobile1.WeaponDice.Diceadd = (int)numericUpDown1DiceAdd.Value;
                mobile2.WeaponDice.Dicemulti = (int)numericUpDown2DiceMulti.Value;
                mobile2.WeaponDice.Dicetype = (int)numericUpDown2DiceType.Value;
                mobile2.WeaponDice.Diceadd = (int)numericUpDown2DiceAdd.Value;
                mobile1.Weaponhp = (int)numericUpDown1WeaponHP.Value;
                mobile2.Weaponhp = (int)numericUpDown2WeaponHP.Value;
                mobile1.Weaponitemhp = (int)numericUpDown1WeaponItemdescHP.Value;
                mobile2.Weaponitemhp = (int)numericUpDown2WeaponItemdescHP.Value;
                mobile1.Weaponcritchance = (int)numericUpDown1WeaponCritchance.Value;
                mobile2.Weaponcritchance = (int)numericUpDown2WeaponCritchance.Value;
                mobile1.Attackspeed = (int)numericUpDown1Attackspeed.Value;
                mobile2.Attackspeed = (int)numericUpDown2AttackSpeed.Value;
                mobile1.XMLName = textBox1Name.Text;
                mobile2.XMLName = textBox2Name.Text;

                mobile1.CalcHitchance(mobile2.Weaponskill);
                mobile2.CalcHitchance(mobile1.Weaponskill);
                mobile1.CalcSwingSpeed();
                mobile2.CalcSwingSpeed();

                mobile1.Armorzone = new ArmorZone();
                mobile2.Armorzone = new ArmorZone();
                ArmorSet aset1 = (ArmorSet)comboBox1ArmorSet.Items[mobile1.ArmorSetIndex];
                ArmorSet aset2 = (ArmorSet)comboBox2ArmorSet.Items[mobile2.ArmorSetIndex];
                foreach (ZoneParts part in Enum.GetValues(typeof(ZoneParts)))
                {
                    mobile1.Armorzone.Zones[(int)part].Armor = aset1.Armor[(int)part];
                    mobile2.Armorzone.Zones[(int)part].Armor = aset2.Armor[(int)part];
                }

                if (mobile1.Npc)
                    mobile1.Name = "NPC (1)";
                else
                    mobile1.Name = "Spieler (1)";
                if (mobile2.Npc)
                    mobile2.Name = "NPC (2)";
                else
                    mobile2.Name = "Spieler (2)";

                CombatCalcResult RESULT = new CombatCalcResult();
                PointPairList dmg1 = new PointPairList();
                PointPairList dmg2 = new PointPairList();

                for (int _switch = 0; _switch < 2; _switch++)
                {
                    Mobile attacker;
                    Mobile defender;
                    PointPairList dmggraph;
                    GraphPane panel;
                    if (_switch == 0)
                    {
                        attacker = mobile1;
                        defender = mobile2;
                        dmggraph = dmg1;
                        panel = RESULT.Pane1;
                    }
                    else
                    {
                        attacker = mobile2;
                        defender = mobile1;
                        dmggraph = dmg2;
                        panel = RESULT.Pane2;
                    }
                    int nicht_getroffen = 0;
                    int anatomie = 0;
                    int schildboni = 0;
                    int ausweichen = 0;
                    int voll = 0;
                    int schwer = 0;
                    if (!Combat(attacker, defender, ref dmggraph, ref nicht_getroffen, ref anatomie, ref schildboni,
                        ref ausweichen, ref voll, ref schwer))
                        continue;
                    attacker.CalcMeanDamage(dmggraph);
                    panel.Legend.IsVisible = false;
                    panel.Title.Text = attacker.Name + " Schaden - " + attacker.XMLName;
                    string xtitle = String.Format("Durchschnitt {0:0.0000} Theo. Nicht getroffen {1} ({2:0.00}%) Standardabweichung {3:0.0000}", attacker.MeanDamage, nicht_getroffen, (double)nicht_getroffen / Calculationtimes * 100.0, attacker.StandardDeviation(dmggraph));
                    xtitle += Environment.NewLine;
                    xtitle += String.Format("Anz Ausweichen {0:0.00}% Anatomie {1:0.00}% Volltreffer {2:0.00}% Schwerer {3:0.00}% Schildboni {4:0.00}%", (double)ausweichen / Calculationtimes * 100.0, (double)anatomie / Calculationtimes * 100.0, (double)voll / Calculationtimes * 100.0, (double)schwer / Calculationtimes * 100.0, (double)schildboni / Calculationtimes * 100.0);
                    xtitle += Environment.NewLine + "Daten:" + Environment.NewLine;
                    xtitle += String.Format("Angreifer Str {0}, Taktik {1}, Anatomie {2}, Eledmg {3}", attacker.Str, attacker.Taktik, attacker.Anatomie, attacker.Eledmg);
                    xtitle += Environment.NewLine;
                    xtitle += String.Format("Waffe {0}, hp zu itemdesc {1}/{2} , Criticalhitchance {3}", attacker.WeaponDice, attacker.Weaponhp, attacker.Weaponitemhp, attacker.Weaponcritchance);
                    xtitle += Environment.NewLine;
                    xtitle += String.Format("Verteidiger Dex {0}, Auseichen {1}, ARSet {2}, AR {3}", defender.Dex, defender.Ausweichen, setList.sets[defender.ArmorSetIndex], defender.Ar);
                    xtitle += Environment.NewLine;
                    xtitle += String.Format("Schildkampf {0}, SchildAR {1}", defender.Shieldskill, defender.Shieldar);

                    panel.XAxis.Title.Text = xtitle;
                    panel.XAxis.Title.FontSpec.Size = 9;
                    panel.YAxis.Title.Text = "Dmg";
                    panel.XAxis.Scale.Max = Calculationtimes;

                    double maxy = 0;
                    for (int i = 0; i < dmggraph.Count; ++i)
                    {
                        if (maxy < dmggraph[i].Y)
                            maxy = dmggraph[i].Y;
                    }
                    panel.YAxis.Scale.Max = maxy * 1.01;


                    PointPairList dmg1mean = new PointPairList();
                    dmg1mean.Add(0, attacker.MeanDamage);
                    dmg1mean.Add(Calculationtimes - 1, attacker.MeanDamage);
                    LineItem myCurveMean = panel.AddCurve("", dmg1mean, Color.Green,
                                      SymbolType.None);
                    myCurveMean.Line.Style = System.Drawing.Drawing2D.DashStyle.Dash;
                    myCurveMean.Line.Width = 2f;

                    LineItem myCurve = panel.AddCurve("", dmggraph, attacker == mobile1 ? Color.Red : Color.Blue,
                                      SymbolType.None);

                    panel.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45F);
                    panel.Fill = new Fill(Color.White, Color.FromArgb(220, 220, 255), 45F);
                }

                GraphPane myPane3 = RESULT.Pane3;
                myPane3.Legend.IsVisible = false;

                PointPairList dmgcompare1 = new PointPairList();
                PointPairList dmgcompare2 = new PointPairList();
                int count;
                if (mobile2.MeanDamage == 0)
                    count = 10;
                else
                    count = (int)Math.Floor((mobile1.Hp + 100) / mobile2.MeanDamage + 2);
                dmgcompare2.Add(0, mobile1.Hp);
                dmgcompare1.Add(0, 0);
                for (int i = 1; i < count; ++i)
                {
                    dmgcompare2.Add(i, dmgcompare2[i - 1].Y - mobile2.MeanDamage);
                    dmgcompare1.Add(i, dmgcompare1[i - 1].Y + mobile1.MeanDamage);
                }
                myPane3.Title.Text = "Schadensvergleich mit Durchschnittsschaden";
                myPane3.XAxis.Title.Text = "Schläge";
                myPane3.YAxis.Title.Text = "HP/Damage";
                myPane3.XAxis.Scale.Max = (count - 1) * 1.01;
                myPane3.YAxis.Scale.Min = Math.Min(dmgcompare2[count - 1].Y, dmgcompare1[0].Y) * 0.99;
                myPane3.YAxis.Scale.Max = Math.Max(dmgcompare1[count - 1].Y, dmgcompare2[0].Y) * 1.01;

                LineItem myCurve1 = myPane3.AddCurve("", dmgcompare1, Color.Red, SymbolType.None);
                LineItem myCurve2 = myPane3.AddCurve("", dmgcompare2, Color.Blue, SymbolType.None);

                myPane3.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45F);
                myPane3.Fill = new Fill(Color.White, Color.FromArgb(220, 220, 255), 45F);

                GraphPane myPane4 = RESULT.Pane4;
                myPane4.Legend.IsVisible = false;


                PointPairList dmgcompare3 = mobile1.HitList();
                PointPairList dmgcompare4 = mobile2.HitList();

                myPane4.Title.Text = "Schadensvergleich über Zeit";
                string x_title = "Zeit" + Environment.NewLine + "Hitchance:" + Environment.NewLine;
                x_title += String.Format("{0} (Skill {1}) {2:0.0000}% {3} (Skill {4}) {5:0.000}%", mobile1.Name, mobile1.Weaponskill, mobile1.Hitchance * 100, mobile2.Name, mobile2.Weaponskill, mobile2.Hitchance * 100);
                x_title += Environment.NewLine + "Sekunden pro Attacke:" + Environment.NewLine;
                x_title += String.Format("{0}=(Waffenspeed {1}) {2:0.0000} {3}=(Waffenspeed {4}) {5:0.0000}", mobile1.Name, mobile1.Attackspeed, mobile1.SwingSpeed, mobile2.Name, mobile2.Attackspeed, mobile2.SwingSpeed);
                x_title += Environment.NewLine + "Schnitt in 30s:" + Environment.NewLine;
                x_title += String.Format("{0} {1:0.0000} {2} {3:0.0000}", mobile1.Name, (30 / mobile1.SwingSpeed * mobile1.Hitchance * mobile1.MeanDamage), mobile2.Name, (30 / mobile2.SwingSpeed * mobile2.Hitchance * mobile2.MeanDamage));
                myPane4.XAxis.Title.FontSpec.Size = 9;
                myPane4.XAxis.Title.Text = x_title;
                myPane4.YAxis.Title.Text = "Damage";
                myPane4.XAxis.Scale.Max = 60;
                myPane4.YAxis.Scale.Max = Math.Max(dmgcompare3[dmgcompare3.Count - 1].Y, dmgcompare4[dmgcompare4.Count - 1].Y) * 1.01;

                PointPairList dmg3hp = new PointPairList();
                dmg3hp.Add(0, mobile2.Hp);
                dmg3hp.Add(dmgcompare3.InterpolateY(mobile2.Hp), mobile2.Hp);
                dmg3hp.Add(dmgcompare3.InterpolateY(mobile2.Hp), 0);
                LineItem myCurvedmg3hp = new LineItem("", dmg3hp, Color.Red, SymbolType.None);
                myCurvedmg3hp.Line.Style = System.Drawing.Drawing2D.DashStyle.Dash;
                myPane4.CurveList.Insert(0, myCurvedmg3hp);

                PointPairList dmg4hp = new PointPairList();
                dmg4hp.Add(0, mobile1.Hp);
                dmg4hp.Add(dmgcompare4.InterpolateY(mobile1.Hp), mobile1.Hp);
                dmg4hp.Add(dmgcompare4.InterpolateY(mobile1.Hp), 0);
                LineItem myCurvedmg4hp = new LineItem("", dmg4hp, Color.Blue, SymbolType.None);
                myCurvedmg4hp.Line.Style = System.Drawing.Drawing2D.DashStyle.Dash;
                myPane4.CurveList.Insert(0, myCurvedmg4hp);

                LineItem myCurve3 = myPane4.AddCurve("", dmgcompare3, Color.Red, SymbolType.None);
                LineItem myCurve4 = myPane4.AddCurve("", dmgcompare4, Color.Blue, SymbolType.None);

                myPane4.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45F);
                myPane4.Fill = new Fill(Color.White, Color.FromArgb(220, 220, 255), 45F);

                // kompletter Testkampf

                CombatCalcResultTestCombat RESULT1 = new CombatCalcResultTestCombat();
                dmg1 = new PointPairList();
                dmg2 = new PointPairList();

                for (int _switch = 0; _switch < 2; _switch++)
                {
                    Mobile attacker;
                    Mobile defender;
                    PointPairList dmggraph;
                    GraphPane panel;
                    if (_switch == 0)
                    {
                        attacker = mobile1;
                        defender = mobile2;
                        dmggraph = dmg1;
                        panel = RESULT1.Pane1;
                    }
                    else
                    {
                        attacker = mobile2;
                        defender = mobile1;
                        dmggraph = dmg2;
                        panel = RESULT1.Pane2;
                    }
                    int nicht_getroffen = 0;
                    int anatomie = 0;
                    int schildboni = 0;
                    int ausweichen = 0;
                    int voll = 0;
                    int schwer = 0;
                    if (!Combat(attacker, defender, ref dmggraph, ref nicht_getroffen, ref anatomie, ref schildboni,
                        ref ausweichen, ref voll, ref schwer))
                        continue;

                    double maxx = 0;
                    double meanx = 0;
                    int countd = 0;
                    int hp;
                    int maxhp = 300;
                    if (defender.Hp > 0)
                        maxhp = defender.Hp;

                    while (countd < dmggraph.Count)
                    {
                        hp = maxhp;
                        PointPairList dmglist = new PointPairList();
                        double time = 0;
                        count = 0;
                        dmglist.Add(0, 0);
                        while (hp > 0)
                        {
                            if (countd >= dmggraph.Count)
                                break;
                            time += attacker.SwingSpeed;
                            if (Utility.RandomDouble() < attacker.Hitchance)
                            {
                                hp -= (int)dmggraph[countd].Y;
                                dmglist.Add(time, dmglist[count++].Y + dmggraph[countd++].Y);
                            }
                            else
                                dmglist.Add(time, dmglist[count++].Y);
                        }
                        if (countd < dmggraph.Count) //vollständer graph
                        {
                            maxx = Math.Max(maxx, time);
                            meanx += dmglist.InterpolateY(maxhp);
                            LineItem myCurve = panel.AddCurve("", dmglist, attacker == mobile1 ? Color.Red : Color.Blue,
                                          SymbolType.None);
                        }
                    }

                    PointPairList dmg1mean = new PointPairList();
                    dmg1mean.Add(0, 0);
                    dmg1mean.Add(meanx / panel.CurveList.Count, maxhp);
                    dmg1mean.Add(meanx / panel.CurveList.Count, 0);

                    LineItem myCurveMean = new LineItem("", dmg1mean, Color.Green, SymbolType.None);
                    myCurveMean.Line.Style = System.Drawing.Drawing2D.DashStyle.Dash;
                    myCurveMean.Line.Width = 2f;
                    panel.CurveList.Insert(0, myCurveMean);

                    TextObj text = new TextObj(String.Format("{0:0.0000}", meanx / panel.CurveList.Count), meanx / panel.CurveList.Count, 3F);
                    text.Location.AlignH = AlignH.Left;
                    text.Location.AlignV = AlignV.Bottom;
                    text.FontSpec.StringAlignment = StringAlignment.Near;
                    text.FontSpec.Size = 9;
                    panel.GraphObjList.Add(text);

                    panel.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45F);
                    panel.Fill = new Fill(Color.White, Color.FromArgb(220, 220, 255), 45F);
                    panel.Legend.IsVisible = false;
                    panel.Title.Text = attacker.Name + " Beispielkampf - " + attacker.XMLName;
                    panel.XAxis.Title.Text = "Zeit";
                    panel.YAxis.Title.Text = "Damage";
                    panel.XAxis.Scale.Max = maxx * 1.01;
                    panel.YAxis.Scale.Max = maxhp * 1.01;
                }

                RESULT1.Finish();
                RESULT1.Show();

                RESULT.Finish();
                RESULT.Show();
            }
            catch (Exception err)
            {
                MessageBox.Show("Error: " + err.Message);
            }
        }

        private bool Combat(Mobile attacker, Mobile defender, ref PointPairList pointlist, ref int nicht_getroffen,
            ref int anatomie, ref int schildboni, ref int ausweichen, ref int voll, ref int schwer)
        {
            try
            {
                for (int calccount = 0; calccount < Calculationtimes; ++calccount)
                {
                    if (Utility.RandomDouble() >= attacker.Hitchance)
                        ++nicht_getroffen;
                    int damage = attacker.WeaponDice.Roll() * attacker.Weaponhp / attacker.Weaponitemhp;
                    double dmg_multi = (attacker.Taktik + 50 + (attacker.Str * 0.2)) * 0.01;
                    double dmg = dmg_multi * damage;
                    if (defender.Shieldskill > 0)
                    {
                        double parrychance = defender.Shieldskill / 200.0;
                        if (Utility.RandomDouble() < parrychance)
                            dmg -= defender.Shieldar;
                        if (dmg < 0)
                            dmg = 0;
                    }
                    Zone armorzone = defender.ChooseArmor();
                    int armor = 0;
                    bool hasarmor = true;
                    if ((armorzone == null) || (armorzone.Armor == 0))
                        hasarmor = false;
                    if (armorzone != null)
                        armor = armorzone.Armor;
                    else if (defender.Npc)
                        armor = defender.Ar;
                    if (defender.ArMod != 0)
                        armor += defender.ArMod;
                    if (armor != 0)
                    {
                        int blocked = armor;
                        int absorb = blocked / 2;
                        blocked -= absorb;
                        absorb += Utility.Random(blocked + 1);
                        dmg -= absorb;
                        if (dmg >= 2.0)
                            dmg /= 2;
                        if (dmg < 0)
                            dmg = 0;
                    }
                    dmg = (double)Math.Floor(dmg);

                    //Scriptstart
                    double dmgboni = 0;

                    if (attacker.Eledmg > 0)
                        dmgboni += attacker.Eledmg * defender.EleResist;

                    if (!attacker.Npc)
                    {
                        if (CheckSkill(-1, attacker.Anatomie))
                        {
                            dmgboni += (double)Math.Floor(attacker.Anatomie / 6.0);
                            anatomie++;
                        }
                    }

                    if (dmgboni > 0)
                    {
                        if (defender.Shieldskill > 0)
                        {
                            double parrychance = defender.Shieldskill / 200.0;
                            if (Utility.RandomDouble() < parrychance)
                            {
                                dmgboni *= 0.5;
                                schildboni++;
                            }
                        }
                        dmg += dmgboni;
                    }

                    double ausweichchance = defender.Ausweichen / 140.0;
                    if (Utility.RandomDouble() < ausweichchance)
                    {
                        double dmg_div = (defender.Ausweichen + 50 + defender.Dex * 0.2) * 0.0085;
                        if (dmg_div > 1.0)
                            dmg /= dmg_div;
                        ausweichen++;
                    }
                    if (!attacker.Npc)
                    {
                        if (attacker.Weaponcritchance > 0)
                        {
                            if (hasarmor)
                            {
                                if (Utility.Random(200) <= attacker.Weaponcritchance)
                                {
                                    if (Utility.Random(100) < 10)
                                    {
                                        dmg *= 6;
                                        voll++;
                                    }
                                    else
                                    {
                                        dmg *= 2;
                                        schwer++;
                                    }
                                }
                            }
                            else
                            {
                                if (Utility.Random(100) <= attacker.Weaponcritchance)
                                {
                                    if (Utility.Random(100) < 10)
                                    {
                                        dmg *= 6;
                                        voll++;
                                    }
                                    else
                                    {
                                        dmg *= 3;
                                        schwer++;
                                    }
                                }
                            }
                        }
                    }
                    dmg = (double)Math.Floor(dmg);
                    pointlist.Add(calccount, dmg);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
                return false;
            }
            return true;
        }

        private bool CheckSkill(int difficulty, int skillvalue)
        {
            double chance;
            if (difficulty >= 0)
                chance = Math.Min(1, 0.025 * (skillvalue - difficulty) + 0.5);
            else
                chance = Math.Min(1, 0.01 * skillvalue);

            if (chance > 0)
            {
                if ((int)Math.Floor(chance * 1000) >= Utility.Random(1000))
                    return true;
            }
            return false;
        }

        private void OnClickRename(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count == 1)
            {
                listView1.LabelEdit = true;
                listView1.SelectedItems[0].BeginEdit();
            }
        }

        private void AfterLabelEdit(object sender, LabelEditEventArgs e)
        {
            if (e.Item >= 0 && e.Item < mobList.mobileList.Count)
            {
                (mobList.mobileList[e.Item]).XMLName = e.Label;
            }
        }

        private void OnClose(object sender, FormClosingEventArgs e)
        {
            TextWriter w = new StreamWriter(XMLpath);
            TextWriter wset = new StreamWriter(XMLpathSets);
            mobList.mobileList.Sort();
            ArmorSetSerializer.Serialize(wset, setList);
            MobListSerializer.Serialize(w, mobList);
            w.Close();
            wset.Close();
        }

        private void OnClickRemove(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count == 1)
            {
                int index = listView1.SelectedItems[0].Index;
                listView1.SelectedItems[0].Remove();
                mobList.mobileList.RemoveAt(index);
            }
        }

        private void OnClickAdd1(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count == 1)
            {
                Mobile mob = mobList.mobileList[listView1.SelectedItems[0].Index];
                checkBox1NPC.Checked = mob.Npc;
                numericUpDown1HP.Value = mob.Hp;
                numericUpDown1Dex.Value = mob.Dex;
                numericUpDown1AR.Value = mob.Ar;
                numericUpDown1ArMod.Value = mob.ArMod;
                comboBox1ArmorSet.SelectedIndex = mob.ArmorSetIndex;
                numericUpDown1Ausweichen.Value = mob.Ausweichen;
                numericUpDown1SchildAR.Value = mob.Shieldar;
                numericUpDown1SchildSkill.Value = mob.Shieldskill;
                numericUpDown1WaffenSkill.Value = mob.Weaponskill;
                numericUpDown1Str.Value = mob.Str;
                numericUpDown1Taktik.Value = mob.Taktik;
                numericUpDown1Anatomie.Value = mob.Anatomie;
                numericUpDown1EleSchaden.Value = mob.Eledmg;
                numericUpDown1EleResist.Value = mob.EResist;
                numericUpDown1DiceMulti.Value = mob.WeaponDice.Dicemulti;
                numericUpDown1DiceType.Value = mob.WeaponDice.Dicetype;
                numericUpDown1DiceAdd.Value = mob.WeaponDice.Diceadd;
                numericUpDown1WeaponHP.Value = mob.Weaponhp;
                numericUpDown1WeaponItemdescHP.Value = mob.Weaponitemhp;
                numericUpDown1WeaponCritchance.Value = mob.Weaponcritchance;
                numericUpDown1Attackspeed.Value = mob.Attackspeed;
                textBox1Name.Text = mob.ToString();
            }
        }

        private void OnClickAdd2(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count == 1)
            {
                Mobile mob = mobList.mobileList[listView1.SelectedItems[0].Index];
                checkBox2NPC.Checked = mob.Npc;
                numericUpDown2HP.Value = mob.Hp;
                numericUpDown2Dex.Value = mob.Dex;
                numericUpDown2AR.Value = mob.Ar;
                numericUpDown2ArMod.Value = mob.ArMod;
                comboBox2ArmorSet.SelectedIndex = mob.ArmorSetIndex;
                numericUpDown2Ausweichen.Value = mob.Ausweichen;
                numericUpDown2SchildAR.Value = mob.Shieldar;
                numericUpDown2SchildSkill.Value = mob.Shieldskill;
                numericUpDown2WaffenSkill.Value = mob.Weaponskill;
                numericUpDown2Str.Value = mob.Str;
                numericUpDown2Taktik.Value = mob.Taktik;
                numericUpDown2Anatomie.Value = mob.Anatomie;
                numericUpDown2EleSchaden.Value = mob.Eledmg;
                numericUpDown2EleResist.Value = mob.EResist;
                numericUpDown2DiceMulti.Value = mob.WeaponDice.Dicemulti;
                numericUpDown2DiceType.Value = mob.WeaponDice.Dicetype;
                numericUpDown2DiceAdd.Value = mob.WeaponDice.Diceadd;
                numericUpDown2WeaponHP.Value = mob.Weaponhp;
                numericUpDown2WeaponItemdescHP.Value = mob.Weaponitemhp;
                numericUpDown2WeaponCritchance.Value = mob.Weaponcritchance;
                numericUpDown2AttackSpeed.Value = mob.Attackspeed;
                textBox2Name.Text = mob.ToString();
            }
        }

        private void OnClickSaveMob1(object sender, EventArgs e)
        {
            Mobile mob = new Mobile();
            mob.Npc = checkBox1NPC.Checked;
            mob.Hp = (int)numericUpDown1HP.Value;
            mob.Dex = (int)numericUpDown1Dex.Value;
            mob.Ar = (int)numericUpDown1AR.Value;
            mob.ArMod = (int)numericUpDown1ArMod.Value;
            mob.ArmorSetIndex = comboBox1ArmorSet.SelectedIndex;
            mob.Ausweichen = (int)numericUpDown1Ausweichen.Value;
            mob.Shieldar = (int)numericUpDown1SchildAR.Value;
            mob.Shieldskill = (int)numericUpDown1SchildSkill.Value;
            mob.Weaponskill = (int)numericUpDown1WaffenSkill.Value;
            mob.Str = (int)numericUpDown1Str.Value;
            mob.Taktik = (int)numericUpDown1Taktik.Value;
            mob.Anatomie = (int)numericUpDown1Anatomie.Value;
            mob.Eledmg = (int)numericUpDown1EleSchaden.Value;
            mob.EResist = (int)numericUpDown1EleResist.Value;
            mob.WeaponDice.Dicemulti = (int)numericUpDown1DiceMulti.Value;
            mob.WeaponDice.Dicetype = (int)numericUpDown1DiceType.Value;
            mob.WeaponDice.Diceadd = (int)numericUpDown1DiceAdd.Value;
            mob.Weaponhp = (int)numericUpDown1WeaponHP.Value;
            mob.Weaponitemhp = (int)numericUpDown1WeaponItemdescHP.Value;
            mob.Weaponcritchance = (int)numericUpDown1WeaponCritchance.Value;
            mob.Attackspeed = (int)numericUpDown1Attackspeed.Value;
            mob.XMLName = textBox1Name.Text;
            mobList.AddMobile(mob);
            ListViewItem item = new ListViewItem();
            item.Text = mob.ToString();
            listView1.Items.Add(item);
        }

        private void OnClickSaveMob2(object sender, EventArgs e)
        {
            Mobile mob = new Mobile();
            mob.Npc = checkBox2NPC.Checked;
            mob.Hp = (int)numericUpDown2HP.Value;
            mob.Dex = (int)numericUpDown2Dex.Value;
            mob.Ar = (int)numericUpDown2AR.Value;
            mob.ArMod = (int)numericUpDown2ArMod.Value;
            mob.ArmorSetIndex = comboBox2ArmorSet.SelectedIndex;
            mob.Ausweichen = (int)numericUpDown2Ausweichen.Value;
            mob.Shieldar = (int)numericUpDown2SchildAR.Value;
            mob.Shieldskill = (int)numericUpDown2SchildSkill.Value;
            mob.Weaponskill = (int)numericUpDown2WaffenSkill.Value;
            mob.Str = (int)numericUpDown2Str.Value;
            mob.Taktik = (int)numericUpDown2Taktik.Value;
            mob.Anatomie = (int)numericUpDown2Anatomie.Value;
            mob.Eledmg = (int)numericUpDown2EleSchaden.Value;
            mob.EResist = (int)numericUpDown2EleResist.Value;
            mob.WeaponDice.Dicemulti = (int)numericUpDown2DiceMulti.Value;
            mob.WeaponDice.Dicetype = (int)numericUpDown2DiceType.Value;
            mob.WeaponDice.Diceadd = (int)numericUpDown2DiceAdd.Value;
            mob.Weaponhp = (int)numericUpDown2WeaponHP.Value;
            mob.Weaponitemhp = (int)numericUpDown2WeaponItemdescHP.Value;
            mob.Weaponcritchance = (int)numericUpDown2WeaponCritchance.Value;
            mob.Attackspeed = (int)numericUpDown2AttackSpeed.Value;
            mob.XMLName = textBox2Name.Text;
            mobList.AddMobile(mob);
            ListViewItem item = new ListViewItem();
            item.Text = mob.ToString();
            listView1.Items.Add(item);
        }

        private void OnCheckedNPC1(object sender, EventArgs e)
        {
            numericUpDown1Ausweichen.Enabled = !checkBox1NPC.Checked;
            numericUpDown1WeaponCritchance.Enabled = !checkBox1NPC.Checked;
            numericUpDown1AR.Enabled = checkBox1NPC.Checked;
            numericUpDown1Anatomie.Enabled = !checkBox1NPC.Checked;
        }

        private void OnCheckedNPC2(object sender, EventArgs e)
        {
            numericUpDown2Ausweichen.Enabled = !checkBox2NPC.Checked;
            numericUpDown2WeaponCritchance.Enabled = !checkBox2NPC.Checked;
            numericUpDown2AR.Enabled = checkBox2NPC.Checked;
            numericUpDown2Anatomie.Enabled = !checkBox2NPC.Checked;
        }
    }
}

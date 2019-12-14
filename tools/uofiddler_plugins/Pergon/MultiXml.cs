using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using POLConfig;
using FiddlerControls;
using System.Text.RegularExpressions;

namespace Pergon
{
    public partial class MultiXml : Form
    {
        public MultiXml()
        {
            InitializeComponent();
        }

        private void OnClickBrowse(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = false;
            dialog.Title = "Choose itemdesc.cfg file to import";
            dialog.CheckFileExists = true;
            dialog.Filter="itemdesc.cfg file (itemdesc.cfg)|itemdesc.cfg";
            if (dialog.ShowDialog() == DialogResult.OK)
                textBox1.Text = dialog.FileName;
            dialog.Dispose();
        }

        private void OnClickConvert(object sender, EventArgs e)
        {
            string file = textBox1.Text;
            if (String.IsNullOrEmpty(file))
                return;
            if (!File.Exists(file))
                return;
            richTextBox1.Clear();
            POLConfigFile cfg = new POLConfigFile(file, POLConfigFile.FlagOpts.read_structured);
            List<string> tooltips=new List<string>();
            foreach (object o in cfg.Structure)
            {
                if (o.GetType()==typeof(POLConfigLine))
                {
                    POLConfigLine line=(POLConfigLine)o;
                    if (!String.IsNullOrEmpty(line._comments))
                    {

                        string[] split = Regex.Split(line._comments, @"\s+");
                        if (split.Length<5)
                            continue;
                        int multiid, bauplanid;
                        if (Utils.ConvertStringToInt(split[1], out multiid) &&
                            Utils.ConvertStringToInt(split[3], out bauplanid))
                        {
                            string desc = "";
                            for (int i = 4; i < split.Length; ++i)
                            {
                                desc += split[i];
                                if (i < split.Length - 1)
                                    desc += " ";
                            }
                            richTextBox1.AppendText(String.Format("  <Multi name=\"{0}\" id=\"{1}\" type=\"1\" />\r\n",
                                    desc,
                                    multiid));
                            foreach (object elems in cfg.Structure)
                            {
                                if (elems.GetType() == typeof(POLConfigElem))
                                {
                                    POLConfigElem elem = (POLConfigElem)elems;
                                    if (elem.Prefix == "Item")
                                    {
                                        int itemid;
                                        if (Utils.ConvertStringToInt(elem.ElemName.Trim(), out itemid))
                                        {
                                            if (itemid==bauplanid)
                                            {
                                                tooltips.Add(String.Format("  <ToolTip id=\"{0}\" text=\"{1}\" />\r\n",multiid,
                                                    String.Format("HausTyp: {0}\r\n"+
                                                                  "Barren: {1}\r\n"+
                                                                  "Bretter: {2}\r\n"+
                                                                  "Granit: {3}\r\n"+
                                                                  "Lehm: {4}\r\n"+
                                                                  "Marmor: {5}\r\n"+
                                                                  "Sandstein: {6}\r\n"+
                                                                  "Staemme: {7}\r\n"+
                                                                  "Stoff: {8}\r\n"+
                                                                  "Stroh: {9}",
                                                    elem.GetConfigString("HouseType").Trim(),
                                                    elem.GetConfigInt("Barren"),
                                                    elem.GetConfigInt("Bretter"),
                                                    elem.GetConfigInt("Granit"),
                                                    elem.GetConfigInt("Lehm"),
                                                    elem.GetConfigInt("Marmor"),
                                                    elem.GetConfigInt("Sandstein"),
                                                    elem.GetConfigInt("Staemme"),
                                                    elem.GetConfigInt("Stoff"),
                                                    elem.GetConfigInt("Stroh"))));
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (o.GetType()==typeof(POLConfigElem))
                    break;
            }
            foreach (string tip in tooltips)
            {
                richTextBox1.AppendText(tip);
            }
        }
    }
}

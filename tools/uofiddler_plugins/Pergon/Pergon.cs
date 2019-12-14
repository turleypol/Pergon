using System;
using System.Windows.Forms;
using PluginInterface;

namespace FiddlerPlugin
{
    public class PergonPlugin : IPlugin
    {
        public PergonPlugin()
        {
        }

        string myName = "PergonPlugin";
        string myDescription = "Pergon Plugin";
        string myAuthor = "Turley";
        string myVersion = "2.1.3";
        IPluginHost myHost = null;

        /// <summary>
        /// Name of the plugin
        /// </summary>
        public override string Name { get { return myName; } }
        /// <summary>
        /// Description of the Plugin's purpose
        /// </summary>
        public override string Description { get { return myDescription; } }
        /// <summary>
        /// Author of the plugin
        /// </summary>
        public override string Author { get { return myAuthor; } }
        /// <summary>
        /// Version of the plugin
        /// </summary>
        public override string Version { get { return myVersion; } }
        /// <summary>
        /// Host of the plugin.
        /// </summary>
        public override IPluginHost Host { get { return myHost; } set { myHost = value; } }

        public override void Initialize()
        {
        }

        public override void Dispose()
        {
        }

        public override void ModifyTabPages(TabControl tabcontrol)
        {
        }

        public override void ModifyPluginToolStrip(ToolStripDropDownButton toolstrip)
        {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = "Pergon - MultiCalc";
            item.Click += new EventHandler(multicalc_click);
            toolstrip.DropDownItems.Add(item);
            item = new ToolStripMenuItem();
            item.Text = "Pergon - CombatCalc";
            item.Click += new EventHandler(combatcalc_click);
            toolstrip.DropDownItems.Add(item);
            item = new ToolStripMenuItem();
            item.Text = "Pergon - CreateMultiXml";
            item.Click += new EventHandler(multixml_click);
            toolstrip.DropDownItems.Add(item);
        }

        Pergon.MultiCalc multicalc;
        public void multicalc_click(object sender, EventArgs e)
        {
            if ((multicalc == null) || (multicalc.IsDisposed))
            {
                multicalc = new Pergon.MultiCalc();
                multicalc.Show();
            }
        }
        Pergon.CombatCalc combatcalc;
        public void combatcalc_click(object sender, EventArgs e)
        {
            if ((combatcalc == null) || (combatcalc.IsDisposed))
            {
                combatcalc = new Pergon.CombatCalc();
                combatcalc.Show();
            }
        }

        Pergon.MultiXml multixml;
        public void multixml_click(object sender, EventArgs e)
        {
            if ((multixml == null) || (multixml.IsDisposed))
            {
                multixml = new Pergon.MultiXml();
                multixml.Show();
            }
        }
    }
}

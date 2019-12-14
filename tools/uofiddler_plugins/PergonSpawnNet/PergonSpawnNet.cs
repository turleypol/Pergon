﻿/***************************************************************************
 *
 * $Author: Turley
 * 
 * "THE BEER-WARE LICENSE"
 * As long as you retain this notice you can do whatever you want with 
 * this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me a beer in return.
 *
 ***************************************************************************/

using System;
using System.Windows.Forms;
using PluginInterface;

namespace FiddlerPlugin
{
    public class PergonSpawnNetPlugin : IPlugin
    {
        public PergonSpawnNetPlugin()
        {
        }

        string myName = "PergonSpawnNetPlugin";
        string myDescription = "Show Spawnnet";
        string myAuthor = "Turley";
        string myVersion = "0.0.1";
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

        public override void ModifyTabPages(TabControl tabcontrol) { }

        public override void ModifyPluginToolStrip(ToolStripDropDownButton toolstrip)
        {
            ToolStripMenuItem item = new ToolStripMenuItem();
            item.Text = "Pergon SpawnNet";
            item.Click += new EventHandler(toolstrip_click);
            toolstrip.DropDownItems.Add(item);
        }

        private void toolstrip_click(object sender, EventArgs e)
        {
            new PergonSpawnNet.SpawnViewer().Show();
        }
    }
}

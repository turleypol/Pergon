namespace Pergon
{
    partial class MultiCalc
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MultiCalc));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Multi_treeView = new System.Windows.Forms.TreeView();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
            this.reloadXMLDefinitionToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAllImagespngToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAllHeightImagesOfSelectedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPagePic = new System.Windows.Forms.TabPage();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.tabPageContents = new System.Windows.Forms.TabPage();
            this.richTextBoxContents = new System.Windows.Forms.RichTextBox();
            this.richTextBoxCalc = new System.Windows.Forms.RichTextBox();
            this.calculateAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPagePic.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.tabPageContents.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.Multi_treeView);
            this.splitContainer1.Panel1.Controls.Add(this.toolStrip1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Size = new System.Drawing.Size(851, 431);
            this.splitContainer1.SplitterDistance = 283;
            this.splitContainer1.TabIndex = 0;
            // 
            // Multi_treeView
            // 
            this.Multi_treeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Multi_treeView.HideSelection = false;
            this.Multi_treeView.Location = new System.Drawing.Point(0, 25);
            this.Multi_treeView.Name = "Multi_treeView";
            this.Multi_treeView.Size = new System.Drawing.Size(283, 406);
            this.Multi_treeView.TabIndex = 0;
            this.Multi_treeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.AfterSelect_MultiTreeView);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripDropDownButton1});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(283, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripDropDownButton1
            // 
            this.toolStripDropDownButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripDropDownButton1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.reloadXMLDefinitionToolStripMenuItem1,
            this.saveAllImagespngToolStripMenuItem,
            this.saveAllHeightImagesOfSelectedToolStripMenuItem,
            this.calculateAllToolStripMenuItem});
            this.toolStripDropDownButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripDropDownButton1.Image")));
            this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
            this.toolStripDropDownButton1.Size = new System.Drawing.Size(62, 22);
            this.toolStripDropDownButton1.Text = "Options";
            // 
            // reloadXMLDefinitionToolStripMenuItem1
            // 
            this.reloadXMLDefinitionToolStripMenuItem1.Name = "reloadXMLDefinitionToolStripMenuItem1";
            this.reloadXMLDefinitionToolStripMenuItem1.Size = new System.Drawing.Size(256, 22);
            this.reloadXMLDefinitionToolStripMenuItem1.Text = "Reload XML Definition";
            this.reloadXMLDefinitionToolStripMenuItem1.Click += new System.EventHandler(this.OnClickReloadXML);
            // 
            // saveAllImagespngToolStripMenuItem
            // 
            this.saveAllImagespngToolStripMenuItem.Name = "saveAllImagespngToolStripMenuItem";
            this.saveAllImagespngToolStripMenuItem.Size = new System.Drawing.Size(256, 22);
            this.saveAllImagespngToolStripMenuItem.Text = "Save All Images (png)";
            this.saveAllImagespngToolStripMenuItem.Click += new System.EventHandler(this.OnClickSaveAllImages);
            // 
            // saveAllHeightImagesOfSelectedToolStripMenuItem
            // 
            this.saveAllHeightImagesOfSelectedToolStripMenuItem.Name = "saveAllHeightImagesOfSelectedToolStripMenuItem";
            this.saveAllHeightImagesOfSelectedToolStripMenuItem.Size = new System.Drawing.Size(256, 22);
            this.saveAllHeightImagesOfSelectedToolStripMenuItem.Text = "Save All Height Images of Selected";
            this.saveAllHeightImagesOfSelectedToolStripMenuItem.Click += new System.EventHandler(this.OnClickSaveAllHeight);
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.tabControl1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.richTextBoxCalc);
            this.splitContainer2.Size = new System.Drawing.Size(564, 431);
            this.splitContainer2.SplitterDistance = 188;
            this.splitContainer2.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPagePic);
            this.tabControl1.Controls.Add(this.tabPageContents);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(564, 188);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPagePic
            // 
            this.tabPagePic.Controls.Add(this.pictureBox);
            this.tabPagePic.Location = new System.Drawing.Point(4, 22);
            this.tabPagePic.Name = "tabPagePic";
            this.tabPagePic.Padding = new System.Windows.Forms.Padding(3);
            this.tabPagePic.Size = new System.Drawing.Size(556, 162);
            this.tabPagePic.TabIndex = 0;
            this.tabPagePic.Text = "Pic";
            this.tabPagePic.UseVisualStyleBackColor = true;
            // 
            // pictureBox
            // 
            this.pictureBox.BackColor = System.Drawing.Color.White;
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox.Location = new System.Drawing.Point(3, 3);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(550, 156);
            this.pictureBox.TabIndex = 0;
            this.pictureBox.TabStop = false;
            this.pictureBox.Paint += new System.Windows.Forms.PaintEventHandler(this.onPaintMultiPicbox);
            // 
            // tabPageContents
            // 
            this.tabPageContents.Controls.Add(this.richTextBoxContents);
            this.tabPageContents.Location = new System.Drawing.Point(4, 22);
            this.tabPageContents.Name = "tabPageContents";
            this.tabPageContents.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageContents.Size = new System.Drawing.Size(556, 162);
            this.tabPageContents.TabIndex = 1;
            this.tabPageContents.Text = "Contents";
            this.tabPageContents.UseVisualStyleBackColor = true;
            // 
            // richTextBoxContents
            // 
            this.richTextBoxContents.DetectUrls = false;
            this.richTextBoxContents.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBoxContents.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBoxContents.Location = new System.Drawing.Point(3, 3);
            this.richTextBoxContents.Name = "richTextBoxContents";
            this.richTextBoxContents.ReadOnly = true;
            this.richTextBoxContents.Size = new System.Drawing.Size(550, 156);
            this.richTextBoxContents.TabIndex = 0;
            this.richTextBoxContents.Text = "";
            // 
            // richTextBoxCalc
            // 
            this.richTextBoxCalc.DetectUrls = false;
            this.richTextBoxCalc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBoxCalc.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBoxCalc.Location = new System.Drawing.Point(0, 0);
            this.richTextBoxCalc.Name = "richTextBoxCalc";
            this.richTextBoxCalc.ReadOnly = true;
            this.richTextBoxCalc.Size = new System.Drawing.Size(564, 239);
            this.richTextBoxCalc.TabIndex = 0;
            this.richTextBoxCalc.Text = "";
            // 
            // calculateAllToolStripMenuItem
            // 
            this.calculateAllToolStripMenuItem.Name = "calculateAllToolStripMenuItem";
            this.calculateAllToolStripMenuItem.Size = new System.Drawing.Size(256, 22);
            this.calculateAllToolStripMenuItem.Text = "Calculate All...";
            this.calculateAllToolStripMenuItem.Click += new System.EventHandler(this.OnClickCalculateAll);
            // 
            // MultiCalc
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(851, 431);
            this.Controls.Add(this.splitContainer1);
            this.Name = "MultiCalc";
            this.Text = "Pergon - MultiCalc";
            this.Load += new System.EventHandler(this.OnLoad);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPagePic.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.tabPageContents.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView Multi_treeView;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPagePic;
        private System.Windows.Forms.TabPage tabPageContents;
        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.RichTextBox richTextBoxContents;
        private System.Windows.Forms.RichTextBox richTextBoxCalc;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton1;
        private System.Windows.Forms.ToolStripMenuItem reloadXMLDefinitionToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem saveAllImagespngToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAllHeightImagesOfSelectedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem calculateAllToolStripMenuItem;
    }
}


namespace PergonSpawnNet
{
    partial class SpawnViewer
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SpawnViewer));
            this.ToolStrip = new System.Windows.Forms.ToolStrip();
            this.CoordsLabel = new System.Windows.Forms.ToolStripLabel();
            this.ZoomLabel = new System.Windows.Forms.ToolStripLabel();
            this.ClientLocLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.SyncButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
            this.showRunesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showRegionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showRangesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sortByNameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripDropDownButton2 = new System.Windows.Forms.ToolStripDropDownButton();
            this.showClientLocToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.gotoClientLocToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.zoomToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.zoomToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.gotoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.TextBoxGoto = new System.Windows.Forms.ToolStripTextBox();
            this.sendClientToPosToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.feluccaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.trammelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ilshenarToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.malasToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tokunoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.terMurToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.extractMapToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.asBmpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.asTiffToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.asJpgToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.asPngToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.vScrollBar = new System.Windows.Forms.VScrollBar();
            this.hScrollBar = new System.Windows.Forms.HScrollBar();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.listBox = new System.Windows.Forms.ListBox();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.collapsibleSplitter2 = new FiddlerControls.CollapsibleSplitter();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.ToolStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // ToolStrip
            // 
            this.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.ToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CoordsLabel,
            this.ZoomLabel,
            this.ClientLocLabel,
            this.SyncButton,
            this.toolStripDropDownButton1,
            this.toolStripDropDownButton2});
            this.ToolStrip.Location = new System.Drawing.Point(208, 0);
            this.ToolStrip.Name = "ToolStrip";
            this.ToolStrip.Size = new System.Drawing.Size(625, 25);
            this.ToolStrip.TabIndex = 0;
            this.ToolStrip.Text = "toolStrip1";
            // 
            // CoordsLabel
            // 
            this.CoordsLabel.AutoSize = false;
            this.CoordsLabel.Name = "CoordsLabel";
            this.CoordsLabel.Size = new System.Drawing.Size(120, 17);
            this.CoordsLabel.Text = "Coords: 0,0";
            this.CoordsLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // ZoomLabel
            // 
            this.ZoomLabel.AutoSize = false;
            this.ZoomLabel.Name = "ZoomLabel";
            this.ZoomLabel.Size = new System.Drawing.Size(100, 17);
            this.ZoomLabel.Text = "Zoom: ";
            this.ZoomLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // ClientLocLabel
            // 
            this.ClientLocLabel.AutoSize = false;
            this.ClientLocLabel.Name = "ClientLocLabel";
            this.ClientLocLabel.Size = new System.Drawing.Size(200, 17);
            this.ClientLocLabel.Text = "ClientLoc: 0,0";
            this.ClientLocLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // SyncButton
            // 
            this.SyncButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.SyncButton.Image = ((System.Drawing.Image)(resources.GetObject("SyncButton.Image")));
            this.SyncButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.SyncButton.Name = "SyncButton";
            this.SyncButton.Size = new System.Drawing.Size(34, 22);
            this.SyncButton.Text = "Sync";
            this.SyncButton.Click += new System.EventHandler(this.OnClickSyncButton);
            // 
            // toolStripDropDownButton1
            // 
            this.toolStripDropDownButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripDropDownButton1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showRunesToolStripMenuItem,
            this.showRegionsToolStripMenuItem,
            this.showRangesToolStripMenuItem,
            this.sortByNameToolStripMenuItem});
            this.toolStripDropDownButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripDropDownButton1.Image")));
            this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
            this.toolStripDropDownButton1.Size = new System.Drawing.Size(42, 22);
            this.toolStripDropDownButton1.Text = "View";
            // 
            // showRunesToolStripMenuItem
            // 
            this.showRunesToolStripMenuItem.Checked = true;
            this.showRunesToolStripMenuItem.CheckOnClick = true;
            this.showRunesToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.showRunesToolStripMenuItem.Name = "showRunesToolStripMenuItem";
            this.showRunesToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.showRunesToolStripMenuItem.Text = "Show Runes";
            this.showRunesToolStripMenuItem.CheckedChanged += new System.EventHandler(this.OnCheckedChange);
            // 
            // showRegionsToolStripMenuItem
            // 
            this.showRegionsToolStripMenuItem.Checked = true;
            this.showRegionsToolStripMenuItem.CheckOnClick = true;
            this.showRegionsToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.showRegionsToolStripMenuItem.Name = "showRegionsToolStripMenuItem";
            this.showRegionsToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.showRegionsToolStripMenuItem.Text = "Show Regions";
            this.showRegionsToolStripMenuItem.CheckedChanged += new System.EventHandler(this.OnCheckedChangeRegions);
            // 
            // showRangesToolStripMenuItem
            // 
            this.showRangesToolStripMenuItem.CheckOnClick = true;
            this.showRangesToolStripMenuItem.Name = "showRangesToolStripMenuItem";
            this.showRangesToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.showRangesToolStripMenuItem.Text = "Show Ranges";
            this.showRangesToolStripMenuItem.Click += new System.EventHandler(this.OnClickShowRanges);
            // 
            // sortByNameToolStripMenuItem
            // 
            this.sortByNameToolStripMenuItem.CheckOnClick = true;
            this.sortByNameToolStripMenuItem.Name = "sortByNameToolStripMenuItem";
            this.sortByNameToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.sortByNameToolStripMenuItem.Text = "Sort by Name";
            this.sortByNameToolStripMenuItem.CheckedChanged += new System.EventHandler(this.OnSortingChanged);
            // 
            // toolStripDropDownButton2
            // 
            this.toolStripDropDownButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripDropDownButton2.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showClientLocToolStripMenuItem1,
            this.toolStripSeparator5,
            this.gotoClientLocToolStripMenuItem1});
            this.toolStripDropDownButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton2.Name = "toolStripDropDownButton2";
            this.toolStripDropDownButton2.Size = new System.Drawing.Size(89, 22);
            this.toolStripDropDownButton2.Text = "Client Interact";
            // 
            // showClientLocToolStripMenuItem1
            // 
            this.showClientLocToolStripMenuItem1.CheckOnClick = true;
            this.showClientLocToolStripMenuItem1.Name = "showClientLocToolStripMenuItem1";
            this.showClientLocToolStripMenuItem1.Size = new System.Drawing.Size(160, 22);
            this.showClientLocToolStripMenuItem1.Text = "Show Client Loc";
            this.showClientLocToolStripMenuItem1.CheckStateChanged += new System.EventHandler(this.OnClientLocCheck);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(157, 6);
            // 
            // gotoClientLocToolStripMenuItem1
            // 
            this.gotoClientLocToolStripMenuItem1.Name = "gotoClientLocToolStripMenuItem1";
            this.gotoClientLocToolStripMenuItem1.Size = new System.Drawing.Size(160, 22);
            this.gotoClientLocToolStripMenuItem1.Text = "Goto Client Loc";
            this.gotoClientLocToolStripMenuItem1.Click += new System.EventHandler(this.OnClickGotoClient);
            // 
            // pictureBox
            // 
            this.pictureBox.ContextMenuStrip = this.contextMenuStrip1;
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox.Location = new System.Drawing.Point(208, 25);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(625, 462);
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            this.pictureBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.OnMouseMoveMap);
            this.pictureBox.Resize += new System.EventHandler(this.OnResizeMap);
            this.pictureBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnMouseDownMap);
            this.pictureBox.Paint += new System.Windows.Forms.PaintEventHandler(this.OnPaint);
            this.pictureBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnMouseUpMap);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.zoomToolStripMenuItem,
            this.zoomToolStripMenuItem1,
            this.gotoToolStripMenuItem,
            this.sendClientToPosToolStripMenuItem,
            this.toolStripSeparator2,
            this.feluccaToolStripMenuItem,
            this.trammelToolStripMenuItem,
            this.ilshenarToolStripMenuItem,
            this.malasToolStripMenuItem,
            this.tokunoToolStripMenuItem,
            this.terMurToolStripMenuItem,
            this.toolStripSeparator1,
            this.extractMapToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(175, 258);
            this.contextMenuStrip1.Closed += new System.Windows.Forms.ToolStripDropDownClosedEventHandler(this.OnContextClosed);
            this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.OnOpenContext);
            // 
            // zoomToolStripMenuItem
            // 
            this.zoomToolStripMenuItem.Name = "zoomToolStripMenuItem";
            this.zoomToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.zoomToolStripMenuItem.Text = "+Zoom";
            this.zoomToolStripMenuItem.Click += new System.EventHandler(this.OnZoomPlus);
            // 
            // zoomToolStripMenuItem1
            // 
            this.zoomToolStripMenuItem1.Name = "zoomToolStripMenuItem1";
            this.zoomToolStripMenuItem1.Size = new System.Drawing.Size(174, 22);
            this.zoomToolStripMenuItem1.Text = "-Zoom";
            this.zoomToolStripMenuItem1.Click += new System.EventHandler(this.OnZoomMinus);
            // 
            // gotoToolStripMenuItem
            // 
            this.gotoToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.TextBoxGoto});
            this.gotoToolStripMenuItem.Name = "gotoToolStripMenuItem";
            this.gotoToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.gotoToolStripMenuItem.Text = "Goto...";
            this.gotoToolStripMenuItem.DropDownClosed += new System.EventHandler(this.OnDropDownClosed);
            // 
            // TextBoxGoto
            // 
            this.TextBoxGoto.Name = "TextBoxGoto";
            this.TextBoxGoto.Size = new System.Drawing.Size(100, 21);
            this.TextBoxGoto.KeyDown += new System.Windows.Forms.KeyEventHandler(this.onKeyDownGoto);
            // 
            // sendClientToPosToolStripMenuItem
            // 
            this.sendClientToPosToolStripMenuItem.Name = "sendClientToPosToolStripMenuItem";
            this.sendClientToPosToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.sendClientToPosToolStripMenuItem.Text = "Send Client To Pos";
            this.sendClientToPosToolStripMenuItem.Click += new System.EventHandler(this.onClickSendClientPos);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(171, 6);
            // 
            // feluccaToolStripMenuItem
            // 
            this.feluccaToolStripMenuItem.Name = "feluccaToolStripMenuItem";
            this.feluccaToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.feluccaToolStripMenuItem.Text = "Felucca";
            this.feluccaToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapFelucca);
            // 
            // trammelToolStripMenuItem
            // 
            this.trammelToolStripMenuItem.Name = "trammelToolStripMenuItem";
            this.trammelToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.trammelToolStripMenuItem.Text = "Trammel";
            this.trammelToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapTrammel);
            // 
            // ilshenarToolStripMenuItem
            // 
            this.ilshenarToolStripMenuItem.Name = "ilshenarToolStripMenuItem";
            this.ilshenarToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.ilshenarToolStripMenuItem.Text = "Ilshenar";
            this.ilshenarToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapIlshenar);
            // 
            // malasToolStripMenuItem
            // 
            this.malasToolStripMenuItem.Name = "malasToolStripMenuItem";
            this.malasToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.malasToolStripMenuItem.Text = "Malas";
            this.malasToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapMalas);
            // 
            // tokunoToolStripMenuItem
            // 
            this.tokunoToolStripMenuItem.Name = "tokunoToolStripMenuItem";
            this.tokunoToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.tokunoToolStripMenuItem.Text = "Tokuno";
            this.tokunoToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapTokuno);
            // 
            // terMurToolStripMenuItem
            // 
            this.terMurToolStripMenuItem.Name = "terMurToolStripMenuItem";
            this.terMurToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.terMurToolStripMenuItem.Text = "TerMur";
            this.terMurToolStripMenuItem.Click += new System.EventHandler(this.OnChangeMapTerMur);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(171, 6);
            // 
            // extractMapToolStripMenuItem
            // 
            this.extractMapToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.asBmpToolStripMenuItem,
            this.asTiffToolStripMenuItem,
            this.asJpgToolStripMenuItem,
            this.asPngToolStripMenuItem});
            this.extractMapToolStripMenuItem.Name = "extractMapToolStripMenuItem";
            this.extractMapToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.extractMapToolStripMenuItem.Text = "Extract Map..";
            // 
            // asBmpToolStripMenuItem
            // 
            this.asBmpToolStripMenuItem.Name = "asBmpToolStripMenuItem";
            this.asBmpToolStripMenuItem.Size = new System.Drawing.Size(120, 22);
            this.asBmpToolStripMenuItem.Text = "As Bmp";
            this.asBmpToolStripMenuItem.Click += new System.EventHandler(this.OnClickExtractBmp);
            // 
            // asTiffToolStripMenuItem
            // 
            this.asTiffToolStripMenuItem.Name = "asTiffToolStripMenuItem";
            this.asTiffToolStripMenuItem.Size = new System.Drawing.Size(120, 22);
            this.asTiffToolStripMenuItem.Text = "As Tiff";
            this.asTiffToolStripMenuItem.Click += new System.EventHandler(this.OnClickExtractTiff);
            // 
            // asJpgToolStripMenuItem
            // 
            this.asJpgToolStripMenuItem.Name = "asJpgToolStripMenuItem";
            this.asJpgToolStripMenuItem.Size = new System.Drawing.Size(120, 22);
            this.asJpgToolStripMenuItem.Text = "As Jpg";
            this.asJpgToolStripMenuItem.Click += new System.EventHandler(this.OnClickExtractJpg);
            // 
            // asPngToolStripMenuItem
            // 
            this.asPngToolStripMenuItem.Name = "asPngToolStripMenuItem";
            this.asPngToolStripMenuItem.Size = new System.Drawing.Size(120, 22);
            this.asPngToolStripMenuItem.Text = "As Png";
            this.asPngToolStripMenuItem.Click += new System.EventHandler(this.OnClickExtractPng);
            // 
            // vScrollBar
            // 
            this.vScrollBar.Dock = System.Windows.Forms.DockStyle.Right;
            this.vScrollBar.Location = new System.Drawing.Point(816, 25);
            this.vScrollBar.Name = "vScrollBar";
            this.vScrollBar.Size = new System.Drawing.Size(17, 462);
            this.vScrollBar.TabIndex = 2;
            this.vScrollBar.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleScroll);
            // 
            // hScrollBar
            // 
            this.hScrollBar.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.hScrollBar.Location = new System.Drawing.Point(208, 470);
            this.hScrollBar.Name = "hScrollBar";
            this.hScrollBar.Size = new System.Drawing.Size(608, 17);
            this.hScrollBar.TabIndex = 3;
            this.hScrollBar.Scroll += new System.Windows.Forms.ScrollEventHandler(this.HandleScroll);
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Left;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.listBox);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer2.Size = new System.Drawing.Size(200, 487);
            this.splitContainer2.SplitterDistance = 250;
            this.splitContainer2.TabIndex = 10;
            // 
            // listBox
            // 
            this.listBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listBox.FormattingEnabled = true;
            this.listBox.Location = new System.Drawing.Point(0, 0);
            this.listBox.Name = "listBox";
            this.listBox.Size = new System.Drawing.Size(200, 238);
            this.listBox.TabIndex = 0;
            this.listBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.OnMouseDoubleClickListBox);
            this.listBox.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.ListBoxDrawItem);
            this.listBox.SelectedIndexChanged += new System.EventHandler(this.OnSelectedChange);
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.propertyGrid1.Size = new System.Drawing.Size(200, 233);
            this.propertyGrid1.TabIndex = 0;
            // 
            // collapsibleSplitter2
            // 
            this.collapsibleSplitter2.AnimationDelay = 20;
            this.collapsibleSplitter2.AnimationStep = 20;
            this.collapsibleSplitter2.BorderStyle3D = System.Windows.Forms.Border3DStyle.Flat;
            this.collapsibleSplitter2.ControlToHide = this.splitContainer2;
            this.collapsibleSplitter2.ExpandParentForm = false;
            this.collapsibleSplitter2.Location = new System.Drawing.Point(200, 0);
            this.collapsibleSplitter2.Name = "collapsibleSplitter2";
            this.collapsibleSplitter2.TabIndex = 9;
            this.collapsibleSplitter2.TabStop = false;
            this.collapsibleSplitter2.UseAnimations = true;
            this.collapsibleSplitter2.VisualStyle = FiddlerControls.VisualStyles.DoubleDots;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 2000;
            this.timer1.Tick += new System.EventHandler(this.SyncClientTimer);
            // 
            // SpawnViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(833, 487);
            this.Controls.Add(this.hScrollBar);
            this.Controls.Add(this.vScrollBar);
            this.Controls.Add(this.pictureBox);
            this.Controls.Add(this.ToolStrip);
            this.Controls.Add(this.collapsibleSplitter2);
            this.Controls.Add(this.splitContainer2);
            this.Name = "SpawnViewer";
            this.Text = "SpawnViewer";
            this.Load += new System.EventHandler(this.OnLoad);
            this.ToolStrip.ResumeLayout(false);
            this.ToolStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.ToolStrip ToolStrip;
        private System.Windows.Forms.VScrollBar vScrollBar;
        private System.Windows.Forms.HScrollBar hScrollBar;
        private System.Windows.Forms.ToolStripLabel CoordsLabel;
        private System.Windows.Forms.ToolStripLabel ZoomLabel;
        private FiddlerControls.CollapsibleSplitter collapsibleSplitter2;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ListBox listBox;
        private System.Windows.Forms.ToolStripButton SyncButton;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem zoomToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem zoomToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem gotoToolStripMenuItem;
        private System.Windows.Forms.ToolStripTextBox TextBoxGoto;
        private System.Windows.Forms.ToolStripMenuItem sendClientToPosToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem feluccaToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem trammelToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ilshenarToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem malasToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tokunoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem terMurToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem extractMapToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem asBmpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem asTiffToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem asJpgToolStripMenuItem;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton1;
        private System.Windows.Forms.ToolStripMenuItem showRunesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showRegionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sortByNameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showRangesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem asPngToolStripMenuItem;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton2;
        private System.Windows.Forms.ToolStripMenuItem showClientLocToolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem gotoClientLocToolStripMenuItem1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.ToolStripStatusLabel ClientLocLabel;
    }
}


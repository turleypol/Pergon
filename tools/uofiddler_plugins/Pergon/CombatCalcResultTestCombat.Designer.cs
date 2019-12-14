namespace Pergon
{
    partial class CombatCalcResultTestCombat
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.panel1 = new ZedGraph.ZedGraphControl();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.ScrollGrace = 0;
            this.panel1.ScrollMaxX = 0;
            this.panel1.ScrollMaxY = 0;
            this.panel1.ScrollMaxY2 = 0;
            this.panel1.ScrollMinX = 0;
            this.panel1.ScrollMinY = 0;
            this.panel1.ScrollMinY2 = 0;
            this.panel1.Size = new System.Drawing.Size(837, 407);
            this.panel1.TabIndex = 0;
            // 
            // CombatCalcResultTestCombat
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(837, 407);
            this.Controls.Add(this.panel1);
            this.Name = "CombatCalcResultTestCombat";
            this.Text = "CombatCalcResult TestCombat";
            this.ResumeLayout(false);

        }

        #endregion

        private ZedGraph.ZedGraphControl panel1;
    }
}
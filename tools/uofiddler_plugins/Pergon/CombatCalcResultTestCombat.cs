using System.Drawing;
using System.Windows.Forms;
using ZedGraph;

namespace Pergon
{
    public partial class CombatCalcResultTestCombat : Form
    {
        private GraphPane pane1;
        private GraphPane pane2;
        public GraphPane Pane1 { get { return pane1; } }
        public GraphPane Pane2 { get { return pane2; } }
        public CombatCalcResultTestCombat()
        {
            InitializeComponent();
            this.Icon = FiddlerControls.Options.GetFiddlerIcon();
            MasterPane master = panel1.MasterPane;
            master.PaneList.Clear();
            master.Title.IsVisible = false;
            master.Margin.All = 1;
            master.InnerPaneGap = 1;

            pane1 = new GraphPane();
            pane2 = new GraphPane();
        }

        public void Finish()
        {
            MasterPane master = panel1.MasterPane;
            master.Add(pane1);
            master.Add(pane2);

            panel1.AxisChange();

            using (Graphics g = this.CreateGraphics())
            {
                master.SetLayout(g, PaneLayout.SquareColPreferred);
            }
        }
    }
}

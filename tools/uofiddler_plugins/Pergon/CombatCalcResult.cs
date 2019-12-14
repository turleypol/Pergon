using System.Drawing;
using System.Windows.Forms;
using ZedGraph;

namespace Pergon
{
    public partial class CombatCalcResult : Form
    {
        private GraphPane pane1;
        private GraphPane pane2;
        private GraphPane pane3;
        private GraphPane pane4;
        public GraphPane Pane1 { get { return pane1; } }
        public GraphPane Pane2 { get { return pane2; } }
        public GraphPane Pane3 { get { return pane3; } }
        public GraphPane Pane4 { get { return pane4; } }
        public CombatCalcResult()
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
            pane3 = new GraphPane();
            pane4 = new GraphPane();
        }

        public void Finish()
        {
            MasterPane master = panel1.MasterPane;
            master.Add(pane1);
            master.Add(pane2);
            master.Add(pane3);
            master.Add(pane4);

            panel1.AxisChange();

            using (Graphics g = this.CreateGraphics())
            {
                master.SetLayout(g, PaneLayout.SquareColPreferred);
            }
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simple_live_windows_forms
{
    public partial class FormSecondScreen : Form
    {

        public FormWindow formWindow { get; set; }

        public FormSecondScreen()
        {
            InitializeComponent();

        }

        private void FormSecondScreen_Load(object sender, EventArgs e)
        {
            BackColor = Color.Black;
            FormBorderStyle = FormBorderStyle.None;
            Bounds = Screen.AllScreens[1].Bounds;
            TopMost = true;
        }


        public void pictureBox1_Paint(object sender, PaintEventArgs e)
        {


            int screen_x_center = this.Bounds.Width / 2;
            int screen_y_center = this.Bounds.Height / 2;


            int r = Decimal.ToInt32(formWindow.numericUpDown_R.Value);
            int x = Decimal.ToInt32(formWindow.numericUpDown_dotX.Value);
            int y = Decimal.ToInt32(formWindow.numericUpDown_dotY.Value);



            if (formWindow.checkBox_showDot.Checked)
            { 

                Rectangle rect = new Rectangle(screen_x_center - r + y, screen_y_center - r + x, r, r);
                e.Graphics.FillEllipse(Brushes.White, rect);
            }

            



        }
    }
}

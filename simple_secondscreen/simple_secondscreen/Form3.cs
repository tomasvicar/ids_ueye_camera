using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simple_secondscreen
{
    public partial class Form3 : Form
    {
        Image image1;
        public Form3()
        {
            InitializeComponent();
        }

        private void Form3_Load(object sender, EventArgs e)
        {
            BackColor = Color.Black;
            FormBorderStyle = FormBorderStyle.None;
            Bounds = Screen.AllScreens[1].Bounds;
            TopMost = true;


            pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);


            //image1 = Image.FromFile(@"C:\Users\vicar\Desktop\VO_soft_2\flickering_test\ids_ueye_camera\simple_secondscreen\example_img.png");

            //pictureBox1.Image = image1;
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            Pen pen = new Pen(Color.White, 3);

            // Create rectangle for ellipse.
            Rectangle rect = new Rectangle(700, 500, 200, 200);

            // Draw ellipse to screen.
            e.Graphics.FillEllipse(Brushes.White, rect);
        }
    }
}

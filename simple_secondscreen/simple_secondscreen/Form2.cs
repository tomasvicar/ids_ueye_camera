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
    public partial class Form2 : Form
    {
        Image image1;
        private Timer timer = new Timer();
        private bool isBlack = true;
        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            BackColor = Color.Black;
            FormBorderStyle = FormBorderStyle.None;
            Bounds = Screen.AllScreens[1].Bounds;
            TopMost = true;




            pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);


            //image1 = Image.FromFile(@"C:\Users\vicar\Desktop\VO_soft_2\flickering_test\ids_ueye_camera\simple_secondscreen\example_img.png");

            //pictureBox1.Image = image1;

            timer.Interval = Convert.ToInt32(1000 / 12.5); // Set the interval to the desired frequency (in milliseconds)
            timer.Tick += Timer_Tick;
            timer.Start();



        }
        private void Timer_Tick(object sender, EventArgs e)
        {
            isBlack = !isBlack; // Toggle the color
            pictureBox1.Invalidate(); // Force the PictureBox to redraw
        }

        private void pictureBox1_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {

            if (isBlack)
            {
                pictureBox1.BackColor = Color.Black;
                e.Graphics.Clear(Color.Black);
            }
            else
            {
                pictureBox1.BackColor = Color.Green;
                e.Graphics.Clear(Color.Green);
            }


            //Pen pen = new Pen(Color.White, 3);

            //// Create rectangle for ellipse.
            //Rectangle rect = new Rectangle(700, 500, 200, 200);

            //// Draw ellipse to screen.
            //e.Graphics.FillEllipse(Brushes.White, rect);

        }
    }
}

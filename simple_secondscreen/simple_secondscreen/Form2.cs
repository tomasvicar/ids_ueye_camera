using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using Timer = System.Windows.Forms.Timer;
using System.Reflection.Emit;
using System.Net;

namespace simple_secondscreen
{
    public partial class Form2 : Form
    {
        Image image1;
        private System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
        private ManualResetEvent _resetEvent = new ManualResetEvent(false);
        private static System.Threading.Timer _timer;
        private bool isBlack = true;
        private int pocitac_tiku = 0;
        private int pocitac_tiku2 = 0;
        private DateTime time_start;
        private DateTime time_end;
        Form1 form1;
        //public Form2()
        //{
        //    InitializeComponent();
        //}

        public Form2(Form1 form1)
        {
            this.form1 = form1;
            InitializeComponent();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            BackColor = Color.Black;
            FormBorderStyle = FormBorderStyle.None;
            Bounds = Screen.AllScreens[1].Bounds;
            TopMost = true;




            pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);



            timer.Interval = Convert.ToInt32(1000 / (10.7 * 2));
            timer.Tick += Timer_Tick;

            timer.Start();




            time_start = DateTime.Now;
            

        }




        //private void Timer_Tick(object sender, EventArgs e)
        //{

        //    pocitac_tiku++;
        //    Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));

        //    if ((pocitac_tiku % 2) == 0)
        //    {
        //        pocitac_tiku2++;
        //    }



        //    isBlack = !isBlack; // toggle the color
        //    //pictureBox1.Invalidate(); // force the picturebox to redraw
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate()));

        //    if ((pocitac_tiku2 % 3) == 0)
        //    {
        //        timer.Interval = Convert.ToInt32(1000 / (16 * 2));
        //    }
        //    else 
        //    {
        //        timer.Interval = Convert.ToInt32(1000 / (12 * 2));
        //    }

        //}

        private void Timer_Tick(object sender, EventArgs e)
        {

            pocitac_tiku++;
            double freq = 1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2));
            Console.WriteLine(freq);
            form1.label2.Text = freq.ToString();

            isBlack = !isBlack; // toggle the color
            //pictureBox1.Invalidate(); // force the picturebox to redraw
            pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate()));


            //if ((pocitac_tiku % 2) == 0)
            //{
            //    if (freq < 12.5)
            //    {
            //        timer.Interval = Convert.ToInt32(1000 / (16 * 2));
            //    }
            //    else
            //    {
            //        timer.Interval = Convert.ToInt32(1000 / (12.5 * 2));
            //    }
            //}

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


            Pen pen = new Pen(Color.Red, 3);
            Rectangle rect = new Rectangle(700, 500, 20, 20);
            e.Graphics.FillEllipse(Brushes.Red, rect);

        }
    }
}

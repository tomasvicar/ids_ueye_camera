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

            timer.Interval = Convert.ToInt32(1000 / (12 * 2)); // Set the interval to the desired frequency (in milliseconds)
            timer.Tick += Timer_Tick;

            timer.Start();


            //TimerCallback timerCallback = new TimerCallback(Timer_Tick);


            //int interval = Convert.ToInt32(1000 / (12 * 2));
            //_timer = new System.Threading.Timer(timerCallback, null, interval, interval);
            //_timer = new System.Threading.Timer(timerCallback, null, interval, interval);

            time_start = DateTime.Now;






        }


        //private void Timer_Tick(object sender, EventArgs e)
        //{
        //    pocitac_tiku++;
        //    Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));


        //    isBlack = !isBlack; // toggle the color
        //    //pictureBox1.Invalidate(); // force the picturebox to redraw
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate()));


        //}


        private void Timer_Tick(object sender, EventArgs e)
        {

            pocitac_tiku++;
            Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));

            if ((pocitac_tiku % 2) == 0)
            {
                pocitac_tiku2++;
            }



            isBlack = !isBlack; // toggle the color
            //pictureBox1.Invalidate(); // force the picturebox to redraw
            pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate()));

            if ((pocitac_tiku2 % 3) == 0)
            {
                timer.Interval = Convert.ToInt32(1000 / (16 * 2));
            }
            else 
            {
                timer.Interval = Convert.ToInt32(1000 / (12 * 2));
            }

        }



        //private async void Timer_Tick(object sender, EventArgs e)
        //{


        //    await Task.Run(() =>
        //    {
        //        pocitac_tiku++;
        //        Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));


        //        isBlack = !isBlack; // toggle the color
        //        // Update the UI
        //        this.Invoke(new Action(() =>
        //        {
        //            pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate()));
        //        }));

        //    });
        //}






        //private void Timer_Tick(object state)
        //{
        //    pocitac_tiku++;
        //    Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));
        //    isBlack = !isBlack; // toggle the color
        //    pictureBox1.Invalidate(); // force the picturebox to redraw

        //}


        //private async void Timer_Tick(object state)
        //{
        //    await Task.Run(() =>
        //    {
        //        pocitac_tiku++;
        //        Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));
        //        isBlack = !isBlack; // Toggle the color
        //        pictureBox1.Invalidate(); // Force the PictureBox to redraw
        //    });
        //}

        //private void Timer_Tick(object state)
        //{
        //    isBlack = !isBlack; // Toggle the color
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate())); // Force the PictureBox to redraw
        //    pocitac_tiku++;

        //    if (pocitac_tiku % (12 * 2) == 0) // Only calculate and print frequency once per second
        //    {
        //        double frequency = 1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2));
        //        Console.WriteLine(frequency);
        //    }
        //}

        //private void Timer_Tick(object state)
        //{

        //    _resetEvent.Reset();

        //    isBlack = !isBlack; // Toggle the color
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate())); // Force the PictureBox to redraw
        //    pocitac_tiku++;

        //    if (pocitac_tiku % (12 * 2) == 0) // Only calculate and print frequency once per second
        //    {
        //        double frequency = 1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2));
        //        Console.WriteLine(frequency);
        //    }


        //    _resetEvent.Set();
        //}



        //private void Timer_Tick(object state)
        //{
        //    var watch = System.Diagnostics.Stopwatch.StartNew();

        //    isBlack = !isBlack; // Toggle the color
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate())); // Force the PictureBox to redraw

        //    watch.Stop();
        //    var elapsedMs = watch.ElapsedMilliseconds;



        //    pocitac_tiku++;

        //    if (pocitac_tiku % (12 * 2) == 0) // Only calculate and print frequency once per second
        //    {
        //        double frequency = 1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2));
        //        Console.WriteLine(frequency);
        //    }

        //    // Adjust the next timer interval
        //    int interval = Convert.ToInt32(1000 / (12 * 2));
        //    Console.WriteLine(interval);
        //    int interval2 = Math.Max(1, interval - (int)elapsedMs); // Ensure interval is at least 1
        //    _timer.Change(interval2, interval2);
        //}


        //private void Timer_Tick(object state)
        //{


        //    isBlack = !isBlack; // Toggle the color
        //    pictureBox1.Invoke(new Action(() => pictureBox1.Invalidate())); // Force the PictureBox to redraw


        //    pocitac_tiku++;

        //    if (pocitac_tiku % (12 * 2) == 0) // Only calculate and print frequency once per second
        //    {
        //        double frequency = 1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2));
        //        Console.WriteLine(frequency);
        //    }

        //    // Adjust the next timer interval
        //    int interval = Convert.ToInt32(1000 / (12 * 2));
        //    int interval2 = Math.Max(1, interval - 5); // Ensure interval is at least 1

        //    _timer.Change(interval2, interval2);
        //}



        //private async void Timer_Tick(object sender, EventArgs e)
        //{
        //    await Task.Run(() =>
        //    {
        //        pocitac_tiku++;
        //        Console.WriteLine(1 / ((DateTime.Now - time_start).TotalSeconds / (pocitac_tiku / 2)));
        //        isBlack = !isBlack; // Toggle the color
        //        pictureBox1.Invalidate(); // Force the PictureBox to redraw
        //    });
        //}

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

            // Create rectangle for ellipse.
            Rectangle rect = new Rectangle(700, 500, 20, 20);

            // Draw ellipse to screen.
            e.Graphics.FillEllipse(Brushes.Red, rect);

        }
    }
}

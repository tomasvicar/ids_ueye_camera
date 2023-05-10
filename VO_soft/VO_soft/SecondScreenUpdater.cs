using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VO_soft
{
    public class SecondScreenUpdater
    {
        private Form1 form1;
        private bool isClosed;

        private Timer timer = new Timer();
        private Timer stopFlickeringTimer = new Timer();
        private bool isBlack = true;

        public SecondScreenUpdater(Form1 form1)
        {
            this.form1 = form1;
            isClosed = true;

            timer.Interval = 1000; 
            timer.Tick += Timer_Tick;


            stopFlickeringTimer.Interval = 10000; // Set the interval to 10 seconds
            stopFlickeringTimer.Tick += StopFlickeringTimer_Tick;

        }

        public void createSecondScreen()
        {
            form1.secondScreenForm = new FormSecondScreen();
            form1.secondScreenForm.form1 = form1;
            form1.secondScreenForm.Show();
            // secondScreenForm = new Stopwatch();

            form1.pictureBox_secondScreen.BackColor = Color.Black;


            int exampleDisplay_w = form1.pictureBox_secondScreen.Size.Width;
            int exampleDisplay_h = form1.pictureBox_secondScreen.Size.Height;

            int secondScreen_w = form1.secondScreenForm.Bounds.Width;
            int secondScreen_h = form1.secondScreenForm.Bounds.Height;
            //int secondScreen_w = 1280;
            //int secondScreen_h = 1024;

            //int secondScreen_w = 960;
            //int secondScreen_h = 1280;

            float screen_ratio = (float)secondScreen_w / (float)secondScreen_h;

            int new_exampleDisplay_w = Convert.ToInt32((float)exampleDisplay_h * screen_ratio);

            form1.pictureBox_secondScreen.Size = new System.Drawing.Size(new_exampleDisplay_w, exampleDisplay_h);


            form1.pictureBox_secondScreen.Paint += pictureBox_exampleDisplay_Paint;
            form1.secondScreenForm.pictureBox1.Paint += pictureBox1_Paint;

            isClosed = false;

        }



        public void updateDot()
        {

            if (isClosed)
                createSecondScreen();

            form1.secondScreenForm.pictureBox1.Refresh();
            form1.pictureBox_secondScreen.Refresh();
        }



        public void pictureBox_exampleDisplay_Paint(object sender, PaintEventArgs e)
        {

            int exampleDisplay_w = form1.pictureBox_secondScreen.Size.Width;
            int exampleDisplay_h = form1.pictureBox_secondScreen.Size.Height;

            int secondScreen_w = form1.secondScreenForm.Bounds.Width;
            int secondScreen_h = form1.secondScreenForm.Bounds.Height;
            //int secondScreen_w = 960;
            //int secondScreen_h = 1280;
            //int secondScreen_w = 1280;
            //int secondScreen_h = 1024;

            float screens_ratio = (float)exampleDisplay_w / (float)secondScreen_h;


            int screen_x_center = form1.pictureBox_secondScreen.Bounds.Width / 2;
            int screen_y_center = form1.pictureBox_secondScreen.Bounds.Height / 2;


            int r = Decimal.ToInt32(form1.numericUpDown_R.Value);
            int x = -Decimal.ToInt32(form1.numericUpDown_dotX.Value);
            int y = Decimal.ToInt32(form1.numericLeftRight_dotY.NumericUpDown.Value);

            r = (int)Math.Ceiling(r * screens_ratio);
            x = Convert.ToInt32((float)x * screens_ratio);
            y = Convert.ToInt32((float)y * screens_ratio);


            if (form1.checkBox_showDot.Checked)
            {


                if (isBlack)
                {
                    form1.secondScreenForm.pictureBox1.BackColor = Color.Black;
                    e.Graphics.Clear(Color.Black);
                }
                else
                {
                    Color color = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_bg_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_b.Value));
                    form1.secondScreenForm.pictureBox1.BackColor = color;
                    e.Graphics.Clear(color);
                }



                //Rectangle rect = new Rectangle( (screen_x_center - r + y) - 32, screen_y_center - r + x, r * 2, r * 2);
                //var tmp = screen_x_center - r + y;
                var tmp = form1.pictureBox_secondScreen.Bounds.Width - (form1.pictureBox_secondScreen.Bounds.Width - (screen_x_center - r + y)) * 1.734;
                Rectangle rect = new Rectangle((int)tmp, screen_y_center - r + x, r * 2, r * 2);

                Color color_point = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_point_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_b.Value));
                SolidBrush brush = new SolidBrush(color_point);
                e.Graphics.FillEllipse(brush, rect);
                brush.Dispose();
            }
        }

        public void pictureBox1_Paint(object sender, PaintEventArgs e)
        {


            int screen_x_center = form1.secondScreenForm.Bounds.Width / 2;
            int screen_y_center = form1.secondScreenForm.Bounds.Height / 2;


            int r = Decimal.ToInt32(form1.numericUpDown_R.Value);
            int x = Decimal.ToInt32(form1.numericUpDown_dotX.Value);
            int y = Decimal.ToInt32(form1.numericLeftRight_dotY.NumericUpDown.Value);



            if (form1.checkBox_showDot.Checked)
            {
                if (isBlack)
                {
                    form1.secondScreenForm.pictureBox1.BackColor = Color.Black;
                    e.Graphics.Clear(Color.Black);
                }
                else
                {
                    Color color = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_bg_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_b.Value));
                    form1.secondScreenForm.pictureBox1.BackColor = color;
                    e.Graphics.Clear(color);
                }

                Rectangle rect = new Rectangle(screen_x_center - r + y, screen_y_center - r - x, r, r);
                

                Color color_point = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_point_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_b.Value));
                SolidBrush brush = new SolidBrush(color_point);
                e.Graphics.FillEllipse(brush, rect);
                brush.Dispose();
            }



        }


        public void flickering_start()
        {
            timer.Interval = Convert.ToInt32(1000 / form1.formSettings.numericUpDown_freq.Value);
            timer.Start();
            stopFlickeringTimer.Interval = Convert.ToInt32(1000 * form1.formSettings.numericUpDown_flicker_len.Value);
            stopFlickeringTimer.Start();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            isBlack = !isBlack; // Toggle the color
            form1.secondScreenForm.pictureBox1.Invalidate(); // Force the PictureBox to redraw
            form1.pictureBox_secondScreen.Invalidate();
        }
        public void flickering_stop()
        {
            timer.Stop();
            isBlack = true; // Set the background color to black
            form1.secondScreenForm.pictureBox1.Invalidate(); // Force the PictureBox to redraw
            form1.pictureBox_secondScreen.Invalidate();
        }

        private void StopFlickeringTimer_Tick(object sender, EventArgs e)
        {
            flickering_stop();
            stopFlickeringTimer.Stop();
            form1.button_flicker.Enabled = true;
        }



        internal void Close()
        {

            

            if (!isClosed)
            {
                form1.pictureBox_secondScreen.Paint -= pictureBox_exampleDisplay_Paint;
                form1.secondScreenForm.Paint -= pictureBox1_Paint;
                form1.secondScreenForm.Close();
                form1.secondScreenForm.Dispose();
                isClosed = true;
            }
        }
    }
}

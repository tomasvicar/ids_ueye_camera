using Accord.Video.FFMPEG;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AxWMPLib;
using PluxDotNet.Event;
using System.Reflection;

namespace VO_soft
{
    public class SecondScreenUpdater
    {
        private Form1 form1;
        private bool isClosed;

        private Timer stopFlickeringTimer = new Timer();
        private bool isBlack = true;
        private int tick_counter;
        private DateTime time_start;
        private int interval;
        private int interval_fast;
        private AxWindowsMediaPlayer player;
        private Form_videoplayer form_videoplayer;

        public SecondScreenUpdater(Form1 form1)
        {
            this.form1 = form1;
            isClosed = true;



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
            form1.cameraBackEnd.SetShow_subsampling(900000);
            form1.pluxBackEnd.dev.subsample_plot = 1000000;

            isBlack = false; // Set the background color to black
            form1.secondScreenForm.pictureBox1.Invalidate(); // Force the PictureBox to redraw
            form1.pictureBox_secondScreen.Invalidate();

            create_video();
            


            Close();

            

            interval = Convert.ToInt32(1000m / (form1.formSettings.numericUpDown_freq.Value * 2m));

            stopFlickeringTimer.Interval = Convert.ToInt32(1000 * form1.formSettings.numericUpDown_flicker_len.Value);
            stopFlickeringTimer.Start();
            form1.ficker_start.Add(DateTime.Now.ToString("HH:mm:ss.ffff"));


            //player.URL = @"C:\Users\vicar\Desktop\VO_soft_2\flickering_test\ids_ueye_camera\simple_secondscreen\simple_secondscr


            form_videoplayer = new Form_videoplayer();
            form_videoplayer.Show();

            time_start = DateTime.Now;
            form1.cameraBackEnd.ComTrigerBoff_execute();

        }


        public void flickering_stop()
        {
            if (form_videoplayer != null)
            {
                if (!form_videoplayer.IsDisposed)
                {
                    form_videoplayer.Close();
                }
            }

            isBlack = true; // Set the background color to black
            form1.secondScreenForm.pictureBox1.Invalidate(); // Force the PictureBox to redraw
            form1.pictureBox_secondScreen.Invalidate();

            updateDot();

            form1.ficker_end.Add(DateTime.Now.ToString("HH:mm:ss.ffff"));
            form1.cameraBackEnd.ComTrigerBon_execute();

            form1.cameraBackEnd.SetShow_subsampling((int)form1.numericUpDown_pictureBoxTimeDecimation.Value);
            form1.pluxBackEnd.dev.subsample_plot = (int)form1.numericUpDown_subsampling.Value;
        }

        private void StopFlickeringTimer_Tick(object sender, EventArgs e)
        {
            flickering_stop();
            stopFlickeringTimer.Stop();
            form1.button_flicker.Enabled = true;

        }

        public void create_video()
        {

            // Create a new video file.
            using (VideoFileWriter vFWriter = new VideoFileWriter())
            {
                int screen_x_center = form1.secondScreenForm.Bounds.Width / 2;
                int screen_y_center = form1.secondScreenForm.Bounds.Height / 2;

                int r = Decimal.ToInt32(form1.numericUpDown_R.Value);
                int x = Decimal.ToInt32(form1.numericUpDown_dotX.Value);
                int y = Decimal.ToInt32(form1.numericLeftRight_dotY.NumericUpDown.Value);



                int sizex = form1.secondScreenForm.Bounds.Width;
                int sizey = form1.secondScreenForm.Bounds.Height;
                float fps = 60;
                // Create new video file
                vFWriter.Open(@".\flicker.avi", sizex, sizey, (Accord.Math.Rational)fps);


                Color color_point = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_point_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_point_b.Value));
                Color color = Color.FromArgb((int)Math.Round(form1.formSettings.numericUpDown_bg_r.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_g.Value), (int)Math.Round(form1.formSettings.numericUpDown_bg_b.Value));


                Bitmap blackBmp = new Bitmap(sizex, sizey);
                using (Graphics g = Graphics.FromImage(blackBmp))
                {
                    g.Clear(Color.Black);
                    using (Brush brush = new SolidBrush(Color.Red))
                    {
                        g.FillEllipse(brush, screen_x_center - r + y, screen_y_center - r - x, r, r);
                    }
                }
                Bitmap lightBmp = new Bitmap(sizex, sizey);
                using (Graphics g = Graphics.FromImage(lightBmp))
                {
                    g.Clear(color);
                    using (Brush brush = new SolidBrush(Color.Red))
                    {
                        g.FillEllipse(brush, screen_x_center - r + y, screen_y_center - r - x, r, r);
                    }
                }


                // Create a new bitmap for each frame
                for (int i = 0; i < fps * (float)form1.formSettings.numericUpDown_flicker_len.Value; i++)
                {
                    float blinkFrequency = (float)form1.formSettings.numericUpDown_freq.Value;
                    float blinkPeriod = 1 / blinkFrequency;
                    float videoPeriod = 1 / fps;
                    float timeModulo = (i * videoPeriod) % blinkPeriod;
                    if (timeModulo < (blinkPeriod / 2))
                    {
                        vFWriter.WriteVideoFrame(blackBmp);
                    }
                    else 
                    {
                        vFWriter.WriteVideoFrame(lightBmp);
                    }

                }
                // Close the video file.
                vFWriter.Close();

            }
        }






        internal void Close()
        {

            

            if (!isClosed)
            {
                //form1.pictureBox_secondScreen.Paint -= pictureBox_exampleDisplay_Paint;
                form1.secondScreenForm.Paint -= pictureBox1_Paint;
                form1.secondScreenForm.Close();
                form1.secondScreenForm.Dispose();
                isClosed = true;
            }
        }
    }
}

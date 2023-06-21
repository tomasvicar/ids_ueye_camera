using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Accord.Video.FFMPEG;

namespace simple_secondscreen
{
    public partial class Form1 : Form
    {
        Form f2;
        Form f3;
        private Form4 f4;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            

        }

        private void button_open_Click(object sender, EventArgs e)
        {


            f3 = new Form3();
            f3.Show();
        }

        private void button_close_Click(object sender, EventArgs e)
        {
            if (f2 != null)
            {
                if (!f2.IsDisposed)
                {
                    f2.Close();
                }
            }

            if (f3 != null)
            {
                if (!f3.IsDisposed)
                {
                    f3.Close();
                }
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            f2 = new Form2(this);
            f2.Show();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            f4 = new Form4(this);
            f4.Show();
        }

        private void button_create_video_Click(object sender, EventArgs e)
        {
            // Create a new video file.
            using (VideoFileWriter vFWriter = new VideoFileWriter())
            {
                int sizex = 800;
                int sizey = 600;
                int fps = 30;
                // Create new video file
                vFWriter.Open(@"C:\Users\vicar\Desktop\VO_soft_2\flickering_test\ids_ueye_camera\simple_secondscreen\simple_secondscreen\bin\Debug\test.avi", sizex, sizey, fps);

                // Create a new bitmap for each frame
                for (int i = 0; i < fps; i++)
                {
                    using (Bitmap bmp = new Bitmap(sizex, sizey))
                    {
                        // Create a graphics object for drawing.
                        using (Graphics g = Graphics.FromImage(bmp))
                        {
                            // Set the background color.
                            g.Clear(i % 2 == 0 ? Color.Black : Color.Green);


                            // Draw a red circle.
                            using (Brush brush = new SolidBrush(Color.Red))
                            {
                                g.FillEllipse(brush, 400, 300, 40, 40);
                            }
                        }

                        // Write the frame to the video file.
                        vFWriter.WriteVideoFrame(bmp);
                    }
                }

                // Close the video file.
                vFWriter.Close();
            }

        }
    }
}

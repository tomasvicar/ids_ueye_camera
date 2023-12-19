using simple_live_windows_forms;
using System.Diagnostics;
using System.Windows.Forms;

namespace simple_aquisition
{
    public partial class Form1 : Form
    {
        private BackEnd backEnd;
        private bool hasError;

        public Form1()
        {
            InitializeComponent();

            try
            {

                backEnd = new BackEnd();
                FormClosing += FormWindow_FormClosing;

                backEnd.ImageReceived1 += backEnd_ImageReceived1;
                backEnd.CounterChanged1 += backEnd_CounterChanged1;
                backEnd.MessageBoxTrigger1 += backEnd_MessageBoxTrigger1;

                backEnd.ImageReceived2 += backEnd_ImageReceived2;
                backEnd.CounterChanged2 += backEnd_CounterChanged2;
                backEnd.MessageBoxTrigger2 += backEnd_MessageBoxTrigger2;


            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger1(this, "Exception", e.Message);
            }

        }



        private void backEnd_ImageReceived1(object sender, Bitmap image)
        {
            try
            {
                pictureBox_1.Invoke(new Action(() =>
                {
                    var previousImage = pictureBox_1.Image;
                    pictureBox_1.Image = (Bitmap)image.Clone();
                    previousImage?.Dispose();
                }));
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger1(this, "Exception", e.Message);
            }
        }


        private void backEnd_CounterChanged1(object sender, uint frameCounter, uint errorCounter)
        {
            if (counterLabel1.InvokeRequired)
            {
                counterLabel1.Invoke(new Action(() =>
                {
                    counterLabel1.Text = "Frames: " + frameCounter + ", Errors: " + errorCounter;
                }));
            }
            else
            {
                counterLabel1.Text = "Frames: " + frameCounter + ", Errors: " + errorCounter;
            }
        }


        private void backEnd_MessageBoxTrigger1(object sender, String messageTitle, String messageText)
        {
            MessageBox.Show(messageText, messageTitle);
        }






        private void backEnd_ImageReceived2(object sender, Bitmap image)
        {
            try
            {
                pictureBox_2.Invoke(new Action(() =>
                {
                    var previousImage = pictureBox_2.Image;
                    pictureBox_2.Image = (Bitmap)image.Clone();
                    previousImage?.Dispose();
                }));
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger2(this, "Exception", e.Message);
            }
        }


        private void backEnd_CounterChanged2(object sender, uint frameCounter, uint errorCounter)
        {
            if (counterLabel2.InvokeRequired)
            {
                counterLabel2.Invoke(new Action(() =>
                {
                    counterLabel2.Text = "Frames: " + frameCounter + ", Errors: " + errorCounter;
                }));
            }
            else
            {
                counterLabel2.Text = "Frames: " + frameCounter + ", Errors: " + errorCounter;
            }
        }


        private void backEnd_MessageBoxTrigger2(object sender, String messageTitle, String messageText)
        {
            MessageBox.Show(messageText, messageTitle);
        }




        private void FormWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            Debug.WriteLine("--- [FormWindow] Closing");
            backEnd.Stop();
        }

        public bool HasError()
        {
            return hasError;
        }





        private void button_play_Click(object sender, EventArgs e)
        {
            if (backEnd.start())
            {
                hasError = false;
            }
            else
            {
                hasError = true;
            }

        }

        private void button_rec_Click(object sender, EventArgs e)
        {

        }
    }
}
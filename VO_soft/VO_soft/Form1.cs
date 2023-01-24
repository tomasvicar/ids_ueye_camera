using Accord.Video.FFMPEG;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace VO_soft
{
    public partial class Form1 : Form
    {
        public CameraParameters cameraParameters;
        public CameraBackEnd cameraBackEnd;
        private PluxBackEnd pluxBackEnd;
        private bool stopTrigerClicked2;
        internal bool is_triger;
        public FormSettings formSettings;
        private bool stopTrigerClicked;
        private VideoFileWriter videoWriter;
        
        public Chart chart1;
        public Chart chart2;
        public Chart chart3;
        public Chart chart4;
        public Chart chart5;
        public Chart chart6;
        public Chart chart7;
        public Chart chart8;

        public Form1()
        {
            #if (DEBUG == false)
                path = "logs";

                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }

                time = DateTime.Now;
                CultureInfo.CurrentCulture = CultureInfo.GetCultureInfo("cs-CZ");

                filename = Path.Combine(path, "log" + "_" + time.ToString().Replace(".", "_").Replace(":", "_").Replace(" ", "_") + ".txt");

                FileStream filestream = new FileStream(filename, FileMode.Create);
                var streamwriter = new StreamWriter(filestream);
                streamwriter.AutoFlush = true;
                Console.SetOut(streamwriter);
                Console.SetError(streamwriter);
            #endif


            InitializeComponent();

            this.Show();

            formSettings = new FormSettings(this);




            cameraParameters = new CameraParameters();
            cameraBackEnd = new CameraBackEnd(this);
            pluxBackEnd = new PluxBackEnd(this);

            cameraBackEnd.getParams();
            Form1Setter.LoadMySetting(this, cameraParameters);
            loadSettingsToolStripMenuItem_Click(this, EventArgs.Empty);
            Form1Setter.SetCharts(this);

            cameraBackEnd.ImageReceived += backEnd_ImageReceived;
            cameraBackEnd.CountersUpdated += backEnd_CountersUpdated;
            cameraBackEnd.MessageBoxTrigger += backEnd_MessageBoxTrigger;
            cameraBackEnd.ComTrigerOn += backEnd_ComTrigerOn;

            Thread.Sleep(1000);

            label_comPortStatus_Click(null, EventArgs.Empty);

            //label_pluxState_Click(this, EventArgs.Empty);

            



        }

        private void backEnd_ComTrigerOn(object sender, EventArgs args)
        {
            cameraBackEnd.ComTrigerOn_execute();
        }

        private void backEnd_MessageBoxTrigger(object sender, string messageTitle, string messageText)
        {
            if (messageText.Contains("Error-Text: Wait for event data timed out! Timeout") & stopTrigerClicked)
            {
                buttonStop.BeginInvoke((MethodInvoker)delegate { myStop(); });
                stopTrigerClicked = false;
            }
            else
            {
                MessageBox.Show(messageText, messageTitle);
            }
        }

        private void myStop()
        {
            cameraBackEnd.Stop();


            if (is_triger)
            {
                videoWriter.Close();

                button_pluxStop_Click(this, EventArgs.Empty);
            }
        }

        private void backEnd_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            if (labelCounter.InvokeRequired)
            {
                labelCounter.BeginInvoke((MethodInvoker)delegate { labelCounter.Text = frameCounter.ToString(); });
            }

            if (labelCounter.InvokeRequired)
            {
                labelCounter.BeginInvoke((MethodInvoker)delegate {

                    if (errorCounter == 0)
                    {
                        label_error.ForeColor = Color.Black;
                    }

                    if (!stopTrigerClicked2)
                    {
                        label_error.Text = errorCounter.ToString();
                        if (errorCounter != 0)
                        {
                            label_error.ForeColor = Color.Red;
                        }
                    }
                });
            }
        }

        private void backEnd_ImageReceived(object sender, Bitmap image, uint counter)
        {
            ImageUpdater.updateImage(image, counter, this);
        }


        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void label_comPortStatus_Click(object sender, EventArgs e)
        {
            cameraBackEnd.connectCom();
        }

        private void label_pluxState_Click(object sender, EventArgs e)
        {
            if (pluxBackEnd.openPlux())
            {
                label_pluxState.Text = "connected";
                label_pluxState.ForeColor = Color.Green;
            }
            else
            {
                label_pluxState.Text = "NA";
                label_pluxState.ForeColor = Color.Red;
            }
        }

        private void loadSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaverLoaderSettings.Load(this);
        }

        private void saveSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaverLoaderSettings.Save(this);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            cameraBackEnd.close();
            pluxBackEnd.closePlux();
        }

        public void numericUpDown_gain_ValueChanged(object sender, EventArgs e)
        {
            if (!(cameraBackEnd == null))
            {
                if (cameraBackEnd.IsActive())
                {
                    cameraBackEnd.adjustParam("Gain");
                }
            }
        }

        public void numericUpDown_frameRate_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_frameRate.ValueChanged -= new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);
            numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);

            if (numericUpDown_frameRate.Value > cameraParameters.frameRateHardMax)
            {
                numericUpDown_frameRate.Value = cameraParameters.frameRateHardMax;
            }

            numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / numericUpDown_frameRate.Value - cameraParameters.exposureSafeMargin, 1);
            label_exposureTimeMax.Text = numericUpDown_exposureTime.Maximum.ToString();


            if (checkBox_exposurTimeMax.Checked)
            {

                numericUpDown_exposureTime.Value = Decimal.Round(1000 / numericUpDown_frameRate.Value - cameraParameters.exposureSafeMargin, 1);

            }

            updatePixelClock();

            updateCameraParams();


            numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);
            numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
        }

        public void updatePixelClock()
        {
            label_pixelClock.Text = cameraBackEnd.getPixelClock(numericUpDown_frameRate.Value, formSettings.numericUpDown_h.Value).ToString() + "Mclock";
        }

        public void numericUpDown_exposureTime_ValueChanged(object sender, EventArgs e)
        {
            updateCameraParams();
        }

        private void button_triger_Click(object sender, EventArgs e)
        {
            stopTrigerClicked2 = false;
            if (buttonStart.Enabled == false)
            {
                myStop();
            }


            buttonStart.Enabled = false;
            buttonStop.Enabled = false;
            button_triger.Enabled = false;
            button_stopTriger.Enabled = true;


            is_triger = true;
            myStart();

            button_pluxStart.Enabled = false;
            button_pluxStop.Enabled = false;
            numericUpDown_red.Enabled = false;
            numericUpDown_IR.Enabled = false;
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            stopTrigerClicked2 = false;

            is_triger = false;
            buttonStart.Enabled = false;
            buttonStop.Enabled = true;
            button_triger.Enabled = true;
            button_stopTriger.Enabled = false;

            myStart();
        }

        private void myStart()
        {
            stopTrigerClicked = false;

            if (is_triger)
            {



                var path = textBox_dataname.Text;

                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }

                videoWriter = new VideoFileWriter();

                var time = DateTime.Now;
                CultureInfo.CurrentCulture = CultureInfo.GetCultureInfo("cs-CZ");

                var filename = Path.Combine(path, textBox_dataname.Text + '_' + time.ToString().Replace(".", "_").Replace(":", "_").Replace(" ", "_") + ".avi");

                // http://accord-framework.net/docs/html/T_Accord_Video_FFMPEG_VideoCodec.htm
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.MPEG4); 
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.Raw);
                videoWriter.Open(filename, Decimal.ToInt32(formSettings.numericUpDown_w.Value), Decimal.ToInt32(formSettings.numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.FFV1);

                pluxBackEnd.startRecordPlux(filename.Replace(".avi", ".txt"));
                cameraBackEnd.Start();

            }
        }

        private void button_stopTriger_Click(object sender, EventArgs e)
        {

            buttonStart.Enabled = true;
            buttonStop.Enabled = false;
            button_triger.Enabled = true;
            button_stopTriger.Enabled = false;
            button_pluxStart.Enabled = true;
            button_pluxStop.Enabled = false;
            numericUpDown_red.Enabled = true;
            numericUpDown_IR.Enabled = true;

            stopTrigerClicked = true;
            stopTrigerClicked2 = true;
            cameraBackEnd.ComTrigerOff();
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            buttonStop.Enabled = false;
            buttonStart.Enabled = true;
            button_triger.Enabled = true;
            button_stopTriger.Enabled = false;

            myStop();
        }

        public void button_pluxStop_Click(object sender, EventArgs e)
        {
            button_pluxStart.Enabled = true;
            button_pluxStop.Enabled = false;
            numericUpDown_red.Enabled = true;
            numericUpDown_IR.Enabled = true;

            pluxBackEnd.stop();
        }

        public void updateCameraParams()
        {
            if (!(cameraBackEnd == null))
            {
                if (cameraBackEnd.IsActive())
                {
                    var frameRateTmp = numericUpDown_frameRate.Value;
                    numericUpDown_frameRate.Value = numericUpDown_frameRate.Minimum;
                    cameraBackEnd.adjustParam("AcquisitionFrameRate");
                    numericUpDown_frameRate.Value = frameRateTmp;

                    cameraBackEnd.adjustParam("ExposureTime");
                    cameraBackEnd.adjustParam("DeviceClockFrequency");
                    cameraBackEnd.adjustParam("AcquisitionFrameRate");


                }
            }


        }

        private void checkBox_exposurTimeMax_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_exposurTimeMax.Checked)
            {
                numericUpDown_exposureTime.Enabled = false;
                numericUpDown_exposureTime.Value = numericUpDown_exposureTime.Maximum;
            }
            else
            {
                numericUpDown_exposureTime.Enabled = true;
            }
        }

        private void checkBox_LED_CheckedChanged(object sender, EventArgs e)
        {
            cameraBackEnd.comLedUpdate();
        }

        private void numericUpDown_LED_ValueChanged(object sender, EventArgs e)
        {
            cameraBackEnd.comLedUpdate();
        }

        public void button_pluxStart_Click(object sender, EventArgs e)
        {
            button_pluxStart.Enabled = false;
            button_pluxStop.Enabled = true;
            numericUpDown_red.Enabled = false;
            numericUpDown_IR.Enabled = false;
            pluxBackEnd.startPlux();
        }

        private void advancedSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            formSettings.ShowDialog();
        }
    }
}

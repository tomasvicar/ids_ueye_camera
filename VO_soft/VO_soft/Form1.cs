using Accord.Video.FFMPEG;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
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
        public PluxBackEnd pluxBackEnd;
        private bool stopTrigerClicked2;
        internal bool is_triger;
        public bool is_recording;
        public FormSettings formSettings;
        public FormAbout formAbout;
        public SecondScreenUpdater secondScreenUpdater;
        private ImageUpdater imageUpdater;
        private bool stopTrigerClicked;
        //private VideoFileWriter videoWriter;
        private Stopwatch sw = new Stopwatch();
        public string filename;

        public List<String> ficker_start = new List<String>();
        public List<String> ficker_end = new List<String>();

        public Chart chart1;
        public Chart chart2;
        public Chart chart3;
        public Chart chart4;
        public Chart chart5;
        public Chart chart6;
        public Chart chart7;
        public Chart chart8;
        internal FormSecondScreen secondScreenForm;

        public Form1()
        {
            #if (DEBUG == false)
                var path = "logs";

                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }

                var time = DateTime.Now;
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
            formAbout = new FormAbout();
            secondScreenUpdater = new SecondScreenUpdater(this);


            imageUpdater = new ImageUpdater();
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

            label_pluxState_Click(this, EventArgs.Empty);

            formSettings.checkBox_visible_led_CheckedChanged(null, EventArgs.Empty);
            formSettings.checkBox_visible_two_wl_CheckedChanged(null, EventArgs.Empty);
            formSettings.checkBox_display_CheckedChanged(null, EventArgs.Empty);
            checkBox_LED_CheckedChanged(null, EventArgs.Empty);






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
                if (messageText.Contains("Error-Text: Wait for event data timed out! Timeout"))
                {
                    Console.WriteLine(messageTitle + messageText);
                }
                else
                {
                    MessageBox.Show(messageText, messageTitle);
                }


            }
        }

        private void myStop()
        {
            cameraBackEnd.Stop();


            if (is_recording)
            {
                //videoWriter.Close();

                button_pluxStop_Click(this, EventArgs.Empty);
            }

            cameraBackEnd.ComTrigerXoff_execute();
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

            if (labelCounter.InvokeRequired)
            {
                labelCounter.BeginInvoke((MethodInvoker)delegate
                {
                    if (((int.Parse(labelCounter.Text) % 40) == 10) & (int.Parse(labelCounter.Text) > 10))
                    {
                        if (sw.IsRunning)
                        {

                            TimeSpan ts = sw.Elapsed;
                            sw.Restart();

                            label_fps.Text = Math.Round(40.0 / ts.TotalSeconds).ToString() + "fps";

                            if (Convert.ToDecimal(Math.Round(40.0 / ts.TotalSeconds)) != Math.Round(numericUpDown_frameRate.Value))
                            {
                                label_fps.ForeColor = Color.Red;
                            }
                            else
                            {
                                label_fps.ForeColor = Color.Black;
                            }
                        }
                        else
                        {
                            sw.Start();
                        }
                    }
                });
            }
        }

        private void backEnd_ImageReceived(object sender, Bitmap image, uint counter)
        {
            imageUpdater.updateImage(image, counter, this);
        }


        private void label_comPortStatus_Click(object sender, EventArgs e)
        {
            cameraBackEnd.connectCom();
        }

        public void label_pluxState_Click(object sender, EventArgs e)
        {
            label_pluxState.Text = "Connecting";
            label_pluxState.ForeColor = System.Drawing.Color.Gray;
            label_pluxState.Refresh();

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
            SaverLoaderSettings.Load(this, "settings.json");
        }

        private void saveSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaverLoaderSettings.Save(this, "settings.json");
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

            //if (formSettings.checkBox_visible_two_wl.Checked)
            //    cameraBackEnd.ComTrigerSoff_execute();
            ficker_start = new List<String>();
            ficker_end = new List<String>();


            stopTrigerClicked2 = false;

            //if (buttonStart.Enabled == false)
            //{
            //    cameraBackEnd.ComTrigerOff();
            //    myStop();
            //    Thread.Sleep(500);
            //}

            //Thread.Sleep(1000);
            cameraBackEnd.ComTrigerOff();
            myStop();
            Thread.Sleep(500);


            is_triger = true;
            is_recording = true;

            buttonStart.Enabled = false;
            buttonStop.Enabled = false;
            button_triger.Enabled = false;
            button_stopTriger.Enabled = true;

            if (formSettings.checkBox_visible_two_wl.Checked)
            {
                if (checkBox_2xfps.Checked)
                {
                    numericUpDown_frameRate.Value = numericUpDown_frameRate.Value * 2;
                    numericUpDown_gain.Value = numericUpDown_gain2xfps.Value;
                }
            }


           
            myStart();

            button_pluxStart.Enabled = false;
            button_pluxStop.Enabled = false;
            numericUpDown_red.Enabled = false;
            numericUpDown_IR.Enabled = false;
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (formSettings.checkBox_visible_two_wl.Checked)
                if (checkBox_one_wl_stable.Checked)
                    cameraBackEnd.ComTrigerS_execute();

            stopTrigerClicked2 = false;

            is_triger = true;
            is_recording = false;


            buttonStart.Enabled = false;
            buttonStop.Enabled = true;
            button_triger.Enabled = true;
            button_stopTriger.Enabled = false;

            myStart();
        }

        private void myStart()
        {
            cameraBackEnd.ComTrigerX_execute();

            stopTrigerClicked = false;
            

            //formSettings.checkBox_visible_two_wl_CheckedChanged(null, EventArgs.Empty);


            if (is_recording)
            {


                var time = DateTime.Now;
                CultureInfo.CurrentCulture = CultureInfo.GetCultureInfo("cs-CZ");
                var tmp = textBox_dataname.Text + '_' + time.ToString().Replace(".", "_").Replace(":", "_").Replace(" ", "_");


                var path = Path.Combine(formSettings.textBox_save_path.Text, textBox_dataname.Text, tmp);


                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }

                //videoWriter = new VideoFileWriter();

                

                filename = Path.Combine(path, tmp);

                // http://accord-framework.net/docs/html/T_Accord_Video_FFMPEG_VideoCodec.htm
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.MPEG4); 
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.Raw);
                // videoWriter.Open(filename, Decimal.ToInt32(formSettings.numericUpDown_w.Value), Decimal.ToInt32(formSettings.numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.FFV1);

                SaverLoaderSettings.Save(this, filename + "_settings.json");

                pluxBackEnd.startRecordPlux(filename + ".txt");
            }
            cameraBackEnd.Start();


            /// for 2wls is required?
            //checkBox_one_wl_stable.CheckedChanged -= checkBox_one_wl_stable_CheckedChanged;
            //checkBox_one_wl_stable.Checked = false;
            //checkBox_one_wl_stable.CheckedChanged += checkBox_one_wl_stable_CheckedChanged;
        }

        private void button_stopTriger_Click(object sender, EventArgs e)
        {

            Dictionary<string, object> _data = new Dictionary<string, object>();
            _data.Add("flicker_start", ficker_start);
            _data.Add("flicker_end", ficker_end);
            string json = Newtonsoft.Json.JsonConvert.SerializeObject(_data);
            File.WriteAllText(filename + "_flicker.json", json);


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

            stopTrigerClicked = true;
            stopTrigerClicked2 = true;
            cameraBackEnd.ComTrigerOff();
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

        public void checkBox_LED_CheckedChanged(object sender, EventArgs e)
        {
            cameraBackEnd.comLedUpdate();
        }

        public void numericUpDown_LED_ValueChanged(object sender, EventArgs e)
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

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            formAbout.ShowDialog();
        }

        public void checkBox_show_1_wl_CheckedChanged(object sender, EventArgs e)
        {
            if (formSettings.checkBox_visible_two_wl.Checked == false)
            {
                pictureBoxWithInterpolationMode1.Visible = true;
                pictureBoxWithInterpolationMode2.Visible = false;
                pictureBoxWithInterpolationMode3.Visible = false;
            }
            else if (checkBox_show_1_wl.Checked)
            {
                pictureBoxWithInterpolationMode1.Visible = true;
                pictureBoxWithInterpolationMode2.Visible = false;
                pictureBoxWithInterpolationMode3.Visible = false;
            }
            else 
            {
                pictureBoxWithInterpolationMode1.Visible = false;
                pictureBoxWithInterpolationMode2.Visible = true;
                pictureBoxWithInterpolationMode3.Visible = true;
            }
        }

        private void checkBox_showDot_CheckedChanged(object sender, EventArgs e)
        {
            if (formSettings.checkBox_display.Checked)
                secondScreenUpdater.updateDot();
        }

        private void numericUpDown_dotX_ValueChanged(object sender, EventArgs e)
        {
            if (formSettings.checkBox_display.Checked)
                secondScreenUpdater.updateDot();
        }

        public void numericLeftRight_dotY_ValueChanged(object sender, EventArgs e)
        {
            if (formSettings.checkBox_display.Checked)
                secondScreenUpdater.updateDot();
        }


        private void numericUpDown_R_ValueChanged(object sender, EventArgs e)
        {
            if (formSettings.checkBox_display.Checked)
                secondScreenUpdater.updateDot();
        }

        private void pictureBox_secondScreen_Click(object sender, EventArgs e)
        {
            if (formSettings.checkBox_display.Checked)
                secondScreenUpdater.updateDot();
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == (Keys.Up))
            {
                numericUpDown_dotX.Value = numericUpDown_dotX.Value + numericUpDown_dotX.Increment;
                return true;
            }
            if (keyData == (Keys.Down))
            {
                numericUpDown_dotX.Value = numericUpDown_dotX.Value - numericUpDown_dotX.Increment;
                return true;
            }
            if (keyData == (Keys.Left))
            {
                numericLeftRight_dotY.NumericUpDown.Value = numericLeftRight_dotY.NumericUpDown.Value - numericLeftRight_dotY.NumericUpDown.Increment;
                return true;
            }
            if (keyData == (Keys.Right))
            {
                numericLeftRight_dotY.NumericUpDown.Value = numericLeftRight_dotY.NumericUpDown.Value + numericLeftRight_dotY.NumericUpDown.Increment;
                return true;
            }
            if (keyData == (Keys.Subtract))
            {
                numericUpDown_R.Value = numericUpDown_R.Value - numericUpDown_R.Increment;
                return true;
            }

            if (keyData == (Keys.Add))
            {
                numericUpDown_R.Value = numericUpDown_R.Value + numericUpDown_R.Increment;
                return true;
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void saveSettingsAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog mySaveFileDialog = new SaveFileDialog();
            mySaveFileDialog.FileName = "settings.json";
            mySaveFileDialog.InitialDirectory = ".";
            mySaveFileDialog.Filter = "JSON|*.json";
            if (!(mySaveFileDialog.ShowDialog() == DialogResult.OK))
            {
                return;
            }


            SaverLoaderSettings.Save(this, mySaveFileDialog.FileName);
        }

        private void loadSettingsAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog myOpenFileDialog = new OpenFileDialog();
            myOpenFileDialog.FileName = "settings.json";
            myOpenFileDialog.InitialDirectory = ".";
            myOpenFileDialog.Filter = "JSON|*.json";
            if (!(myOpenFileDialog.ShowDialog() == DialogResult.OK))
            {
                return;
            }

            SaverLoaderSettings.Load(this, myOpenFileDialog.FileName);
        }

        private void checkBox_one_wl_stable_CheckedChanged(object sender, EventArgs e)
        {
            if (label_comPortStatus.Text != "NA")
            {
                if (checkBox_one_wl_stable.Checked)
                {
                    cameraBackEnd.ComTrigerS_execute();
                }
                else
                {
                    cameraBackEnd.ComTrigerSoff_execute();
                }
            }

        }

        private void button_flicker_Click(object sender, EventArgs e)
        {

            secondScreenUpdater.flickering_start();
            button_flicker.Enabled = false;
        }

    }
}

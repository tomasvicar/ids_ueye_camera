using peak.core;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace VO_soft
{
    public class CameraBackEnd
    {
        // Event which is raised if a new image was received
        public delegate void ImageReceivedEventHandler(object sender, Bitmap image, uint counter);
        public event ImageReceivedEventHandler ImageReceived;

        // Event which is raised if the counters has changed
        public delegate void CountersUpdatedEventHandler(object sender, uint frameCounter, uint errorCounter);
        public event CountersUpdatedEventHandler CountersUpdated;

        public delegate void ComTrigerOnEventHandler(object sender, EventArgs args);
        public event ComTrigerOnEventHandler ComTrigerOn;


        // Event which is raised if an Error or Exception has occurred
        public delegate void MessageBoxTriggerEventHandler(object sender, String messageTitle, String messageText);
        public event MessageBoxTriggerEventHandler MessageBoxTrigger;


        //private AcquisitionWorker acquisitionWorker;
        private CameraAcquisitionWorker cameraAcquisitionWorker;
        private Thread cameraAcquisitionThread;

        private Device device;
        private DataStream dataStream;
        private NodeMap nodeMap;
        private peak.core.Buffer buffer;
        private Form1 form1;
        private CameraSetter cameraSetter;

        private bool isActive;
        private bool isOpen;
        private SerialPort comPort;

        public CameraBackEnd(Form1 form1)
        {

            peak.Library.Initialize();
            this.form1 = form1;
            OpenDevice();

        }

        public void Start()
        {

            cameraAcquisitionWorker = new CameraAcquisitionWorker();

            cameraAcquisitionThread = new Thread(new ThreadStart(cameraAcquisitionWorker.Start));

            cameraAcquisitionWorker.ImageReceived += cameraAcquisitionWorker_ImageReceived;
            cameraAcquisitionWorker.CountersUpdated += cameraAcquisitionWorker_CountersUpdated;
            cameraAcquisitionWorker.MessageBoxTrigger += cameraAcquisitionWorker_MessageBoxTrigger;
            cameraAcquisitionWorker.ComTrigerOn += cameraAcquisitionWorker_ComTrigerOn;

            var dataStreams = device.DataStreams();
            dataStream = dataStreams[0].OpenDataStream();

            
            cameraSetter.SetStart(form1.is_triger);

            UInt32 payloadSize = Convert.ToUInt32(nodeMap.FindNode<peak.core.nodes.IntegerNode>("PayloadSize").Value());
            UInt32 bufferCountMax = decimal.ToUInt32(form1.numericUpDown_bufferSize.Value);

            for (var bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
            {
                buffer = dataStream.AllocAndAnnounceBuffer(payloadSize, IntPtr.Zero);
                dataStream.QueueBuffer(buffer);
            }

            cameraAcquisitionWorker.SetDataStream(dataStream);
            cameraAcquisitionWorker.SetNodemapRemoteDevice(nodeMap);
            //cameraAcquisitionWorker.SetFormWindow(form1);

            var is_triger = form1.is_triger;
            var is_recording = form1.is_recording;
            var subsample = 1;
            if (is_triger)
                subsample = Decimal.ToInt32(form1.numericUpDown_pictureBoxTimeDecimation.Value);
            else
                subsample = Decimal.ToInt32(form1.numericUpDown_pictureBoxTimeDecimation_play.Value);

            var fps = Decimal.ToInt32(form1.numericUpDown_frameRate.Value);
            var width = Decimal.ToInt32(form1.formSettings.numericUpDown_w.Value);
            var height = Decimal.ToInt32(form1.formSettings.numericUpDown_h.Value);
            var bits = Decimal.ToInt32(form1.formSettings.numericUpDown_bits.Value);
            var one_wl_stable = form1.checkBox_one_wl_stable.Checked;
            cameraAcquisitionWorker.SetAquisitionsettings(is_triger, is_recording, subsample, form1.filename, fps, width, height, bits, one_wl_stable);

            cameraAcquisitionThread.Start();

            isActive = true;

        }

        public void Stop()
        {
            isActive = false;

            if (cameraAcquisitionWorker != null)
                cameraAcquisitionWorker.Stop();

            if (cameraAcquisitionThread != null)
            {
                if (cameraAcquisitionThread.IsAlive)
                {
                    cameraAcquisitionThread.Join();
                }
            }

            if (device != null)
            {
                try
                {
                    nodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").Execute();
                    nodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").WaitUntilDone();
                    nodeMap.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(0);
                }
                catch (Exception e)
                {
                    MessageBoxTrigger(this, "Exception", e.Message);
                }
            }


            if (dataStream != null)
            {
                try
                {
                    dataStream.KillWait();
                    dataStream.StopAcquisition(peak.core.AcquisitionStopMode.Default);
                    dataStream.Flush(peak.core.DataStreamFlushMode.DiscardAll);

                    foreach (var buffer in dataStream.AnnouncedBuffers())
                    {
                        dataStream.RevokeBuffer(buffer);
                    }

                    dataStream.Dispose();
                    cameraAcquisitionWorker.SetDataStream(null);

                }
                catch (Exception e)
                {
                    MessageBoxTrigger(this, "Exception", e.Message);
                }
            }




        }

        internal void connectCom()
        {
            form1.label_comPortStatus.Text = "Connecting";
            form1.label_comPortStatus.ForeColor = System.Drawing.Color.Gray;
            form1.label_comPortStatus.Refresh();

            var ArrayComPortsNames = SerialPort.GetPortNames();



            if (!(comPort == null))
            {
                if (comPort.IsOpen)
                {
                    comPort.Close();
                    Thread.Sleep(3000);
                }

            }



            foreach (string port in ArrayComPortsNames)
            {





                try
                {



                    Console.WriteLine(port);
                    comPort = new SerialPort(port, 9600);



                    comPort.Open();


                    comPort.WriteTimeout = 500;
                    comPort.ReadTimeout = 2000;

                    comPort.WriteLine("ok?");



                    string message = comPort.ReadLine();
                    Console.WriteLine(message);

                    if (message.Trim() == "ok")
                    {
                        form1.label_comPortStatus.Text = port;

                        form1.label_comPortStatus.ForeColor = System.Drawing.Color.Green;

                        comPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

                        comLedUpdate();

                        ComTrigerOff();


                    }
                    else
                    {
                        comPort.Close();

                    }

                }
                catch (TimeoutException)
                {
                    Console.WriteLine("time out");
                    comPort.Close();
                }
                catch (Exception ex)
                {

                    Console.WriteLine(ex.ToString());
                    comPort.Close();
                }



            }

            if (form1.label_comPortStatus.Text == "Connecting")
            {
                Console.WriteLine("COM step 10");


                form1.label_comPortStatus.Text = "NA";

                form1.label_comPortStatus.ForeColor = System.Drawing.Color.Red;
            }

            Console.WriteLine("COM step 11");


        }
        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();
        }

        internal void close()
        {
            if (this.IsActive())
            {
                this.Stop();
            }
            if (this.IsOpen())
            {
                this.CloseDevice();
            }

            this.ComClose();
        }

        internal void ComClose()
        {
            if (!(comPort == null))
            {
                if (comPort.IsOpen)
                {
                    comPort.Close();
                }

            }
        }

        public void OpenDevice()
        {
            try
            {
                var deviceManager = peak.DeviceManager.Instance();
                deviceManager.Update();

                var deviceCount = deviceManager.Devices().Count();

                for (var i = 0; i < deviceCount; ++i)
                {
                    if (deviceManager.Devices()[i].IsOpenable())
                    {
                        device = deviceManager.Devices()[i].OpenDevice(peak.core.DeviceAccessType.Control);

                        // Stop after the first opened device
                        break;
                    }
                    else if (i == (deviceCount - 1))
                    {
                        MessageBoxTrigger(this, "Error", "Device could not be openend");
                    }
                }

                nodeMap = device.RemoteDevice().NodeMaps()[0];
                cameraSetter = new CameraSetter(nodeMap, form1);
                cameraSetter.initSet();

                isOpen = true;

            }
            catch (Exception e)
            {
                MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        public void SetShow_subsampling(int show_subsampling)
        {
            //cameraAcquisitionWorker.SetShow_subsampling(show_subsampling);
        }

        internal object getPixelClock(decimal fps, decimal heght)
        {
            //decimal pc_orig = ((fps * heght) / cameraSetter.getC()) * 1.3m;

            ////decimal[] allowed_values = { 470m, 237m, 118m, 59m, 30m, 0m };
            //decimal[] allowed_values = { 118m, 59m, 30m, 0m }; /// is it just this?
            //decimal maxValue = form1.cameraParameters.deviceClockFrequency_max;
            //decimal previousValue = form1.cameraParameters.deviceClockFrequency_max;

            //foreach (decimal value in allowed_values)
            //{
            //    decimal value_m = value * 1000000m;

            //    if (value_m < pc_orig)
            //    {
            //        return previousValue / 1000000m;
            //    }

            //    previousValue = value_m;
            //}
            //return maxValue / 1000000m;

            //return 99m;
            return Decimal.Parse(form1.formSettings.comboBox_pixelClock.Text);


        }

        public void CloseDevice()
        {
            peak.Library.Close();
            isOpen = false;
        }

        public void adjustParam(string paramName)
        {
            cameraSetter.adjustSet(paramName);

        }

        public void getParams()
        {
            cameraSetter.getParams();
        }
        public void ComTrigerOn_execute()
        {
            comPort.WriteLine("t" + form1.numericUpDown_frameRate.Value.ToString());
        }


        public void ComTrigerS_execute()
        {
            comPort.WriteLine("son");
        }

        public void ComTrigerSoff_execute()
        {
            comPort.WriteLine("soff");
        }

        public void ComTrigerX_execute()
        {
            comPort.WriteLine("xon");
        }

        public void ComTrigerXoff_execute()
        {
            comPort.WriteLine("xoff");
        }

        public void ComTrigerB_execute()
        {
            comPort.WriteLine("bon");
        }

        public void ComTrigerBoff_execute()
        {
            comPort.WriteLine("boff");
        }

        private void cameraAcquisitionWorker_ComTrigerOn(object sender, EventArgs args)
        {
            ComTrigerOn(sender, args);
        }


        private void cameraAcquisitionWorker_MessageBoxTrigger(object sender, string messageTitle, string messageText)
        {
            MessageBoxTrigger(sender, messageTitle, messageText);
        }

        private void cameraAcquisitionWorker_ImageReceived(object sender, System.Drawing.Bitmap image, uint counter)
        {
            ImageReceived(sender, image, counter);
        }

        private void cameraAcquisitionWorker_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            CountersUpdated(sender, frameCounter, errorCounter);
        }


        public bool IsActive()
        {
            return isActive;
        }
        public bool IsOpen()
        {
            return isOpen;
        }

        internal void ComTrigerOff()
        {
            comPort.WriteLine("toff");
        }

        internal void comLedUpdate()
        {
            if (form1.checkBox_LED.Checked)
            {
                comPort.WriteLine("l" + form1.numericUpDown_LED.Value.ToString());
            }
            else
            {
                try
                {
                    comPort.WriteLine("loff");
                }
                catch (Exception ex)
                {
                    Console.WriteLine("arduino is not coonected");
                    Console.WriteLine(ex.ToString());
                }
            }
        }
    }



}

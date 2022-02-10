/*!
 * \file    BackEnd.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.0.0
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Drawing;
using System.Diagnostics;

namespace simple_live_windows_forms
{
    class BackEnd
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




        private AcquisitionWorker acquisitionWorker;
        private Thread acquisitionThread;

        private peak.core.Device device;
        private peak.core.DataStream dataStream;
        private peak.core.NodeMap nodeMapRemoteDevice;
        private peak.core.Buffer buffer;
        private FormWindow windowForm;

        private bool isActive;
        private bool isOpen;

        public BackEnd()
        {
            Console.WriteLine("--- [BackEnd] Init");

            

            try
            {

                // Initialize peak library
                peak.Library.Initialize();
            }
            catch (Exception e)
            {
                Console.WriteLine("--- [BackEnd] Exception: " + e.Message);
            }
        }

        public bool Start()
        {
            Console.WriteLine("--- [BackEnd] Start");

            acquisitionWorker = new AcquisitionWorker();

            acquisitionThread = new Thread(new ThreadStart(acquisitionWorker.Start));
            acquisitionThread.Name = "acquisitionThread";


            acquisitionWorker.ImageReceived += acquisitionWorker_ImageReceived;
            acquisitionWorker.CountersUpdated += acquisitionWorker_CountersUpdated;
            acquisitionWorker.MessageBoxTrigger += acquisitionWorker_MessageBoxTrigger;
            acquisitionWorker.ComTrigerOn += acquisitionWorker_ComTrigerOn;


            if (device != null)
            {
                // Check if any datastreams are available
                var dataStreams = device.DataStreams();

                if (!dataStreams.Any())
                {
                    Console.WriteLine("--- [BackEnd] Error: Device has no DataStream");
                    MessageBoxTrigger(this, "Error", "Device has no DataStream");
                    return false;
                }

                // Open standard data stream
                dataStream = dataStreams[0].OpenDataStream();

                // Get nodemap of remote device for all accesses to the genicam nodemap tree
                nodeMapRemoteDevice = device.RemoteDevice().NodeMaps()[0];

                // To prepare for untriggered continuous image acquisition, load the default user set if available
                // and wait until execution is finished



                nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("GainSelector").SetCurrentEntry("All");
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").SetValue(decimal.ToDouble(windowForm.numericUpDown_gain.Value));

                nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("PixelFormat").SetCurrentEntry("Mono8");
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("BlackLevel").SetValue(windowForm.blackLevel);


                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(windowForm.numericUpDown_w.Minimum - (windowForm.numericUpDown_w.Value % windowForm.numericUpDown_w.Increment)));
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(windowForm.numericUpDown_h.Minimum - (windowForm.numericUpDown_h.Value % windowForm.numericUpDown_h.Increment)));

                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").SetValue(decimal.ToInt64(windowForm.numericUpDown_x.Value));
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").SetValue(decimal.ToInt64(windowForm.numericUpDown_y.Value));
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(windowForm.numericUpDown_w.Value));
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(windowForm.numericUpDown_h.Value));




                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Minimum));
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(Convert.ToDouble(windowForm.label_pixelClock.Text.Replace("Mclock", "")) * 1000000);

                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").SetValue(decimal.ToDouble(windowForm.numericUpDown_exposureTime.Value) * 1000);


                if (windowForm.is_triger)
                {
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Value * 1.15m));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("AcquisitionMode").SetCurrentEntry("Continuous");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerSelector").SetCurrentEntry("ExposureStart");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerMode").SetCurrentEntry("On");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerSource").SetCurrentEntry("Line0");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerActivation").SetCurrentEntry("FallingEdge");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("TriggerDelay").SetValue(0.0);
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("TriggerDivider").SetValue(1);

                }
                else
                {
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Value));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("AcquisitionMode").SetCurrentEntry("Continuous");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerSelector").SetCurrentEntry("ExposureStart");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("TriggerMode").SetCurrentEntry("Off");
                }

                Console.WriteLine("--- [BackEnd]Run DeviceClockFrequency3 " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Value().ToString());
                Console.WriteLine("--- [BackEnd]Run AcquisitionFrameRate " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Value().ToString());
                Console.WriteLine("--- [BackEnd]Run ExposureTime " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Value().ToString());

                // Get the payload size for correct buffer allocation
                UInt32 payloadSize = Convert.ToUInt32(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("PayloadSize").Value());

                // Get the minimum number of buffers that must be announced
                //uint bufferCountMax = dataStream.NumBuffersAnnouncedMinRequired();

                UInt32 bufferCountMax = decimal.ToUInt32(windowForm.numericUpDown_bufferSize.Value);

                //Console.WriteLine("--- [BackEnd] min req buffers " +  bufferCountMax.ToString());


                

                // Allocate and announce image buffers and queue them
                for (var bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
                {
                    buffer = dataStream.AllocAndAnnounceBuffer(payloadSize, IntPtr.Zero);
                    dataStream.QueueBuffer(buffer);
                }

                // Configure worker
                acquisitionWorker.SetDataStream(dataStream);
                acquisitionWorker.SetNodemapRemoteDevice(nodeMapRemoteDevice);
                acquisitionWorker.SetFormWindow(windowForm);
            }







            // Start thread execution
            acquisitionThread.Start();

            isActive = true;

            return true;
        }



        public void Stop()
        {
            Console.WriteLine("--- [BackEnd] Stop");
            isActive = false;
            acquisitionWorker.Stop();

            if (acquisitionThread.IsAlive)
            {
                acquisitionThread.Join();
            }




            if (device != null)
            {
                try
                {
                    var remoteNodeMap = device.RemoteDevice().NodeMaps()[0];
                    remoteNodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").Execute();
                    remoteNodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").WaitUntilDone();
                    remoteNodeMap.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(0);
                }
                catch (Exception e)
                {
                    Console.WriteLine("--- [BackEnd] Exception: " + e.Message);
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
                    acquisitionWorker.SetDataStream(null);

                }
                catch (Exception e)
                {
                    Console.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger(this, "Exception", e.Message);
                }
            }











        }





        public bool OpenDevice()
        {
            Console.WriteLine("--- [BackEnd] Open device");
            try
            {
                // Create instance of the device manager
                var deviceManager = peak.DeviceManager.Instance();

                // Update the device manager
                deviceManager.Update();

                // Return if no device was found
                if (!deviceManager.Devices().Any())
                {
                    Console.WriteLine("--- [BackEnd] Error: No device found");
                    MessageBoxTrigger(this, "Error", "No device found");
                    return false;
                }

                // Open the first openable device in the device manager's device list
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
                        Console.WriteLine("--- [BackEnd] Error: Device could not be openend");
                        MessageBoxTrigger(this, "Error", "Device could not be openend");
                        return false;
                    }
                }

                nodeMapRemoteDevice = device.RemoteDevice().NodeMaps()[0];

                nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("UserSetSelector").SetCurrentEntry("Default");
                nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").Execute();
                nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").WaitUntilDone();


                isOpen = true;
            }
            catch (Exception e)
            {
                Console.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger(this, "Exception", e.Message);
                return false;
            }

            return true;
        }

        public void CloseDevice()
        {
            Console.WriteLine("--- [BackEnd] Close device");
            // If device was opened, try to stop acquisition
            
          

            peak.Library.Close();


            isOpen = false;

        }

        private void acquisitionWorker_ImageReceived(object sender, System.Drawing.Bitmap image, uint counter)
        {
            ImageReceived(sender, image, counter);
        }

        private void acquisitionWorker_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            CountersUpdated(sender, frameCounter, errorCounter);
        }

        private void acquisitionWorker_ComTrigerOn(object sender, EventArgs args)
        {
            ComTrigerOn(sender, args);
        }

        

        private void acquisitionWorker_MessageBoxTrigger(object sender, String messageTitle, String messageText)
        {
            MessageBoxTrigger(sender, messageTitle, messageText);
        }

        public bool IsActive()
        {
            return isActive;
        }
        public bool IsOpen()
        {
            return isOpen;
        }


        public void SetWindowForm(FormWindow wf)
        {
            this.windowForm = wf;
        }

        public void adjustParam(string paramName)
        {
            if (paramName=="Gain")
            {

                nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("GainSelector").SetCurrentEntry("All");
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").SetValue(decimal.ToDouble(windowForm.numericUpDown_gain.Value));

            }
            if (paramName == "ExposureTime")
            {
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").SetValue(decimal.ToDouble(windowForm.numericUpDown_exposureTime.Value) * 1000);
                
            }

            if (paramName == "AcquisitionFrameRate")
            {
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Value));

            }
            if (paramName == "DeviceClockFrequency")
            {
                    
                nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(Convert.ToDouble(windowForm.label_pixelClock.Text.Replace("Mclock","")) * 1000000);
                Console.WriteLine("--- [BackEnd] DeviceClockFrequency1 " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Value().ToString());

            }



        }

        public void getParams()
        {
            windowForm.x_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Minimum());
            windowForm.y_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Minimum());
            windowForm.w_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Minimum());
            windowForm.h_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Minimum());

            Console.WriteLine("--- [BackEnd] x_min " + windowForm.x_min.ToString());
            Console.WriteLine("--- [BackEnd] y_min " + windowForm.y_min.ToString());
            Console.WriteLine("--- [BackEnd] w_min " + windowForm.w_min.ToString());
            Console.WriteLine("--- [BackEnd] h_min " + windowForm.h_min.ToString());

            windowForm.x_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Maximum()); ////////////////////
            windowForm.y_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Maximum()); //////////////////
            windowForm.w_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Maximum());
            windowForm.h_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Maximum());

            Console.WriteLine("--- [BackEnd] x_max " + windowForm.x_max.ToString());
            Console.WriteLine("--- [BackEnd] y_max " + windowForm.y_max.ToString());
            Console.WriteLine("--- [BackEnd] w_max " + windowForm.w_max.ToString());
            Console.WriteLine("--- [BackEnd] h_max " + windowForm.h_max.ToString());

            windowForm.x_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Increment());
            windowForm.y_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Increment());
            windowForm.w_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Increment());
            windowForm.h_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Increment());

            Console.WriteLine("--- [BackEnd] x_inc " + windowForm.x_inc.ToString());
            Console.WriteLine("--- [BackEnd] y_inc  " + windowForm.y_inc.ToString());
            Console.WriteLine("--- [BackEnd] w_inc  " + windowForm.w_inc.ToString());
            Console.WriteLine("--- [BackEnd] h_inc  " + windowForm.h_inc.ToString());


            windowForm.deviceClockFrequency_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Minimum());
            windowForm.deviceClockFrequency_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Maximum());
            windowForm.deviceClockFrequency_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Increment());

            Console.WriteLine("--- [BackEnd] DeviceClockFrequency min " + windowForm.deviceClockFrequency_min.ToString());
            Console.WriteLine("--- [BackEnd] DeviceClockFrequency max " + windowForm.deviceClockFrequency_max.ToString());
            Console.WriteLine("--- [BackEnd] DeviceClockFrequency increment " + windowForm.deviceClockFrequency_inc.ToString());



            nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(decimal.ToDouble(windowForm.deviceClockFrequency_max));
            //Thread.Sleep(500);
            //Console.WriteLine("--- [BackEnd] DeviceClockFrequency " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Value().ToString());



            windowForm.acquisitionFrameRate_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Minimum());
            windowForm.acquisitionFrameRate_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Maximum());
            windowForm.acquisitionFrameRate_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Increment());

            Console.WriteLine("--- [BackEnd] AcquisitionFrameRate min " + windowForm.acquisitionFrameRate_min.ToString());
            Console.WriteLine("--- [BackEnd] AcquisitionFrameRate max " + windowForm.acquisitionFrameRate_max.ToString());
            Console.WriteLine("--- [BackEnd] AcquisitionFrameRate increment " + windowForm.acquisitionFrameRate_inc.ToString());


            windowForm.exposureTime_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Minimum());
            windowForm.exposureTime_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Maximum());
            windowForm.exposureTime_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Increment());

            Console.WriteLine("--- [BackEnd] ExposureTime min " + windowForm.exposureTime_min.ToString());
            Console.WriteLine("--- [BackEnd] ExposureTime max " + windowForm.exposureTime_max.ToString());
            Console.WriteLine("--- [BackEnd] ExposureTime increment " + windowForm.exposureTime_inc.ToString());



            
            windowForm.gain_min = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").Minimum());
            windowForm.gain_max = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").Maximum());
            windowForm.gain_inc = Convert.ToDecimal(nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").Increment());

            Console.WriteLine("--- [BackEnd] Gain min " + windowForm.gain_min.ToString());
            Console.WriteLine("--- [BackEnd] Gain max " + windowForm.gain_max.ToString());
            Console.WriteLine("--- [BackEnd] Gain increment " + windowForm.gain_inc.ToString());


            windowForm.c = (windowForm.acquisitionFrameRate_max * windowForm.h_max) / windowForm.deviceClockFrequency_max;
            Console.WriteLine("--- [BackEnd] for image 400px 29.33 fps - pixelClock= " + getPixelClock(29.33m, 400m));

            // not exist? why? they are in documentation!
            // Console.WriteLine("--- [BackEnd] LUTSelector " + nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("LUTSelector").CurrentEntry().SymbolicValue().ToString());
            // Console.WriteLine("--- [BackEnd] Gamma " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gamma").Value().ToString());
            // Console.WriteLine("--- [BackEnd] LUTEnable " + nodeMapRemoteDevice.FindNode<peak.core.nodes.BooleanNode>("LUTEnable").Value().ToString());
            // Console.WriteLine("--- [BackEnd] ExposureTime increment " + nodeMapRemoteDevice.FindNode<peak.core.nodes.BooleanNode>("ReverseX").Value().ToString());




        }
        public decimal getPixelClock(decimal fps, decimal heght)
        {
            decimal pc_orig = ((fps * heght) / windowForm.c) * 1.3m;

            decimal[] allowed_values = {470m, 237m, 118m, 59m, 30m, 0m};
            decimal maxValue = windowForm.deviceClockFrequency_max;
            decimal previousValue = windowForm.deviceClockFrequency_max;

            foreach (decimal value in allowed_values)
            {
                decimal value_m = value * 1000000m;

                if (value_m < pc_orig) 
                {
                    return previousValue / 1000000m;
                }

                previousValue = value_m;
            }
            return maxValue / 1000000m;




        }

    }


}

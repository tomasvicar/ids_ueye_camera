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
        public delegate void ImageReceivedEventHandler(object sender, Bitmap image);
        public event ImageReceivedEventHandler ImageReceived;

        // Event which is raised if the counters has changed
        public delegate void CountersUpdatedEventHandler(object sender, uint frameCounter, uint errorCounter);
        public event CountersUpdatedEventHandler CountersUpdated;

        public delegate void ComTrigerOnEventHandler(object sender, EventArgs args);
        public event ComTrigerOnEventHandler ComTrigerOn;


        // Event which is raised if an Error or Exception has occurred
        public delegate void MessageBoxTriggerEventHandler(object sender, String messageTitle, String messageText);
        public event MessageBoxTriggerEventHandler MessageBoxTrigger;

        public long x_min;
        public long y_min;
        public long w_min;
        public long h_min;

        public long x_max;
        public long y_max;
        public long w_max;
        public long h_max;


        private AcquisitionWorker acquisitionWorker;
        private Thread acquisitionThread;

        private peak.core.Device device;
        private peak.core.DataStream dataStream;
        private peak.core.NodeMap nodeMapRemoteDevice;
        private peak.core.Buffer buffer;
        private FormWindow windowForm;

        private bool isActive;

        public BackEnd()
        {
            Debug.WriteLine("--- [BackEnd] Init");

            isActive = true;

            try
            {
                // Create acquisition worker thread that waits for new images from the camera
                acquisitionWorker = new AcquisitionWorker();

                acquisitionThread = new Thread(new ThreadStart(acquisitionWorker.Start));
                acquisitionThread.Name = "acquisitionThread";


                acquisitionWorker.ImageReceived += acquisitionWorker_ImageReceived;
                acquisitionWorker.CountersUpdated += acquisitionWorker_CountersUpdated;
                acquisitionWorker.MessageBoxTrigger += acquisitionWorker_MessageBoxTrigger;
                acquisitionWorker.ComTrigerOn += acquisitionWorker_ComTrigerOn;


                // Initialize peak library
                peak.Library.Initialize();
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
            }
        }

        public bool Start()
        {
            Debug.WriteLine("--- [BackEnd] Start");
            if (!OpenDevice())
            {
                return false;
            }

            // Start thread execution
            acquisitionThread.Start();

            return true;
        }



        public void Stop()
        {
            Debug.WriteLine("--- [BackEnd] Stop");
            isActive = false;
            acquisitionWorker.Stop();

            if (acquisitionThread.IsAlive)
            {
                acquisitionThread.Join();
            }

            CloseDevice();

            // Close peak library
            peak.Library.Close();
        }





        public bool OpenDevice()
        {
            Debug.WriteLine("--- [BackEnd] Open device");
            try
            {
                // Create instance of the device manager
                var deviceManager = peak.DeviceManager.Instance();

                // Update the device manager
                deviceManager.Update();

                // Return if no device was found
                if (!deviceManager.Devices().Any())
                {
                    Debug.WriteLine("--- [BackEnd] Error: No device found");
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
                        Debug.WriteLine("--- [BackEnd] Error: Device could not be openend");
                        MessageBoxTrigger(this, "Error", "Device could not be openend");
                        return false;
                    }
                }

                if (device != null)
                {
                    // Check if any datastreams are available
                    var dataStreams = device.DataStreams();

                    if (!dataStreams.Any())
                    {
                        Debug.WriteLine("--- [BackEnd] Error: Device has no DataStream");
                        MessageBoxTrigger(this, "Error", "Device has no DataStream");
                        return false;
                    }

                    // Open standard data stream
                    dataStream = dataStreams[0].OpenDataStream();

                    // Get nodemap of remote device for all accesses to the genicam nodemap tree
                    nodeMapRemoteDevice = device.RemoteDevice().NodeMaps()[0];

                    // To prepare for untriggered continuous image acquisition, load the default user set if available
                    // and wait until execution is finished
                    
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("UserSetSelector").SetCurrentEntry("Default");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").Execute();
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").WaitUntilDone();

                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("GainSelector").SetCurrentEntry("All");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("Gain").SetValue(decimal.ToDouble(windowForm.numericUpDown_gain.Value));

                    nodeMapRemoteDevice.FindNode<peak.core.nodes.EnumerationNode>("PixelFormat").SetCurrentEntry("Mono8");
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("BlackLevel").SetValue(windowForm.blackLevel);


                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(windowForm.numericUpDown_w.Minimum));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(windowForm.numericUpDown_h.Minimum));

                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").SetValue(decimal.ToInt64(windowForm.numericUpDown_x.Value));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").SetValue(decimal.ToInt64(windowForm.numericUpDown_y.Value));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(windowForm.numericUpDown_w.Value));
                    nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(windowForm.numericUpDown_h.Value));


                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Minimum));

                    nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").SetValue(decimal.ToDouble(windowForm.numericUpDown_exposureTime.Value) * 1000);


                    if (windowForm.is_triger)
                    {
                        nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(windowForm.numericUpDown_frameRate.Maximum));
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





                    //Debug.WriteLine("--- [BackEnd] Frame rate " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Value().ToString());
                    //Debug.WriteLine("--- [BackEnd] Exposure time " + nodeMapRemoteDevice.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Value().ToString());

                    //x_min = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Minimum();
                    //y_min = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Minimum();
                    //w_min = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Minimum();
                    //h_min = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Minimum();

                    //Debug.WriteLine("--- [BackEnd] x_min " + x_min.ToString());
                    //Debug.WriteLine("--- [BackEnd] y_min " + y_min.ToString());
                    //Debug.WriteLine("--- [BackEnd] w_min " + w_min.ToString());
                    //Debug.WriteLine("--- [BackEnd] h_min " + h_min.ToString());

                    //x_max = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Maximum();
                    //y_max = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Maximum();
                    //w_max = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Maximum();
                    //h_max = nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Maximum();

                    //Debug.WriteLine("--- [BackEnd] x_max " + x_max.ToString());
                    //Debug.WriteLine("--- [BackEnd] y_max " + y_max.ToString());
                    //Debug.WriteLine("--- [BackEnd] w_max " + w_max.ToString());
                    //Debug.WriteLine("--- [BackEnd] h_max " + h_max.ToString());


                    //Debug.WriteLine("--- [BackEnd] x " + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Value().ToString());
                    //Debug.WriteLine("--- [BackEnd] y " + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Value().ToString());
                    //Debug.WriteLine("--- [BackEnd] w " + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Width").Value().ToString());
                    //Debug.WriteLine("--- [BackEnd] h " + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("Height").Value().ToString());





                    //nodeMapRemoteDevice.FindNode<peak.core.nodes.BooleanNode>("ReverseX").SetValue(windowForm.checkBox_rot180.Checked);
                    //nodeMapRemoteDevice.FindNode<peak.core.nodes.BooleanNode>("ReverseY").SetValue(windowForm.checkBox_rot180.Checked);

                    // Get the payload size for correct buffer allocation
                    UInt32 payloadSize = Convert.ToUInt32(nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("PayloadSize").Value());

                    // Get the minimum number of buffers that must be announced
                    //uint bufferCountMax = dataStream.NumBuffersAnnouncedMinRequired();

                    UInt32 bufferCountMax = decimal.ToUInt32(windowForm.numericUpDown_bufferSize.Value);

                    //Debug.WriteLine("--- [BackEnd] min req buffers " +  bufferCountMax.ToString());

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
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger(this, "Exception", e.Message);
                return false;
            }

            return true;
        }

        public void CloseDevice()
        {
            Debug.WriteLine("--- [BackEnd] Close device");
            // If device was opened, try to stop acquisition
            if (device != null)
            {
                try
                {
                    var remoteNodeMap = device.RemoteDevice().NodeMaps()[0];
                    remoteNodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").Execute();
                    remoteNodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").WaitUntilDone();
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger(this, "Exception", e.Message);
                }
            }

            // If data stream was opened, try to stop it and revoke its image buffers
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
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger(this, "Exception", e.Message);
                }
            }

            try
            {
                // Unlock parameters after acquisition stop
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(0);
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        private void acquisitionWorker_ImageReceived(object sender, System.Drawing.Bitmap image)
        {
            ImageReceived(sender, image);
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
        public void SetWindowForm(FormWindow wf)
        {
            this.windowForm = wf;
        }

        public void adjustParam(string paramName)
        {
            if (paramName=="Gain")
            {
                Debug.WriteLine("changing gain");

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

        }



    }
}

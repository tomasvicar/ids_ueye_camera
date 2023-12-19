/*!
 * \file    BackEnd.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2022-06-01
 * \since   1.1.6
 *
 * \version 1.1.0
 *
 * Copyright (C) 2020 - 2023, IDS Imaging Development Systems GmbH.
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
using peak.core;

namespace simple_live_windows_forms
{
    class BackEnd
    {
        // Event which is raised if a new image was received
        public delegate void ImageReceivedEventHandler1(object sender, Bitmap image);
        public event ImageReceivedEventHandler1 ImageReceived1;

        // Event which is raised if the counters has changed
        public delegate void CounterChangedEventHandler1(object sender, uint frameCounter, uint errorCounter);
        public event CounterChangedEventHandler1 CounterChanged1;

        // Event which is raised if an Error or Exception has occurred
        public delegate void MessageBoxTriggerEventHandler1(object sender, String messageTitle, String messageText);
        public event MessageBoxTriggerEventHandler1 MessageBoxTrigger1;


        public delegate void ImageReceivedEventHandler2(object sender, Bitmap image);
        public event ImageReceivedEventHandler2 ImageReceived2;

        // Event which is raised if the counters has changed
        public delegate void CounterChangedEventHandler2(object sender, uint frameCounter, uint errorCounter);
        public event CounterChangedEventHandler2 CounterChanged2;

        // Event which is raised if an Error or Exception has occurred
        public delegate void MessageBoxTriggerEventHandler2(object sender, String messageTitle, String messageText);
        public event MessageBoxTriggerEventHandler2 MessageBoxTrigger2;


        private Thread acquisitionThread1;
        private Thread acquisitionThread2;
        private AcquisitionWorker acquisitionWorker1;
        private AcquisitionWorker acquisitionWorker2;

        private Device device1;
        private Device device2;
        private DataStream dataStream1;
        private DataStream dataStream2;
        private NodeMap nodeMapRemoteDevice1;
        private NodeMap nodeMapRemoteDevice2;

        private bool isActive;
        private int opendDevicesCount;


        public BackEnd()
        {
            Debug.WriteLine("--- [BackEnd] Init");

            isActive = true;

            try
            {
                // Create acquisition worker thread that waits for new images from the camera
                acquisitionWorker1 = new AcquisitionWorker(0);
                acquisitionThread1 = new Thread(new ThreadStart(acquisitionWorker1.Start));

                acquisitionWorker1.ImageReceived += acquisitionWorker_ImageReceived1;
                acquisitionWorker1.CounterChanged += acquisitionWorker_CounterChanged1;
                acquisitionWorker1.MessageBoxTrigger += acquisitionWorker_MessageBoxTrigger1;


                acquisitionWorker2 = new AcquisitionWorker(1);
                acquisitionThread2 = new Thread(new ThreadStart(acquisitionWorker2.Start));

                acquisitionWorker2.ImageReceived += acquisitionWorker_ImageReceived2;
                acquisitionWorker2.CounterChanged += acquisitionWorker_CounterChanged2;
                acquisitionWorker2.MessageBoxTrigger += acquisitionWorker_MessageBoxTrigger2;



                // Initialize peak library
                peak.Library.Initialize();
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
            }
        }

        public bool start()
        {
            Debug.WriteLine("--- [BackEnd] Start");
            if (!OpenDevice())
            {
                return false;
            }

            // Start thread execution
            acquisitionThread1.Start();
            acquisitionThread2.Start();

            return true;
        }

        public void Stop()
        {
            Debug.WriteLine("--- [BackEnd] Stop");
            isActive = false;
            acquisitionWorker1.Stop();

            if (acquisitionThread1.IsAlive)
            {
                acquisitionThread1.Join();
            }

            acquisitionWorker2.Stop();

            if (acquisitionThread2.IsAlive)
            {
                acquisitionThread2.Join();
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
                    MessageBoxTrigger1(this, "Error", "No device found");
                    return false;
                }

                opendDevicesCount = 0;

                // Open the first openable device in the device manager's device list
                var deviceCount = deviceManager.Devices().Count();

                for (var i = 0; i < deviceCount; ++i)
                {
                    if (deviceManager.Devices()[i].IsOpenable())
                    {
                        if (opendDevicesCount == 0)
                            device1 = deviceManager.Devices()[i].OpenDevice(peak.core.DeviceAccessType.Control);
                        else
                            device2 = deviceManager.Devices()[i].OpenDevice(peak.core.DeviceAccessType.Control);

                        opendDevicesCount++;


                        // Stop after the first opened device
                        // break;
                    }
                    else if (i == (deviceCount - 1))
                    {
                        Debug.WriteLine("--- [BackEnd] Error: Device could not be openend");
                        MessageBoxTrigger1(this, "Error", "Device could not be openend");
                        return false;
                    }
                }



                if ((device1 != null)  )
                {
                    // Check if any datastreams are available
                    var dataStreams1 = device1.DataStreams();
                    var dataStreams2 = device2.DataStreams();

                    if (!dataStreams1.Any() & !dataStreams2.Any())
                    {
                        Debug.WriteLine("--- [BackEnd] Error: Device has no DataStream");
                        MessageBoxTrigger1(this, "Error", "Device has no DataStream");
                        return false;
                    }

                    try
                    {
                        // Open standard data stream
                        dataStream1 = dataStreams1[0].OpenDataStream();
                        dataStream2 = dataStreams2[0].OpenDataStream();
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine("--- [BackEnd] Error: Failed to open DataStream");
                        MessageBoxTrigger1(this, "Error", "Failed to open DataStream\n" + e.Message);
                        return false;
                    }

                    // Get nodemap of remote device for all accesses to the genicam nodemap tree
                    nodeMapRemoteDevice1 = device1.RemoteDevice().NodeMaps()[0];
                    nodeMapRemoteDevice2 = device2.RemoteDevice().NodeMaps()[0];

                    // To prepare for untriggered continuous image acquisition, load the default user set if available
                    // and wait until execution is finished
                    try
                    {
                        nodeMapRemoteDevice1.FindNode<peak.core.nodes.EnumerationNode>("UserSetSelector").SetCurrentEntry("Default");
                        nodeMapRemoteDevice1.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").Execute();
                        nodeMapRemoteDevice1.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").WaitUntilDone();

                        nodeMapRemoteDevice2.FindNode<peak.core.nodes.EnumerationNode>("UserSetSelector").SetCurrentEntry("Default");
                        nodeMapRemoteDevice2.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").Execute();
                        nodeMapRemoteDevice2.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").WaitUntilDone();
                    }
                    catch
                    {
                        // UserSet is not available
                    }

                    // Get the payload size for correct buffer allocation
                    UInt32 payloadSize1 = Convert.ToUInt32(nodeMapRemoteDevice1.FindNode<peak.core.nodes.IntegerNode>("PayloadSize").Value());

                    // Get the minimum number of buffers that must be announced
                    var bufferCountMax1 = dataStream1.NumBuffersAnnouncedMinRequired();

                    // Allocate and announce image buffers and queue them
                    for (var bufferCount = 0; bufferCount < bufferCountMax1; ++bufferCount)
                    {
                        var buffer = dataStream1.AllocAndAnnounceBuffer(payloadSize1, IntPtr.Zero);
                        dataStream1.QueueBuffer(buffer);
                    }

                    // Configure worker
                    acquisitionWorker1.SetDataStream(dataStream1);
                    acquisitionWorker1.SetNodemapRemoteDevice(nodeMapRemoteDevice1);



                    // Get the payload size for correct buffer allocation
                    UInt32 payloadSize2 = Convert.ToUInt32(nodeMapRemoteDevice2.FindNode<peak.core.nodes.IntegerNode>("PayloadSize").Value());

                    // Get the minimum number of buffers that must be announced
                    var bufferCountMax2 = dataStream2.NumBuffersAnnouncedMinRequired();

                    // Allocate and announce image buffers and queue them
                    for (var bufferCount = 0; bufferCount < bufferCountMax2; ++bufferCount)
                    {
                        var buffer = dataStream2.AllocAndAnnounceBuffer(payloadSize2, IntPtr.Zero);
                        dataStream2.QueueBuffer(buffer);
                    }

                    // Configure worker
                    acquisitionWorker2.SetDataStream(dataStream2);
                    acquisitionWorker2.SetNodemapRemoteDevice(nodeMapRemoteDevice2);

                }
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger1(this, "Exception", e.Message);
                return false;
            }

            return true;
        }

        public void CloseDevice()
        {
            Debug.WriteLine("--- [BackEnd] Close device");
            // If device was opened, try to stop acquisition
            if (device1 != null)
            {
                try
                {
                    var remoteNodeMap1 = device1.RemoteDevice().NodeMaps()[0];
                    remoteNodeMap1.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").Execute();
                    remoteNodeMap1.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").WaitUntilDone();
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger1(this, "Exception", e.Message);
                }
            }
            if (device2 != null)
            {
                try
                {
                    var remoteNodeMap2 = device2.RemoteDevice().NodeMaps()[0];
                    remoteNodeMap2.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").Execute();
                    remoteNodeMap2.FindNode<peak.core.nodes.CommandNode>("AcquisitionStop").WaitUntilDone();
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger1(this, "Exception", e.Message);
                }
            }



            // If data stream was opened, try to stop it and revoke its image buffers
            if (dataStream1 != null)
            {
                try
                {
                    dataStream1.KillWait();
                    dataStream1.StopAcquisition(peak.core.AcquisitionStopMode.Default);
                    dataStream1.Flush(peak.core.DataStreamFlushMode.DiscardAll);

                    foreach (var buffer in dataStream1.AnnouncedBuffers())
                    {
                        dataStream1.RevokeBuffer(buffer);
                    }
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger1(this, "Exception", e.Message);
                }
            }

            if (dataStream2 != null)
            {
                try
                {
                    dataStream2.KillWait();
                    dataStream2.StopAcquisition(peak.core.AcquisitionStopMode.Default);
                    dataStream2.Flush(peak.core.DataStreamFlushMode.DiscardAll);

                    foreach (var buffer in dataStream2.AnnouncedBuffers())
                    {
                        dataStream2.RevokeBuffer(buffer);
                    }
                }
                catch (Exception e)
                {
                    Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                    MessageBoxTrigger1(this, "Exception", e.Message);
                }
            }


            try
            {
                // Unlock parameters after acquisition stop
                nodeMapRemoteDevice1.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(0);
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger1(this, "Exception", e.Message);
            }


            try
            {
                // Unlock parameters after acquisition stop
                nodeMapRemoteDevice2.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(0);
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [BackEnd] Exception: " + e.Message);
                MessageBoxTrigger1(this, "Exception", e.Message);
            }

        }

        private void acquisitionWorker_ImageReceived1(object sender, System.Drawing.Bitmap image)
        {
            ImageReceived1(sender, image);
        }

        private void acquisitionWorker_CounterChanged1(object sender, uint frameCounter, uint errorCounter)
        {
            CounterChanged1(sender, frameCounter, errorCounter);
        }

        private void acquisitionWorker_MessageBoxTrigger1(object sender, String messageTitle, String messageText)
        {
            MessageBoxTrigger1(sender, messageTitle, messageText);
        }


        private void acquisitionWorker_ImageReceived2(object sender, System.Drawing.Bitmap image)
        {
            ImageReceived2(sender, image);
        }

        private void acquisitionWorker_CounterChanged2(object sender, uint frameCounter, uint errorCounter)
        {
            CounterChanged2(sender, frameCounter, errorCounter);
        }

        private void acquisitionWorker_MessageBoxTrigger2(object sender, String messageTitle, String messageText)
        {
            MessageBoxTrigger2(sender, messageTitle, messageText);
        }


        public bool IsActive()
        {
            return isActive;
        }
    }
}

/*!
 * \file    AcquisitionWorker.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously and do an image conversion into
 *          a desired pixel format.
 *
 * \version 1.1.0
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
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Drawing;
using System.Drawing.Imaging;
using System.Diagnostics;

namespace simple_live_windows_forms
{
    public class AcquisitionWorker
    {
        // Event which is raised if a new image was received
        public delegate void ImageReceivedEventHandler(object sender, Bitmap image);
        public event ImageReceivedEventHandler ImageReceived;

        // Event which is raised if the counters has changed
        public delegate void CountersUpdatedEventHandler(object sender, uint frameCounter, uint errorCounter);
        public event CountersUpdatedEventHandler CountersUpdated;

        // Event which is raised if an Error or Exception has occurred
        public delegate void MessageBoxTriggerEventHandler(object sender, String messageTitle, String messageText);
        public event MessageBoxTriggerEventHandler MessageBoxTrigger;

        private peak.core.DataStream dataStream;
        private peak.core.NodeMap nodeMapRemoteDevice;
        private peak.core.Buffer buffer;
        private peak.ipl.Image iplImg;
        private Int32 width;
        private Int32 height;
        private Int32 stride;
        private Bitmap image;
        private Bitmap imageCopy;
        private FormWindow formWindow;
        private peak.ipl.ImageTransformer m_imageTransformerIPL;

        private bool running;
        private uint frameCounter;
        private uint errorCounter;
        public ColorPalette colorPalette_grayscale = GetGrayScalePalette();

        public AcquisitionWorker()
        {
            Debug.WriteLine("--- [AcquisitionWorker] Init");
            running = false;
            frameCounter = 0;
            errorCounter = 0;
        }

        public void Start()
        {
            Debug.WriteLine("--- [AcquisitionWorker] Start Acquisition");
            try
            {
                // Lock critical features to prevent them from changing during acquisition
                nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(1);

                // Start acquisition
                dataStream.StartAcquisition();
                nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("AcquisitionStart").Execute();
                nodeMapRemoteDevice.FindNode<peak.core.nodes.CommandNode>("AcquisitionStart").WaitUntilDone();
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [AcquisitionWorker] Exception: " + e.Message);
                MessageBoxTrigger(this, "Exception", e.Message);
            }

            running = true;

            m_imageTransformerIPL = new peak.ipl.ImageTransformer();

            while (running)
            {
                try
                {
                    // Get buffer from device's datastream
                    buffer = dataStream.WaitForFinishedBuffer(5000);

                    // Create IDS peak IPL
                    iplImg = new peak.ipl.Image((peak.ipl.PixelFormatName)buffer.PixelFormat(), buffer.BasePtr(), buffer.Size(), buffer.Width(), buffer.Height());

                    // Debayering and convert IDS peak IPL Image to RGB8 format
                    iplImg = iplImg.ConvertTo(peak.ipl.PixelFormatName.BGR8);
                    //iplImg = iplImg.ConvertTo(peak.ipl.PixelFormatName.Mono8);


                    width = Convert.ToInt32(iplImg.Width());
                    height = Convert.ToInt32(iplImg.Height());
                    stride = Convert.ToInt32(iplImg.PixelFormat().CalculateStorageSizeOfPixels(iplImg.Width()));

                    // Queue buffer so that it can be used again 
                    dataStream.QueueBuffer(buffer);

                    if (formWindow.checkBox_rot180.Checked)
                    {

                        //m_imageTransformerIPL.MirrorUpDownLeftRightInPlace(iplImg);
                        m_imageTransformerIPL.RotateInPlace(iplImg, peak.ipl.ImageTransformer.RotationAngle.Degree180);

                    }

                    //image = new Bitmap(width, height, stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, iplImg.Data());
                    //image.Palette = colorPalette_grayscale;
                    image = new Bitmap(width, height, stride,System.Drawing.Imaging.PixelFormat.Format24bppRgb, iplImg.Data());

                    // Create a deep copy of the Bitmap, so it doesn't use memory of the IDS peak IPL Image.
                    // Warning: Don't use image.Clone(), because it only creates a shallow copy!
                    imageCopy = new Bitmap(image);
                    //imageCopy.Palette = colorPalette_grayscale;

                    // The other images are not needed anymore.
                    image.Dispose();
                    iplImg.Dispose();

                    if (ImageReceived != null)
                    {
                        Debug.WriteLine("--- [AcquisitionWorker] Send image Nr. " + (frameCounter + 1));
                        ImageReceived(this, imageCopy);
                    }
                    else 
                    {
                        Debug.WriteLine("--- [AcquisitionWorker] Imge is null");
                    }

                    frameCounter++;
                }
                catch (Exception e)
                {
                    errorCounter++;
                    Debug.WriteLine("--- [AcquisitionWorker] Exception: " + e.Message);
                    MessageBoxTrigger(this, "Exception", e.Message);
                }

                CountersUpdated(this, frameCounter, errorCounter);

            }
        }

        public void Stop()
        {
            Debug.WriteLine("--- [AcquisitionWorker] Stop Acquisition");
            running = false;


        }

        public void SetFormWindow(FormWindow formWindow)
        {
            this.formWindow = formWindow;
        }


        public void SetDataStream(peak.core.DataStream dataStream)
        {
            Debug.WriteLine("--- [AcquisitionWorker] Set dataStream");
            this.dataStream = dataStream;
        }

        public void SetNodemapRemoteDevice(peak.core.NodeMap nodeMap)
        {
            Debug.WriteLine("--- [AcquisitionWorker] Set nodeMap");
            nodeMapRemoteDevice = nodeMap;
        }



        public static ColorPalette GetGrayScalePalette()
        {
            Bitmap bmp = new Bitmap(1, 1, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);

            ColorPalette monoPalette = bmp.Palette;

            System.Drawing.Color[] entries = monoPalette.Entries;

            for (int i = 0; i < 256; i++)
            {
                entries[i] = System.Drawing.Color.FromArgb(i, i, i);
            }

            return monoPalette;
        }


    }
}

﻿using peak.core;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace VO_soft
{
    class CameraAcquisitionWorker
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


        private peak.core.DataStream dataStream;
        private peak.core.NodeMap nodeMap;
        private peak.core.Buffer buffer;
        private peak.ipl.Image iplImg;
        private Int32 width;
        private Int32 height;
        private Int32 stride;
        private Bitmap image;
        private Bitmap imageCopy;
        private peak.ipl.ImageTransformer m_imageTransformerIPL;

        private bool running;
        public uint frameCounter;
        private uint errorCounter;
        private bool triger_first_time;
        private bool is_triger;
        private int show_subsampling;
        private string filename;
        //private int bits;

        public CameraAcquisitionWorker()
        {
            running = false;
            frameCounter = 0;
            errorCounter = 0;
        }

        public void Start()
        {
            running = false;
            try
            {
                nodeMap.FindNode<peak.core.nodes.IntegerNode>("TLParamsLocked").SetValue(1);
                dataStream.StartAcquisition();
                nodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStart").Execute();
                nodeMap.FindNode<peak.core.nodes.CommandNode>("AcquisitionStart").WaitUntilDone();

                triger_first_time = false;

                if (is_triger)
                {
                    ComTrigerOn(this, EventArgs.Empty);
                    Thread.Sleep(100);
                    triger_first_time = true;
                }

            }
            catch (Exception e)
            {

                MessageBoxTrigger(this, "Exception", e.Message);
            }
            running = true;
            m_imageTransformerIPL = new peak.ipl.ImageTransformer();
            while (running)
            {
                try
                {
                    if (triger_first_time)
                    {
                        buffer = dataStream.WaitForFinishedBuffer(5000);
                        triger_first_time = false;
                    }
                    else
                    {
                        buffer = dataStream.WaitForFinishedBuffer(1200);
                    }


                    iplImg = new peak.ipl.Image((peak.ipl.PixelFormatName)buffer.PixelFormat(), buffer.BasePtr(), buffer.Size(), buffer.Width(), buffer.Height());

                    peak.ipl.ImageWriter.WriteAsBMP(filename + "_" + frameCounter.ToString() + ".bmp", iplImg);
                    //peak.ipl.ImageWriter.WriteAsPNG(filename + "_" + frameCounter.ToString() + ".png", iplImg);
                    //peak.ipl.ImageWriter.WriteAsRAW(filename + "_" + frameCounter.ToString() + ".raw", iplImg);


                    if (frameCounter % show_subsampling == 0)
                    {
                        

                        iplImg = iplImg.ConvertTo(peak.ipl.PixelFormatName.RGB8,peak.ipl.ConversionMode.Fast);

                        width = Convert.ToInt32(iplImg.Width());
                        height = Convert.ToInt32(iplImg.Height());
                        stride = Convert.ToInt32(iplImg.PixelFormat().CalculateStorageSizeOfPixels(iplImg.Width()));


                        //image = new Bitmap(width, height, stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, iplImg.Data());
                        //image.Palette = colorPalette_grayscale;
                        image = new Bitmap(width, height, stride, PixelFormat.Format24bppRgb, iplImg.Data());


                        imageCopy = new Bitmap(image);
                        //imageCopy.Palette = colorPalette_grayscale;


                        image.Dispose();
                        
                        ImageReceived(this, imageCopy, frameCounter);
                    }

                    dataStream.QueueBuffer(buffer);
                    iplImg.Dispose();
                    frameCounter++;
                }
                catch (Exception e)
                {
                    errorCounter++;
                    MessageBoxTrigger(this, "Exception", e.Message);
                }

                CountersUpdated(this, frameCounter, errorCounter);
            }
        }


        public void Stop()
        {
            running = false;
        }

        public void SetAquisitionsettings(bool is_triger, int show_subsampling, string filename)
        {
            this.is_triger = is_triger;
            this.show_subsampling = show_subsampling;
            this.filename = filename;

        }


        public void SetDataStream(DataStream dataStream)
        {
            this.dataStream = dataStream;
        }

        public void SetNodemapRemoteDevice(NodeMap nodeMap)
        {
            this.nodeMap = nodeMap;
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

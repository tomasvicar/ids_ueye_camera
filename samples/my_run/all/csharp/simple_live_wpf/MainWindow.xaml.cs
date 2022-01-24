/*!
 * \file    MainWindow.xaml.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.1.2
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
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Drawing;
using System.Drawing.Imaging;
using System.Diagnostics;
using System.ComponentModel;

namespace simple_live_wpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private String projectName;
        private String version;

        private BackEnd backEnd;
        private bool hasError;

        private MemoryStream memory;

        public MainWindow()
        {
            projectName = "simple_live_wpf";
            version = "v1.2.1";

            Debug.WriteLine("--- [MainWindow] Init");
            try
            {
                InitializeComponent();
                infoTextBlock.Text = projectName + " " + version;

                backEnd = new BackEnd();

                Closing += MainWindow_Closing;

                backEnd.ImageReceived += backEnd_ImageReceived;
                backEnd.CountersUpdated += backEnd_CountersUpdated;
                backEnd.MessageBoxTrigger += backEnd_MessageBoxTrigger;

                if (backEnd.Start())
                {
                    hasError = false;
                }
                else
                {
                    hasError = true;
                }
                memory = new MemoryStream();
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [MainWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        private void backEnd_ImageReceived(object sender, Bitmap bitmap)
        {
            try
            {
                bitmap.Save(memory, System.Drawing.Imaging.ImageFormat.Bmp);
                memory.Position = 0;
                BitmapImage bitmapimage = new BitmapImage();
                bitmapimage.BeginInit();
                bitmapimage.StreamSource = memory;
                bitmapimage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapimage.EndInit();
                bitmapimage.Freeze();

                image.Dispatcher.BeginInvoke(new Action<MainWindow>(delegate { image.Source = bitmapimage; }), new object[] { this });
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [MainWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        private void backEnd_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            try
            {
                image.Dispatcher.BeginInvoke(new Action<MainWindow>
                    (delegate { counterTextBlock.Text = "Acquired: " + frameCounter + ", errors: " + errorCounter; }), new object[] { this });
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [MainWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        private void backEnd_MessageBoxTrigger(object sender, String messageTitle, String messageText)
        {
            MessageBox.Show(messageText, messageTitle);
        }

        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            Debug.WriteLine("--- [MainWindow] Closing");
            backEnd.Stop();
        }

        public bool HasError()
        {
            return hasError;
        }
    }
}

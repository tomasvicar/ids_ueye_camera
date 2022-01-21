/*!
 * \file    Program.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-05
 * \since   1.1.6
 *
 * \brief   This application demonstrates how to use the device manager to open a camera
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
using System.Diagnostics;

namespace open_camera
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                String projectName = "open_camera";
                String version = "v1.1.0";

                Console.WriteLine("IDS peak API " + projectName + " Sample " + version);

                // initialize peak library
                peak.Library.Initialize();

                // create a device manager object
                var deviceManager = peak.DeviceManager.Instance();
                // update the deviceManager
                deviceManager.Update();

                // exit program if no device was found
                if (!deviceManager.Devices().Any())
                {
                    Console.WriteLine("No device found. Exiting program.");
                    Console.ReadKey();
                    // close peak library
                    peak.Library.Close();
                    return;
                }

                // list all available devices
                uint i = 0;
                Console.WriteLine("Devices available: ");
                foreach (var deviceDescriptor in deviceManager.Devices())
                {
                    Console.WriteLine(i + ": " + deviceDescriptor.ModelName() + " ("
                              + deviceDescriptor.ParentInterface().DisplayName() + "; "
                              + deviceDescriptor.ParentInterface().ParentSystem().DisplayName() + " v."
                              + deviceDescriptor.ParentInterface().ParentSystem().Version() + ")");
                    ++i;
                }

                // select a device to open
                int selectedDevice = 0;
                // select a device to open via user input or remove these lines to always open the first available device
                Console.WriteLine("\nSelect device to open: ");
                selectedDevice = Convert.ToInt32(Console.ReadLine());

                // open the selected device
                var device = deviceManager.Devices()[selectedDevice].OpenDevice(peak.core.DeviceAccessType.Control);

                // get the remote device node map
                var nodeMapRemoteDevice = device.RemoteDevice().NodeMaps()[0];

                try
                {
                    // print model name, not knowing if device has the node "DeviceModelName"
                    Console.WriteLine("Model Name: " + nodeMapRemoteDevice.FindNode<peak.core.nodes.StringNode>("DeviceModelName").Value());
                }
                catch (Exception)
                {
                    // if DeviceModelName is not a valid node name, do error handling here...
                    Console.WriteLine("Model Name: (unknown)");
                }

                try
                {
                    // print user ID, not knowing if device has the node "DeviceUserID"
                    Console.WriteLine("Model Name: " + nodeMapRemoteDevice.FindNode<peak.core.nodes.StringNode>("DeviceUserID").Value());
                }
                catch (Exception)
                {
                    // if DeviceUserID is not a valid node name, do error handling here...
                    Console.WriteLine("User ID: (unknown)");
                }

                try
                {
                    // print sensor information, not knowing if device has the node "SensorName"
                    Console.WriteLine("Sensor Name: " + nodeMapRemoteDevice.FindNode<peak.core.nodes.StringNode>("SensorName").Value());
                }
                catch (Exception)
                {
                    // if SensorName is not a valid node name, do error handling here...
                    Console.WriteLine("Sensor Name: (unknown)");
                }

                // print resolution
                Console.WriteLine("Max. resolution (w x h): "
                          + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("WidthMax").Value() + " x "
                          + nodeMapRemoteDevice.FindNode<peak.core.nodes.IntegerNode>("HeightMax").Value());
            }
            catch (Exception e)
            {
                Console.WriteLine("EXCEPTION: " + e.Message);
            }

            // close peak library
            // peak.library.close();
            Console.WriteLine("\nPress any key to exit...");
            Console.ReadKey();
            Environment.ExitCode = 0;
            return;
        }
    }
}

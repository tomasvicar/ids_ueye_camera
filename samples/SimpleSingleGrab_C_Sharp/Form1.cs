//===========================================================================//
//                                                                           //
//  Copyright (C) 2006 - 2017                                                //
//  IDS Imaging GmbH                                                         //
//  Dimbacherstr. 6-8                                                        //
//  D-74182 Obersulm-Willsbach                                               //
//                                                                           //
//  The information in this document is subject to change without            //
//  notice and should not be construed as a commitment by IDS Imaging GmbH.  //
//  IDS Imaging GmbH does not assume any responsibility for any errors       //
//  that may appear in this document.                                        //
//                                                                           //
//  This document, or source code, is provided solely as an example          //
//  of how to utilize IDS software libraries in a sample application.        //
//  IDS Imaging GmbH does not assume any responsibility for the use or       //
//  reliability of any portion of this document or the described software.   //
//                                                                           //
//  General permission to copy or modify, but not for profit, is hereby      //
//  granted,  provided that the above copyright notice is included and       //
//  reference made to the fact that reproduction privileges were granted     //
//  by IDS Imaging GmbH.                                                     //
//                                                                           //
//  IDS cannot assume any responsibility for the use or misuse of any        //
//  portion of this software for other than its intended diagnostic purpose  //
//  in calibrating and testing IDS manufactured cameras and software.        //
//                                                                           //
//===========================================================================//

/// Developer Note: I tried to keep it as simple as possible.
/// Therefore there are no functions asking for the newest driver software or freeing memory beforehand, etc.
/// This program is solely meant to show one of the simplest ways to interact with an IDS Camera through the uEye API.
/// (XS Cameras are not supported)

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Sandkasten_CSharp
{
    public partial class SimpleSingleGrab_C_Sharp : Form
    {
        //To use the .NET interface you have to create a uEye .NET object
        private uEye.Camera Camera = new uEye.Camera();
        IntPtr DisplayHandle = IntPtr.Zero;

        public SimpleSingleGrab_C_Sharp()
        {
            InitializeComponent();
            DisplayHandle = DisplayWindow.Handle;

            //Initialize the camera
            Camera.Init();

            //Allocate an default image memory
            Camera.Memory.Allocate();

            Camera.EventFrame += onFrameEvent;

            //Capture a single image with
            Camera.Acquisition.Freeze();
        }
        


        private void onFrameEvent(object sender, EventArgs e)
        {
            uEye.Camera Camera = sender as uEye.Camera;

            //Display the image on the screen
            Camera.Display.Render( DisplayHandle, uEye.Defines.DisplayRenderMode.FitToWindow);
        }



        //The following are key definitions
        private void Quit_Click(object sender, EventArgs e)
        {
            //Free the allocated image memory and closes the program
            Camera.Exit();
            Close();
        }

        private void Refresh_Click(object sender, EventArgs e)
        {
            //Capture a single frame
            Camera.Acquisition.Freeze();
        }

        private void Live_Click(object sender, EventArgs e)
        {
            //Capture a live image
            Camera.Acquisition.Capture();

            //Currently we are working without Ring-Buffer
            //For the purpose of showing this example that's enough
            //But if you want to do some Image data processing you'll need to adjust the settings
        }

        private void DisplayWindow_Click(object sender, EventArgs e)
        {

        }
    }
}
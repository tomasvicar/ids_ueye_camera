﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using peak.core;

namespace VO_soft
{
    class CameraSetter
    {
        private NodeMap nodeMap;
        Form1 form1;
        public CameraSetter(NodeMap nodeMap, Form1 form1)
        {
            this.nodeMap = nodeMap;
            this.form1 = form1;
        }


        public void SetStart(bool isTrigger)
        {
            nodeMap.FindNode<peak.core.nodes.EnumerationNode>("GainSelector").SetCurrentEntry("All");
            nodeMap.FindNode<peak.core.nodes.FloatNode>("Gain").SetValue(decimal.ToDouble(form1.numericUpDown_gain.Value));

            nodeMap.FindNode<peak.core.nodes.EnumerationNode>("PixelFormat").SetCurrentEntry("Mono8");
            nodeMap.FindNode<peak.core.nodes.FloatNode>("BlackLevel").SetValue(1);


            nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_w.Minimum - (form1.formSettings.numericUpDown_w.Value % form1.formSettings.numericUpDown_w.Increment)));
            nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_h.Minimum - (form1.formSettings.numericUpDown_h.Value % form1.formSettings.numericUpDown_h.Increment)));

            nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetX").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_x.Value));
            nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetY").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_y.Value));
            nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_w.Value));
            nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").SetValue(decimal.ToInt64(form1.formSettings.numericUpDown_h.Value));




            nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(form1.numericUpDown_frameRate.Minimum));
            nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(Convert.ToDouble(form1.label_pixelClock.Text.Replace("Mclock", "")) * 1000000);

            nodeMap.FindNode<peak.core.nodes.FloatNode>("ExposureTime").SetValue(decimal.ToDouble(form1.numericUpDown_exposureTime.Value) * 1000);


            if (isTrigger)
            {
                nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(form1.numericUpDown_frameRate.Value * 1.15m));
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("AcquisitionMode").SetCurrentEntry("Continuous");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerSelector").SetCurrentEntry("ExposureStart");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerMode").SetCurrentEntry("On");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerSource").SetCurrentEntry("Line0");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerActivation").SetCurrentEntry("RisingEdge");
                nodeMap.FindNode<peak.core.nodes.FloatNode>("TriggerDelay").SetValue(0.0);
                nodeMap.FindNode<peak.core.nodes.IntegerNode>("TriggerDivider").SetValue(1);

            }
            else
            {
                nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(form1.numericUpDown_frameRate.Value));
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("AcquisitionMode").SetCurrentEntry("Continuous");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerSelector").SetCurrentEntry("ExposureStart");
                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("TriggerMode").SetCurrentEntry("Off");
            }



        }

        internal void initSet()
        {
            nodeMap.FindNode<peak.core.nodes.EnumerationNode>("UserSetSelector").SetCurrentEntry("Default");
            nodeMap.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").Execute();
            nodeMap.FindNode<peak.core.nodes.CommandNode>("UserSetLoad").WaitUntilDone();
        }

        internal void adjustSet(string paramName)
        {
            if (paramName == "Gain")
            {

                nodeMap.FindNode<peak.core.nodes.EnumerationNode>("GainSelector").SetCurrentEntry("All");
                nodeMap.FindNode<peak.core.nodes.FloatNode>("Gain").SetValue(decimal.ToDouble(form1.numericUpDown_gain.Value));

            }
            if (paramName == "ExposureTime")
            {
                nodeMap.FindNode<peak.core.nodes.FloatNode>("ExposureTime").SetValue(decimal.ToDouble(form1.numericUpDown_exposureTime.Value) * 1000);

            }

            if (paramName == "AcquisitionFrameRate")
            {
                nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").SetValue(decimal.ToDouble(form1.numericUpDown_frameRate.Value));

            }
            if (paramName == "DeviceClockFrequency")
            {

                nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(Convert.ToDouble(form1.label_pixelClock.Text.Replace("Mclock", "")) * 1000000);


            }
        }

        internal void getParams()
        {
            form1.cameraParameters.x_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Minimum());
            form1.cameraParameters.y_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Minimum());
            form1.cameraParameters.w_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").Minimum());
            form1.cameraParameters.h_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").Minimum());



            form1.cameraParameters.x_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").Maximum()); 
            form1.cameraParameters.y_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").Maximum()); 
            form1.cameraParameters.w_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").Maximum());
            form1.cameraParameters.h_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").Maximum());


            form1.cameraParameters.x_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetX").Increment());
            form1.cameraParameters.y_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("OffsetY").Increment());
            form1.cameraParameters.w_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Width").Increment());
            form1.cameraParameters.h_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.IntegerNode>("Height").Increment());



            form1.cameraParameters.deviceClockFrequency_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Minimum());
            form1.cameraParameters.deviceClockFrequency_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Maximum());
            form1.cameraParameters.deviceClockFrequency_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").Increment());



            nodeMap.FindNode<peak.core.nodes.FloatNode>("DeviceClockFrequency").SetValue(decimal.ToDouble(form1.cameraParameters.deviceClockFrequency_max));



            form1.cameraParameters.acquisitionFrameRate_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Minimum());
            form1.cameraParameters.acquisitionFrameRate_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Maximum());
            form1.cameraParameters.acquisitionFrameRate_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("AcquisitionFrameRate").Increment());




            form1.cameraParameters.exposureTime_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Minimum());
            form1.cameraParameters.exposureTime_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Maximum());
            form1.cameraParameters.exposureTime_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("ExposureTime").Increment());


            form1.cameraParameters.gain_min = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("Gain").Minimum());
            form1.cameraParameters.gain_max = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("Gain").Maximum());
            form1.cameraParameters.gain_inc = Convert.ToDecimal(nodeMap.FindNode<peak.core.nodes.FloatNode>("Gain").Increment());



            form1.cameraParameters.c = (form1.cameraParameters.acquisitionFrameRate_max * form1.cameraParameters.h_max) / form1.cameraParameters.deviceClockFrequency_max;
        }
    }
}
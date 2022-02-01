﻿/*!
 * \file    FormWindow.cs
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.2.1
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using Accord.Video.FFMPEG;
using System.Drawing.Drawing2D;
using System.IO;
using System.Globalization;
using System.IO.Ports;

namespace simple_live_windows_forms
{
    public partial class FormWindow : Form
    {
        private PictureBox pictureBox;
        private PictureBoxWithInterpolationMode pictureBoxWithInterpolationMode;

        private BackEnd backEnd;
        private BackgroundWorker backgroundWorker1;
        private bool hasError;

        public bool is_triger = false;
        bool tmp_show = false;

        //public double gamma = 1;
        public double blackLevel = 1;
        private decimal exposureSafeMargin = 0.5m;
        private decimal frameRateHardMax = 36m;

        private decimal x_sumMax = 2456m;
        private decimal y_sumMax = 2054m;
        private decimal w_min = 256m;
        private decimal h_min = 2m;


        private decimal frameRateTmp;
        private Stopwatch sw = new Stopwatch();
        private Image previousImage;

        private VideoFileWriter writer;
        private Bitmap imageCopy;

        public string filename;
        public string path;
        public DateTime time;

        string[] ArrayComPortsNames = null;
        SerialPort comPort = null;


        public FormWindow()
        {

            Debug.WriteLine("--- [FormWindow] Init");
            InitializeComponent();
            LoadMySetting();

            label_comPortStatus_Click(null , EventArgs.Empty);


        }

        private void backEnd_ImageReceived(object sender, Bitmap image)
        {
            try
            {

                if (is_triger)
                {
                    writer.WriteVideoFrame(image);
                }

                if (is_triger)
                {
                    tmp_show = (int.Parse(labelCounter.Text) % numericUpDown_pictureBoxTimeDecimation.Value) == 0;
                }
                else
                {
                    tmp_show = true;
                }


                if (tmp_show)
                {
                    previousImage = pictureBoxWithInterpolationMode.Image;
                    pictureBoxWithInterpolationMode.Image = image;
                }

                //pictureBox.BeginInvoke((MethodInvoker)delegate { previousImage = pictureBox.Image; pictureBox.Image = image; if (previousImage != null) { previousImage.Dispose(); } });


                //imageCopy = new Bitmap(image);
                // imageCopy = image.Clone();
                //writer.WriteVideoFrame(imageCopy);



                if ((int.Parse(labelCounter.Text) % 20) == 0)
                {
                    if (sw.IsRunning)
                    {

                        TimeSpan ts = sw.Elapsed;
                        sw.Restart();


                        Debug.WriteLine("-----FPS----- " + (20.0 / ts.TotalSeconds).ToString());

                        label_fps.BeginInvoke((MethodInvoker)delegate { label_fps.Text = Math.Round(20.0 / ts.TotalSeconds).ToString()  + "fps"; });
                     }


                    else
                    {
                        sw.Start();
                    }

                    //buttonStop.BeginInvoke((MethodInvoker)delegate { buttonStop.PerformClick(); });

                }

                // Thread th = Thread.CurrentThread;
                // Debug.WriteLine("This is " + th.Name);



                // Manage memory usage by disposing the previous image
                if (previousImage != null)
                {
                    previousImage.Dispose();
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception1: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception1", e.Message);
            }
        }
        private void backEnd_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            if (labelCounter.InvokeRequired)
            {
                labelCounter.BeginInvoke((MethodInvoker)delegate { labelCounter.Text = frameCounter.ToString(); });
            }
        }

        private void backEnd_MessageBoxTrigger(object sender, String messageTitle, String messageText)
        {
            MessageBox.Show(messageText, messageTitle);
        }

        private void FormWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            Debug.WriteLine("--- [FormWindow] Closing");
            if (backEnd.IsActive())
            {
                backEnd.Stop();
            }
            if (!(comPort == null))
            {
                if (comPort.IsOpen)
                {
                    comPort.Close();
                }

            }


        }

        public bool HasError()
        {
            return hasError;
        }

        private void InitializeComponent()
        {
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.textBox_dataname = new System.Windows.Forms.TextBox();
            this.buttonStart = new System.Windows.Forms.Button();
            this.buttonStop = new System.Windows.Forms.Button();
            this.label_gain = new System.Windows.Forms.Label();
            this.numericUpDown_gain = new System.Windows.Forms.NumericUpDown();
            this.panel_gain = new System.Windows.Forms.Panel();
            this.label_gainMax = new System.Windows.Forms.Label();
            this.label_gainMin = new System.Windows.Forms.Label();
            this.button_triger = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label_exposureTimeMax = new System.Windows.Forms.Label();
            this.label_exposureTimeMin = new System.Windows.Forms.Label();
            this.checkBox_exposurTimeMax = new System.Windows.Forms.CheckBox();
            this.numericUpDown_exposureTime = new System.Windows.Forms.NumericUpDown();
            this.label_exposureTime = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label_frameRateMax = new System.Windows.Forms.Label();
            this.label_frameRateMin = new System.Windows.Forms.Label();
            this.checkBox_frameRateMax = new System.Windows.Forms.CheckBox();
            this.numericUpDown_frameRate = new System.Windows.Forms.NumericUpDown();
            this.label_frameRate = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label_ySumMax = new System.Windows.Forms.Label();
            this.label_xSumMax = new System.Windows.Forms.Label();
            this.numericUpDown_h = new System.Windows.Forms.NumericUpDown();
            this.numericUpDown_w = new System.Windows.Forms.NumericUpDown();
            this.numericUpDown_y = new System.Windows.Forms.NumericUpDown();
            this.numericUpDown_x = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.label_w = new System.Windows.Forms.Label();
            this.checkBox_rot180 = new System.Windows.Forms.CheckBox();
            this.label_y = new System.Windows.Forms.Label();
            this.label_x = new System.Windows.Forms.Label();
            this.label_position = new System.Windows.Forms.Label();
            this.label_sumMax = new System.Windows.Forms.Label();
            this.labelCounter = new System.Windows.Forms.Label();
            this.numericUpDown_bufferSize = new System.Windows.Forms.NumericUpDown();
            this.label_bufferSize = new System.Windows.Forms.Label();
            this.numericUpDown_pictureBoxTimeDecimation = new System.Windows.Forms.NumericUpDown();
            this.label_subsample = new System.Windows.Forms.Label();
            this.button_stopTriger = new System.Windows.Forms.Button();
            this.label_fps = new System.Windows.Forms.Label();
            this.label_comPortStatus = new System.Windows.Forms.Label();
            this.label_recivedCommand = new System.Windows.Forms.Label();
            this.numericUpDown_LED = new System.Windows.Forms.NumericUpDown();
            this.checkBox_LED = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).BeginInit();
            this.panel_gain.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_exposureTime)).BeginInit();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_frameRate)).BeginInit();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_h)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_w)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_y)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_x)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_bufferSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_pictureBoxTimeDecimation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_LED)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(880, 437);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // textBox_dataname
            // 
            this.textBox_dataname.Location = new System.Drawing.Point(565, 491);
            this.textBox_dataname.Name = "textBox_dataname";
            this.textBox_dataname.Size = new System.Drawing.Size(161, 20);
            this.textBox_dataname.TabIndex = 2;
            this.textBox_dataname.Text = "dataset_name";
            this.textBox_dataname.TextChanged += new System.EventHandler(this.textBox_dataname_TextChanged);
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(461, 439);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 8;
            this.buttonStart.Text = "Start";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(461, 464);
            this.buttonStop.Name = "buttonStop";
            this.buttonStop.Size = new System.Drawing.Size(75, 23);
            this.buttonStop.TabIndex = 9;
            this.buttonStop.Text = "Stop";
            this.buttonStop.UseVisualStyleBackColor = true;
            this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
            // 
            // label_gain
            // 
            this.label_gain.AutoSize = true;
            this.label_gain.Location = new System.Drawing.Point(3, 3);
            this.label_gain.Name = "label_gain";
            this.label_gain.Size = new System.Drawing.Size(29, 13);
            this.label_gain.TabIndex = 5;
            this.label_gain.Text = "Gain";
            // 
            // numericUpDown_gain
            // 
            this.numericUpDown_gain.Location = new System.Drawing.Point(3, 19);
            this.numericUpDown_gain.Name = "numericUpDown_gain";
            this.numericUpDown_gain.Size = new System.Drawing.Size(49, 20);
            this.numericUpDown_gain.TabIndex = 6;
            this.numericUpDown_gain.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_gain.ValueChanged += new System.EventHandler(this.numericUpDown_gain_ValueChanged);
            // 
            // panel_gain
            // 
            this.panel_gain.Controls.Add(this.label_gainMax);
            this.panel_gain.Controls.Add(this.label_gainMin);
            this.panel_gain.Controls.Add(this.numericUpDown_gain);
            this.panel_gain.Controls.Add(this.label_gain);
            this.panel_gain.Location = new System.Drawing.Point(0, 440);
            this.panel_gain.Name = "panel_gain";
            this.panel_gain.Size = new System.Drawing.Size(63, 72);
            this.panel_gain.TabIndex = 6;
            // 
            // label_gainMax
            // 
            this.label_gainMax.AutoSize = true;
            this.label_gainMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_gainMax.Location = new System.Drawing.Point(32, 40);
            this.label_gainMax.Name = "label_gainMax";
            this.label_gainMax.Size = new System.Drawing.Size(25, 12);
            this.label_gainMax.TabIndex = 15;
            this.label_gainMax.Text = "xxxx";
            // 
            // label_gainMin
            // 
            this.label_gainMin.AutoSize = true;
            this.label_gainMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_gainMin.Location = new System.Drawing.Point(1, 41);
            this.label_gainMin.Name = "label_gainMin";
            this.label_gainMin.Size = new System.Drawing.Size(25, 12);
            this.label_gainMin.TabIndex = 14;
            this.label_gainMin.Text = "xxxx";
            // 
            // button_triger
            // 
            this.button_triger.Location = new System.Drawing.Point(564, 439);
            this.button_triger.Name = "button_triger";
            this.button_triger.Size = new System.Drawing.Size(93, 23);
            this.button_triger.TabIndex = 10;
            this.button_triger.Text = "Run triger";
            this.button_triger.UseVisualStyleBackColor = true;
            this.button_triger.Click += new System.EventHandler(this.button_triger_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label_exposureTimeMax);
            this.panel1.Controls.Add(this.label_exposureTimeMin);
            this.panel1.Controls.Add(this.checkBox_exposurTimeMax);
            this.panel1.Controls.Add(this.numericUpDown_exposureTime);
            this.panel1.Controls.Add(this.label_exposureTime);
            this.panel1.Location = new System.Drawing.Point(133, 440);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(120, 72);
            this.panel1.TabIndex = 11;
            // 
            // label_exposureTimeMax
            // 
            this.label_exposureTimeMax.AutoSize = true;
            this.label_exposureTimeMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_exposureTimeMax.Location = new System.Drawing.Point(40, 37);
            this.label_exposureTimeMax.Name = "label_exposureTimeMax";
            this.label_exposureTimeMax.Size = new System.Drawing.Size(25, 12);
            this.label_exposureTimeMax.TabIndex = 17;
            this.label_exposureTimeMax.Text = "xxxx";
            // 
            // label_exposureTimeMin
            // 
            this.label_exposureTimeMin.AutoSize = true;
            this.label_exposureTimeMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_exposureTimeMin.Location = new System.Drawing.Point(2, 37);
            this.label_exposureTimeMin.Name = "label_exposureTimeMin";
            this.label_exposureTimeMin.Size = new System.Drawing.Size(25, 12);
            this.label_exposureTimeMin.TabIndex = 16;
            this.label_exposureTimeMin.Text = "xxxx";
            // 
            // checkBox_exposurTimeMax
            // 
            this.checkBox_exposurTimeMax.AutoSize = true;
            this.checkBox_exposurTimeMax.Checked = true;
            this.checkBox_exposurTimeMax.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_exposurTimeMax.Location = new System.Drawing.Point(6, 52);
            this.checkBox_exposurTimeMax.Name = "checkBox_exposurTimeMax";
            this.checkBox_exposurTimeMax.Size = new System.Drawing.Size(45, 17);
            this.checkBox_exposurTimeMax.TabIndex = 10;
            this.checkBox_exposurTimeMax.Text = "max";
            this.checkBox_exposurTimeMax.UseVisualStyleBackColor = true;
            this.checkBox_exposurTimeMax.CheckedChanged += new System.EventHandler(this.checkBox_exposurTimeMax_CheckedChanged);
            // 
            // numericUpDown_exposureTime
            // 
            this.numericUpDown_exposureTime.Enabled = false;
            this.numericUpDown_exposureTime.Location = new System.Drawing.Point(6, 17);
            this.numericUpDown_exposureTime.Name = "numericUpDown_exposureTime";
            this.numericUpDown_exposureTime.Size = new System.Drawing.Size(50, 20);
            this.numericUpDown_exposureTime.TabIndex = 8;
            this.numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            // 
            // label_exposureTime
            // 
            this.label_exposureTime.AutoSize = true;
            this.label_exposureTime.Location = new System.Drawing.Point(2, 2);
            this.label_exposureTime.Name = "label_exposureTime";
            this.label_exposureTime.Size = new System.Drawing.Size(96, 13);
            this.label_exposureTime.TabIndex = 7;
            this.label_exposureTime.Text = "ExposureTime (ms)";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.label_frameRateMax);
            this.panel2.Controls.Add(this.label_frameRateMin);
            this.panel2.Controls.Add(this.checkBox_frameRateMax);
            this.panel2.Controls.Add(this.numericUpDown_frameRate);
            this.panel2.Controls.Add(this.label_frameRate);
            this.panel2.Location = new System.Drawing.Point(63, 440);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(71, 72);
            this.panel2.TabIndex = 12;
            // 
            // label_frameRateMax
            // 
            this.label_frameRateMax.AutoSize = true;
            this.label_frameRateMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_frameRateMax.Location = new System.Drawing.Point(39, 35);
            this.label_frameRateMax.Name = "label_frameRateMax";
            this.label_frameRateMax.Size = new System.Drawing.Size(25, 12);
            this.label_frameRateMax.TabIndex = 17;
            this.label_frameRateMax.Text = "xxxx";
            // 
            // label_frameRateMin
            // 
            this.label_frameRateMin.AutoSize = true;
            this.label_frameRateMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_frameRateMin.Location = new System.Drawing.Point(0, 35);
            this.label_frameRateMin.Name = "label_frameRateMin";
            this.label_frameRateMin.Size = new System.Drawing.Size(25, 12);
            this.label_frameRateMin.TabIndex = 16;
            this.label_frameRateMin.Text = "xxxx";
            // 
            // checkBox_frameRateMax
            // 
            this.checkBox_frameRateMax.AutoSize = true;
            this.checkBox_frameRateMax.Location = new System.Drawing.Point(3, 50);
            this.checkBox_frameRateMax.Name = "checkBox_frameRateMax";
            this.checkBox_frameRateMax.Size = new System.Drawing.Size(45, 17);
            this.checkBox_frameRateMax.TabIndex = 9;
            this.checkBox_frameRateMax.Text = "max";
            this.checkBox_frameRateMax.UseVisualStyleBackColor = true;
            this.checkBox_frameRateMax.CheckedChanged += new System.EventHandler(this.checkBox_frameRateMax_CheckedChanged);
            // 
            // numericUpDown_frameRate
            // 
            this.numericUpDown_frameRate.Location = new System.Drawing.Point(5, 15);
            this.numericUpDown_frameRate.Name = "numericUpDown_frameRate";
            this.numericUpDown_frameRate.Size = new System.Drawing.Size(59, 20);
            this.numericUpDown_frameRate.TabIndex = 8;
            this.numericUpDown_frameRate.Value = new decimal(new int[] {
            25,
            0,
            0,
            0});
            this.numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);
            // 
            // label_frameRate
            // 
            this.label_frameRate.AutoSize = true;
            this.label_frameRate.Location = new System.Drawing.Point(7, 1);
            this.label_frameRate.Name = "label_frameRate";
            this.label_frameRate.Size = new System.Drawing.Size(57, 13);
            this.label_frameRate.TabIndex = 7;
            this.label_frameRate.Text = "Frame rate";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.label_ySumMax);
            this.panel3.Controls.Add(this.label_xSumMax);
            this.panel3.Controls.Add(this.numericUpDown_h);
            this.panel3.Controls.Add(this.numericUpDown_w);
            this.panel3.Controls.Add(this.numericUpDown_y);
            this.panel3.Controls.Add(this.numericUpDown_x);
            this.panel3.Controls.Add(this.label1);
            this.panel3.Controls.Add(this.label_w);
            this.panel3.Controls.Add(this.checkBox_rot180);
            this.panel3.Controls.Add(this.label_y);
            this.panel3.Controls.Add(this.label_x);
            this.panel3.Controls.Add(this.label_position);
            this.panel3.Controls.Add(this.label_sumMax);
            this.panel3.Location = new System.Drawing.Point(254, 440);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(203, 72);
            this.panel3.TabIndex = 13;
            // 
            // label_ySumMax
            // 
            this.label_ySumMax.AutoSize = true;
            this.label_ySumMax.Location = new System.Drawing.Point(163, 39);
            this.label_ySumMax.Name = "label_ySumMax";
            this.label_ySumMax.Size = new System.Drawing.Size(27, 13);
            this.label_ySumMax.TabIndex = 10;
            this.label_ySumMax.Text = "xxxx";
            // 
            // label_xSumMax
            // 
            this.label_xSumMax.AutoSize = true;
            this.label_xSumMax.Location = new System.Drawing.Point(163, 18);
            this.label_xSumMax.Name = "label_xSumMax";
            this.label_xSumMax.Size = new System.Drawing.Size(27, 13);
            this.label_xSumMax.TabIndex = 9;
            this.label_xSumMax.Text = "xxxx";
            // 
            // numericUpDown_h
            // 
            this.numericUpDown_h.Location = new System.Drawing.Point(104, 36);
            this.numericUpDown_h.Name = "numericUpDown_h";
            this.numericUpDown_h.Size = new System.Drawing.Size(56, 20);
            this.numericUpDown_h.TabIndex = 8;
            this.numericUpDown_h.ValueChanged += new System.EventHandler(this.numericUpDown_h_ValueChanged);
            // 
            // numericUpDown_w
            // 
            this.numericUpDown_w.Location = new System.Drawing.Point(104, 17);
            this.numericUpDown_w.Name = "numericUpDown_w";
            this.numericUpDown_w.Size = new System.Drawing.Size(56, 20);
            this.numericUpDown_w.TabIndex = 7;
            this.numericUpDown_w.ValueChanged += new System.EventHandler(this.numericUpDown_w_ValueChanged);
            // 
            // numericUpDown_y
            // 
            this.numericUpDown_y.Location = new System.Drawing.Point(23, 35);
            this.numericUpDown_y.Name = "numericUpDown_y";
            this.numericUpDown_y.Size = new System.Drawing.Size(56, 20);
            this.numericUpDown_y.TabIndex = 6;
            this.numericUpDown_y.ValueChanged += new System.EventHandler(this.numericUpDown_y_ValueChanged);
            // 
            // numericUpDown_x
            // 
            this.numericUpDown_x.Location = new System.Drawing.Point(23, 16);
            this.numericUpDown_x.Name = "numericUpDown_x";
            this.numericUpDown_x.Size = new System.Drawing.Size(56, 20);
            this.numericUpDown_x.TabIndex = 5;
            this.numericUpDown_x.ValueChanged += new System.EventHandler(this.numericUpDown_x_ValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(87, 39);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(13, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "h";
            // 
            // label_w
            // 
            this.label_w.AutoSize = true;
            this.label_w.Location = new System.Drawing.Point(85, 19);
            this.label_w.Name = "label_w";
            this.label_w.Size = new System.Drawing.Size(15, 13);
            this.label_w.TabIndex = 3;
            this.label_w.Text = "w";
            // 
            // checkBox_rot180
            // 
            this.checkBox_rot180.AutoSize = true;
            this.checkBox_rot180.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.checkBox_rot180.Location = new System.Drawing.Point(62, 55);
            this.checkBox_rot180.Name = "checkBox_rot180";
            this.checkBox_rot180.Size = new System.Drawing.Size(50, 16);
            this.checkBox_rot180.TabIndex = 13;
            this.checkBox_rot180.Text = "rot180";
            this.checkBox_rot180.UseVisualStyleBackColor = true;
            // 
            // label_y
            // 
            this.label_y.AutoSize = true;
            this.label_y.Location = new System.Drawing.Point(3, 37);
            this.label_y.Name = "label_y";
            this.label_y.Size = new System.Drawing.Size(12, 13);
            this.label_y.TabIndex = 2;
            this.label_y.Text = "y";
            // 
            // label_x
            // 
            this.label_x.AutoSize = true;
            this.label_x.Location = new System.Drawing.Point(3, 18);
            this.label_x.Name = "label_x";
            this.label_x.Size = new System.Drawing.Size(12, 13);
            this.label_x.TabIndex = 1;
            this.label_x.Text = "x";
            // 
            // label_position
            // 
            this.label_position.AutoSize = true;
            this.label_position.Location = new System.Drawing.Point(6, 1);
            this.label_position.Name = "label_position";
            this.label_position.Size = new System.Drawing.Size(44, 13);
            this.label_position.TabIndex = 11;
            this.label_position.Text = "Position";
            // 
            // label_sumMax
            // 
            this.label_sumMax.AutoSize = true;
            this.label_sumMax.Location = new System.Drawing.Point(153, 3);
            this.label_sumMax.Name = "label_sumMax";
            this.label_sumMax.Size = new System.Drawing.Size(48, 13);
            this.label_sumMax.TabIndex = 12;
            this.label_sumMax.Text = "sum max";
            // 
            // labelCounter
            // 
            this.labelCounter.AutoSize = true;
            this.labelCounter.Location = new System.Drawing.Point(479, 496);
            this.labelCounter.Name = "labelCounter";
            this.labelCounter.Size = new System.Drawing.Size(13, 13);
            this.labelCounter.TabIndex = 14;
            this.labelCounter.Text = "0";
            // 
            // numericUpDown_bufferSize
            // 
            this.numericUpDown_bufferSize.Location = new System.Drawing.Point(663, 455);
            this.numericUpDown_bufferSize.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDown_bufferSize.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDown_bufferSize.Name = "numericUpDown_bufferSize";
            this.numericUpDown_bufferSize.Size = new System.Drawing.Size(55, 20);
            this.numericUpDown_bufferSize.TabIndex = 15;
            this.numericUpDown_bufferSize.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label_bufferSize
            // 
            this.label_bufferSize.AutoSize = true;
            this.label_bufferSize.Location = new System.Drawing.Point(663, 440);
            this.label_bufferSize.Name = "label_bufferSize";
            this.label_bufferSize.Size = new System.Drawing.Size(56, 13);
            this.label_bufferSize.TabIndex = 16;
            this.label_bufferSize.Text = "Buffer size";
            // 
            // numericUpDown_pictureBoxTimeDecimation
            // 
            this.numericUpDown_pictureBoxTimeDecimation.Location = new System.Drawing.Point(724, 455);
            this.numericUpDown_pictureBoxTimeDecimation.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numericUpDown_pictureBoxTimeDecimation.Name = "numericUpDown_pictureBoxTimeDecimation";
            this.numericUpDown_pictureBoxTimeDecimation.Size = new System.Drawing.Size(29, 20);
            this.numericUpDown_pictureBoxTimeDecimation.TabIndex = 17;
            this.numericUpDown_pictureBoxTimeDecimation.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            // 
            // label_subsample
            // 
            this.label_subsample.AutoSize = true;
            this.label_subsample.Location = new System.Drawing.Point(725, 440);
            this.label_subsample.Name = "label_subsample";
            this.label_subsample.Size = new System.Drawing.Size(33, 13);
            this.label_subsample.TabIndex = 18;
            this.label_subsample.Text = "t sub.";
            // 
            // button_stopTriger
            // 
            this.button_stopTriger.Location = new System.Drawing.Point(564, 462);
            this.button_stopTriger.Name = "button_stopTriger";
            this.button_stopTriger.Size = new System.Drawing.Size(93, 23);
            this.button_stopTriger.TabIndex = 19;
            this.button_stopTriger.Text = "Stop triger";
            this.button_stopTriger.UseVisualStyleBackColor = true;
            this.button_stopTriger.Click += new System.EventHandler(this.button_stopTriger_Click);
            // 
            // label_fps
            // 
            this.label_fps.AutoSize = true;
            this.label_fps.Location = new System.Drawing.Point(518, 495);
            this.label_fps.Name = "label_fps";
            this.label_fps.Size = new System.Drawing.Size(31, 13);
            this.label_fps.TabIndex = 20;
            this.label_fps.Text = "xxfps";
            // 
            // label_comPortStatus
            // 
            this.label_comPortStatus.AutoSize = true;
            this.label_comPortStatus.Location = new System.Drawing.Point(772, 444);
            this.label_comPortStatus.Name = "label_comPortStatus";
            this.label_comPortStatus.Size = new System.Drawing.Size(57, 13);
            this.label_comPortStatus.TabIndex = 22;
            this.label_comPortStatus.Text = "COM state";
            this.label_comPortStatus.Click += new System.EventHandler(this.label_comPortStatus_Click);
            // 
            // label_recivedCommand
            // 
            this.label_recivedCommand.AutoSize = true;
            this.label_recivedCommand.Location = new System.Drawing.Point(772, 464);
            this.label_recivedCommand.Name = "label_recivedCommand";
            this.label_recivedCommand.Size = new System.Drawing.Size(96, 13);
            this.label_recivedCommand.TabIndex = 23;
            this.label_recivedCommand.Text = "Recived command";
            // 
            // numericUpDown_LED
            // 
            this.numericUpDown_LED.Location = new System.Drawing.Point(782, 494);
            this.numericUpDown_LED.Name = "numericUpDown_LED";
            this.numericUpDown_LED.Size = new System.Drawing.Size(47, 20);
            this.numericUpDown_LED.TabIndex = 24;
            this.numericUpDown_LED.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDown_LED.ValueChanged += new System.EventHandler(this.numericUpDown_LED_ValueChanged);
            // 
            // checkBox_LED
            // 
            this.checkBox_LED.AutoSize = true;
            this.checkBox_LED.Location = new System.Drawing.Point(734, 495);
            this.checkBox_LED.Name = "checkBox_LED";
            this.checkBox_LED.Size = new System.Drawing.Size(47, 17);
            this.checkBox_LED.TabIndex = 25;
            this.checkBox_LED.Text = "LED";
            this.checkBox_LED.UseVisualStyleBackColor = true;
            this.checkBox_LED.CheckedChanged += new System.EventHandler(this.checkBox_LED_CheckedChanged);
            // 
            // FormWindow
            // 
            this.ClientSize = new System.Drawing.Size(880, 514);
            this.Controls.Add(this.checkBox_LED);
            this.Controls.Add(this.numericUpDown_LED);
            this.Controls.Add(this.label_recivedCommand);
            this.Controls.Add(this.label_comPortStatus);
            this.Controls.Add(this.label_fps);
            this.Controls.Add(this.button_stopTriger);
            this.Controls.Add(this.label_subsample);
            this.Controls.Add(this.numericUpDown_pictureBoxTimeDecimation);
            this.Controls.Add(this.label_bufferSize);
            this.Controls.Add(this.numericUpDown_bufferSize);
            this.Controls.Add(this.labelCounter);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.button_triger);
            this.Controls.Add(this.buttonStop);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.panel_gain);
            this.Controls.Add(this.textBox_dataname);
            this.Controls.Add(this.pictureBox);
            this.Name = "FormWindow";
            this.Text = "Projectname";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).EndInit();
            this.panel_gain.ResumeLayout(false);
            this.panel_gain.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_exposureTime)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_frameRate)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_h)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_w)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_y)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_x)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_bufferSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_pictureBoxTimeDecimation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_LED)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        public void LoadMySetting()
        {
            numericUpDown_gain.Value = 1.0m;
            numericUpDown_gain.DecimalPlaces = 1;
            numericUpDown_gain.Maximum = 24.0m;
            numericUpDown_gain.Minimum = 1.0m;
            numericUpDown_gain.Increment = 0.1m;

            numericUpDown_frameRate.ValueChanged -= new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);
            numericUpDown_frameRate.DecimalPlaces = 0;
            numericUpDown_frameRate.Maximum = frameRateHardMax;
            numericUpDown_frameRate.Minimum = 1.0m;
            numericUpDown_frameRate.Increment = 1m;
            numericUpDown_frameRate.Value = 36m;
            numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);


            numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            numericUpDown_exposureTime.DecimalPlaces = 1;
            numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin,1);
            numericUpDown_exposureTime.Minimum = 0.1m;
            numericUpDown_exposureTime.Increment = 0.1m;
            numericUpDown_exposureTime.Value = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);
            numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);


            label_gainMin.Text = numericUpDown_gain.Minimum.ToString();
            label_gainMax.Text = numericUpDown_gain.Maximum.ToString();

            label_frameRateMin.Text = numericUpDown_frameRate.Minimum.ToString();
            label_frameRateMax.Text = numericUpDown_frameRate.Maximum.ToString();

            label_exposureTimeMin.Text = numericUpDown_exposureTime.Minimum.ToString();
            label_exposureTimeMax.Text = numericUpDown_exposureTime.Maximum.ToString();



            label_xSumMax.Text = x_sumMax.ToString();
            label_ySumMax.Text = y_sumMax.ToString();

            numericUpDown_x.Minimum = 0m;
            numericUpDown_x.Maximum = x_sumMax;
            numericUpDown_x.Value = 732m;
            //numericUpDown_x.Value = 972m;
            //numericUpDown_x.Value = 0m;
            numericUpDown_x.Increment = 8m;

            numericUpDown_w.Minimum = w_min;
            numericUpDown_w.Maximum = x_sumMax;
            numericUpDown_w.Value = 1100;
            //numericUpDown_w.Value = 512m;
            //numericUpDown_w.Value = x_sumMax;
            numericUpDown_w.Increment = 8m;

            numericUpDown_y.Minimum = 0m;
            numericUpDown_y.Maximum = y_sumMax;
            numericUpDown_y.Value = 627m;
            //numericUpDown_y.Value = 771m;
            //numericUpDown_y.Value = 0m;
            numericUpDown_y.Increment = 8m;

            numericUpDown_h.Minimum = h_min;
            numericUpDown_h.Maximum = y_sumMax;
            numericUpDown_h.Value = 800;
            //numericUpDown_h.Value = 512m;
            //numericUpDown_h.Value = y_sumMax;
            numericUpDown_h.Increment = 8m;


            this.pictureBox.Visible = false;

            this.pictureBoxWithInterpolationMode = new PictureBoxWithInterpolationMode();
            pictureBoxWithInterpolationMode.InterpolationMode = InterpolationMode.NearestNeighbor;
            this.Controls.Add(this.pictureBoxWithInterpolationMode);
            this.pictureBoxWithInterpolationMode.Location = this.pictureBox.Location;
            this.pictureBoxWithInterpolationMode.Name = this.pictureBox.Name + "WithInterpolationMode";
            this.pictureBoxWithInterpolationMode.Size = this.pictureBox.Size;
            this.pictureBoxWithInterpolationMode.SizeMode = this.pictureBox.SizeMode;
            this.pictureBoxWithInterpolationMode.TabIndex = this.pictureBox.TabIndex;
            this.pictureBoxWithInterpolationMode.TabStop = this.pictureBox.TabStop;

            

        }



        private void labelPanel_Paint(object sender, PaintEventArgs e)
        {

        }

        private TextBox textBox_dataname;


        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void textBox_dataname_TextChanged(object sender, EventArgs e)
        {

        }

  

        private Button buttonStart;
        private Button buttonStop;

        private void buttonStart_Click(object sender, EventArgs e)
        {
            is_triger = false;
            myStart();

        }

        private void myStart()
        {
            
            

            numericUpDown_x.Enabled = false;
            numericUpDown_w.Enabled = false;
            numericUpDown_y.Enabled = false;
            numericUpDown_h.Enabled = false;
            //checkBox_rot180.Enabled = false;

            buttonStart.Enabled = false;
            buttonStop.Enabled = true;


            if (is_triger)
            {

                

                path = textBox_dataname.Text;

                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }

                writer = new VideoFileWriter();

                time = DateTime.Now;
                CultureInfo.CurrentCulture = CultureInfo.GetCultureInfo("cs-CZ");

                filename = Path.Combine(path, time.ToString().Replace(".", "_").Replace(":", "_").Replace(" ", "_") + ".avi");

                // http://accord-framework.net/docs/html/T_Accord_Video_FFMPEG_VideoCodec.htm
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.MPEG4); 
                //writer.Open("test.avi", Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.Raw);
                writer.Open(filename, Decimal.ToInt32(numericUpDown_w.Value), Decimal.ToInt32(numericUpDown_h.Value), Decimal.ToInt32(numericUpDown_frameRate.Value), VideoCodec.FFV1);

            }

            try
            {

                
                backEnd = new BackEnd();

                backEnd.SetWindowForm(this);

                FormClosing += FormWindow_FormClosing;

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
            }
            catch (Exception ee)
            {
                Debug.WriteLine("--- [FormWindow] Exception2: " + ee.Message);
                backEnd_MessageBoxTrigger(this, "Exception2", ee.Message);
            }

        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            buttonStop.Enabled = false;

            numericUpDown_x.Enabled = true;
            numericUpDown_w.Enabled = true;
            numericUpDown_y.Enabled = true;
            numericUpDown_h.Enabled = true;
            //checkBox_rot180.Enabled = true;

            backEnd.Stop();
            buttonStart.Enabled = true;

            if (is_triger)
            {
                writer.Close();
            }
        }

        private Label label_gain;
        private Panel panel_gain;
        public NumericUpDown numericUpDown_gain;
        private Button button_triger;
        private Panel panel1;
        public NumericUpDown numericUpDown_exposureTime;
        private Label label_exposureTime;



        private void numericUpDown_exposureTime_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            numericUpDown_frameRate.ValueChanged -= new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);

            numericUpDown_frameRate.Maximum = Decimal.Round(1000 / (numericUpDown_exposureTime.Value + exposureSafeMargin));
            if (numericUpDown_frameRate.Maximum > frameRateHardMax)
            {
                numericUpDown_frameRate.Maximum = frameRateHardMax;
            }
            label_frameRateMax.Text = numericUpDown_frameRate.Maximum.ToString();


            if (checkBox_frameRateMax.Checked)
            {
                
                numericUpDown_frameRate.Value = Decimal.Round(1000 / (numericUpDown_exposureTime.Value + exposureSafeMargin));
                

            }


            if (backEnd.IsActive())
            {

                frameRateTmp = numericUpDown_frameRate.Value;
                numericUpDown_frameRate.Value = numericUpDown_frameRate.Minimum;
                backEnd.adjustParam("AcquisitionFrameRate");
                numericUpDown_frameRate.Value = frameRateTmp;

                numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
                numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);

                backEnd.adjustParam("ExposureTime");
                backEnd.adjustParam("AcquisitionFrameRate");

            }
         }

        private Panel panel2;
        public NumericUpDown numericUpDown_frameRate;
        private Label label_frameRate;
        private CheckBox checkBox_exposurTimeMax;
        private CheckBox checkBox_frameRateMax;

        private void numericUpDown_gain_ValueChanged(object sender, EventArgs e)
        {
            backEnd.adjustParam("Gain");
        }

        private Label label_gainMax;
        private Label label_gainMin;
        private Label label_exposureTimeMax;
        private Label label_exposureTimeMin;
        private Label label_frameRateMax;
        private Label label_frameRateMin;

        private void numericUpDown_frameRate_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            numericUpDown_frameRate.ValueChanged -= new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);

            if (numericUpDown_frameRate.Value > frameRateHardMax)
            {
                numericUpDown_frameRate.Value = frameRateHardMax;
            }

            numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);
            label_exposureTimeMax.Text = numericUpDown_exposureTime.Maximum.ToString();


            if (checkBox_exposurTimeMax.Checked)
            {

                numericUpDown_exposureTime.Value = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);

            }


            if (backEnd.IsActive())
            {
                frameRateTmp = numericUpDown_frameRate.Value;
                numericUpDown_frameRate.Value = numericUpDown_frameRate.Minimum;
                backEnd.adjustParam("AcquisitionFrameRate");
                numericUpDown_frameRate.Value = frameRateTmp;


                numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
                numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);


                backEnd.adjustParam("ExposureTime");
                backEnd.adjustParam("AcquisitionFrameRate");

            }


        }

        private void checkBox_frameRateMax_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_frameRateMax.Checked)
            {
                numericUpDown_frameRate.Enabled = false;

                checkBox_exposurTimeMax.Checked = false;

                numericUpDown_frameRate.Value = numericUpDown_frameRate.Maximum;

            }
            else
            {
                numericUpDown_frameRate.Enabled = true;

            }
        }

        private void checkBox_exposurTimeMax_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_exposurTimeMax.Checked)
            {
                numericUpDown_exposureTime.Enabled = false;

                checkBox_frameRateMax.Checked = false;

                numericUpDown_exposureTime.Value = numericUpDown_exposureTime.Maximum;


            }
            else
            {
                numericUpDown_exposureTime.Enabled = true;

            }
        }

        private Panel panel3;
        private Label label_x;
        private Label label_ySumMax;
        private Label label_xSumMax;
        private Label label1;
        private Label label_w;
        private Label label_y;

        private void numericUpDown_x_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_x.ValueChanged -= new System.EventHandler(this.numericUpDown_x_ValueChanged);
            numericUpDown_x.Value = numericUpDown_x.Value - (numericUpDown_x.Value % 8);
            numericUpDown_x.ValueChanged += new System.EventHandler(this.numericUpDown_x_ValueChanged);
            positionCheck();
        }

        private void numericUpDown_y_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_y.ValueChanged -= new System.EventHandler(this.numericUpDown_y_ValueChanged);
            numericUpDown_y.Value = numericUpDown_y.Value - (numericUpDown_y.Value % 8);
            numericUpDown_y.ValueChanged += new System.EventHandler(this.numericUpDown_y_ValueChanged);
            positionCheck();
        }

        private void numericUpDown_w_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_w.ValueChanged -= new System.EventHandler(this.numericUpDown_w_ValueChanged);
            numericUpDown_w.Value = numericUpDown_w.Value - (numericUpDown_w.Value % 8);
            numericUpDown_w.ValueChanged += new System.EventHandler(this.numericUpDown_w_ValueChanged);
            positionCheck();
        }

        private void numericUpDown_h_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_h.ValueChanged -= new System.EventHandler(this.numericUpDown_h_ValueChanged);
            //numericUpDown_h.Value = numericUpDown_h.Value - (numericUpDown_h.Value % 8);
            numericUpDown_h.ValueChanged += new System.EventHandler(this.numericUpDown_h_ValueChanged);

            positionCheck();
        }

        private void positionCheck()
        {
            buttonStart.Enabled = true;
            label_xSumMax.Enabled = true;
            label_ySumMax.Enabled = true;

            if ((numericUpDown_x.Value + numericUpDown_w.Value) > x_sumMax)
            {
                buttonStart.Enabled = false;
                label_xSumMax.Enabled = false;
            }
            if ((numericUpDown_y.Value + numericUpDown_h.Value) > y_sumMax)
            {
                buttonStart.Enabled = false;
                label_ySumMax.Enabled = false;
            }

        }

        private Label label_position;
        private Label label_sumMax;
        public NumericUpDown numericUpDown_h;
        public NumericUpDown numericUpDown_w;
        public NumericUpDown numericUpDown_y;
        public NumericUpDown numericUpDown_x;
        public CheckBox checkBox_rot180;
        private Label labelCounter;

        private void button_triger_Click(object sender, EventArgs e)
        {
            is_triger = true;
            myStart();
        }
        private Label label_bufferSize;
        public NumericUpDown numericUpDown_bufferSize;
        private NumericUpDown numericUpDown_pictureBoxTimeDecimation;
        private Label label_subsample;
        private Button button_stopTriger;
        private Label label_fps;

        private void button_stopTriger_Click(object sender, EventArgs e)
        {
            buttonStop.PerformClick();
        }



        private Label label_comPortStatus;

        private void label_comPortStatus_Click(object sender, EventArgs e)
        {
            label_comPortStatus.Text = "Connecting";

            label_comPortStatus.ForeColor = System.Drawing.Color.Gray;

            ArrayComPortsNames = SerialPort.GetPortNames();

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



                Debug.WriteLine(port);
                comPort = new SerialPort(port, 9600);

                comPort.Open();

                comPort.WriteTimeout = 500;
                comPort.ReadTimeout = 2000;

                comPort.WriteLine("ok?");



                try
                {
                    string message = comPort.ReadLine();
                    Debug.WriteLine(message);
                    if (message.Trim() == "ok")
                    {
                        label_comPortStatus.Text = port;

                        label_comPortStatus.ForeColor = System.Drawing.Color.Green;

                        comPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);


                    }
                    else
                    {
                        comPort.Close();
                    }

                }
                catch (TimeoutException)
                {
                    Debug.WriteLine("time out");
                    comPort.Close();
                }




            }

            if (label_comPortStatus.Text == "Connecting")
            {
                label_comPortStatus.Text = "NA";

                label_comPortStatus.ForeColor = System.Drawing.Color.Red;
            }

        }

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();

            label_recivedCommand.BeginInvoke((MethodInvoker)delegate { label_recivedCommand.Text = indata; });
        }

        private Label label_recivedCommand;

        public void ComTrigerOn()
        {
           
        }

        public void ComTrigerOff()
        {

        }

        private NumericUpDown numericUpDown_LED;
        private CheckBox checkBox_LED;

        private void checkBox_LED_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_LED.Checked)
            {

                comPort.WriteLine("l" + numericUpDown_LED.Value.ToString());
            }
            else
            {
                comPort.WriteLine("loff");
            }
        }

        private void numericUpDown_LED_ValueChanged(object sender, EventArgs e)
        {
            if (checkBox_LED.Checked)
            {

                comPort.WriteLine("l" + numericUpDown_LED.Value.ToString());
            }
        }
    }
}



public class PictureBoxWithInterpolationMode : PictureBox
{
    public InterpolationMode InterpolationMode { get; set; }

    protected override void OnPaint(PaintEventArgs paintEventArgs)
    {
        paintEventArgs.Graphics.InterpolationMode = InterpolationMode;
        base.OnPaint(paintEventArgs);
    }
}


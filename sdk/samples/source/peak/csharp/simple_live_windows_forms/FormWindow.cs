/*!
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

namespace simple_live_windows_forms
{
    public partial class FormWindow : Form
    {
        private PictureBox pictureBox;
        private BackEnd backEnd;
        private BackgroundWorker backgroundWorker1;
        private bool hasError;



        public FormWindow()
        {

            Debug.WriteLine("--- [FormWindow] Init");
            InitializeComponent();

            try
            {
                backEnd = new BackEnd();

                FormClosing += FormWindow_FormClosing;

                backEnd.ImageReceived += backEnd_ImageReceived;
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
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception", e.Message);
            }
        }

        private void backEnd_ImageReceived(object sender, Bitmap image)
        {
            try
            {
                var previousImage = pictureBox.Image;

                pictureBox.Image = image;

                // Manage memory usage by disposing the previous image
                if (previousImage != null)
                {
                    previousImage.Dispose();
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine("--- [FormWindow] Exception: " + e.Message);
                backEnd_MessageBoxTrigger(this, "Exception", e.Message);
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
            this.trackBar_gain = new System.Windows.Forms.TrackBar();
            this.numericUpDown_gain = new System.Windows.Forms.NumericUpDown();
            this.label_gain = new System.Windows.Forms.Label();
            this.panel_gain = new System.Windows.Forms.Panel();
            this.button_settings = new System.Windows.Forms.Button();
            this.buttonStart = new System.Windows.Forms.Button();
            this.buttonStop = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_gain)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).BeginInit();
            this.panel_gain.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(880, 437);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // textBox_dataname
            // 
            this.textBox_dataname.Location = new System.Drawing.Point(502, 442);
            this.textBox_dataname.Name = "textBox_dataname";
            this.textBox_dataname.Size = new System.Drawing.Size(161, 20);
            this.textBox_dataname.TabIndex = 2;
            this.textBox_dataname.Text = "dataset_name";
            this.textBox_dataname.TextChanged += new System.EventHandler(this.textBox_dataname_TextChanged);
            // 
            // trackBar_gain
            // 
            this.trackBar_gain.Location = new System.Drawing.Point(0, 4);
            this.trackBar_gain.Maximum = 240;
            this.trackBar_gain.Minimum = 10;
            this.trackBar_gain.Name = "trackBar_gain";
            this.trackBar_gain.Size = new System.Drawing.Size(330, 45);
            this.trackBar_gain.TabIndex = 3;
            this.trackBar_gain.TickFrequency = 10;
            this.trackBar_gain.Value = 10;
            this.trackBar_gain.Scroll += new System.EventHandler(this.trackBar_gain_Scroll);
            // 
            // numericUpDown_gain
            // 
            this.numericUpDown_gain.DecimalPlaces = 1;
            this.numericUpDown_gain.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numericUpDown_gain.Location = new System.Drawing.Point(336, 20);
            this.numericUpDown_gain.Maximum = new decimal(new int[] {
            24,
            0,
            0,
            0});
            this.numericUpDown_gain.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDown_gain.Name = "numericUpDown_gain";
            this.numericUpDown_gain.Size = new System.Drawing.Size(63, 20);
            this.numericUpDown_gain.TabIndex = 4;
            this.numericUpDown_gain.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDown_gain.ValueChanged += new System.EventHandler(this.numericUpDown_gain_ValueChanged);
            // 
            // label_gain
            // 
            this.label_gain.AutoSize = true;
            this.label_gain.Location = new System.Drawing.Point(349, 4);
            this.label_gain.Name = "label_gain";
            this.label_gain.Size = new System.Drawing.Size(29, 13);
            this.label_gain.TabIndex = 5;
            this.label_gain.Text = "Gain";
            // 
            // panel_gain
            // 
            this.panel_gain.Controls.Add(this.label_gain);
            this.panel_gain.Controls.Add(this.numericUpDown_gain);
            this.panel_gain.Controls.Add(this.trackBar_gain);
            this.panel_gain.Location = new System.Drawing.Point(0, 439);
            this.panel_gain.Name = "panel_gain";
            this.panel_gain.Size = new System.Drawing.Size(419, 49);
            this.panel_gain.TabIndex = 6;
            // 
            // button_settings
            // 
            this.button_settings.Location = new System.Drawing.Point(425, 439);
            this.button_settings.Name = "button_settings";
            this.button_settings.Size = new System.Drawing.Size(54, 23);
            this.button_settings.TabIndex = 7;
            this.button_settings.Text = "Settings";
            this.button_settings.UseVisualStyleBackColor = true;
            this.button_settings.Click += new System.EventHandler(this.button1_Click);
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(738, 438);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 8;
            this.buttonStart.Text = "Start";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(738, 459);
            this.buttonStop.Name = "buttonStop";
            this.buttonStop.Size = new System.Drawing.Size(75, 23);
            this.buttonStop.TabIndex = 9;
            this.buttonStop.Text = "Stop";
            this.buttonStop.UseVisualStyleBackColor = true;
            this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
            // 
            // FormWindow
            // 
            this.ClientSize = new System.Drawing.Size(880, 484);
            this.Controls.Add(this.buttonStop);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.button_settings);
            this.Controls.Add(this.panel_gain);
            this.Controls.Add(this.textBox_dataname);
            this.Controls.Add(this.pictureBox);
            this.Name = "FormWindow";
            this.Text = "Projectname";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_gain)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).EndInit();
            this.panel_gain.ResumeLayout(false);
            this.panel_gain.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private void labelPanel_Paint(object sender, PaintEventArgs e)
        {

        }

        private TextBox textBox_dataname;
        private TrackBar trackBar_gain;

        private void trackBar_gain_Scroll(object sender, EventArgs e)
        {
            numericUpDown_gain.Value = trackBar_gain.Value/10.0m;
            backEnd.changeGain(trackBar_gain.Value);
        }

        private NumericUpDown numericUpDown_gain;
        private Label label_gain;
        private Panel panel_gain;
        private Button button_settings;

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void textBox_dataname_TextChanged(object sender, EventArgs e)
        {

        }

        private void numericUpDown_gain_ValueChanged(object sender, EventArgs e)
        {
            trackBar_gain.Value = Decimal.ToInt32(numericUpDown_gain.Value * 10);
            backEnd.changeGain(trackBar_gain.Value);

        }

        private Button buttonStart;
        private Button buttonStop;

        private void buttonStart_Click(object sender, EventArgs e)
        {

            try
            {


                backEnd = new BackEnd();

                FormClosing += FormWindow_FormClosing;

                backEnd.ImageReceived += backEnd_ImageReceived;
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
                Debug.WriteLine("--- [FormWindow] Exception: " + ee.Message);
                backEnd_MessageBoxTrigger(this, "Exception", ee.Message);
            }

        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            backEnd.Stop();


        }

    }
}

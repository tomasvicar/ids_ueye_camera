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

        public double gamma = 3;
        public double blackLevel = 1;
        private decimal exposureSafeMargin = 0.5m;


        private decimal exposureTimeOld;
        private decimal frameRateOld;
        private decimal exposureTimeMaxOld;
        private decimal frameRateMaxOld;
        private decimal exposureTimeMaxCurent;
        private decimal frameRateMaxCurent;
        private decimal frameRateTmp;
        private decimal exposureTimeTmp;


        public FormWindow()
        {

            Debug.WriteLine("--- [FormWindow] Init");
            InitializeComponent();
            LoadMySetting();

            try
            {
                backEnd = new BackEnd();

                backEnd.SetWindowForm(this);

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
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).BeginInit();
            this.panel_gain.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_exposureTime)).BeginInit();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_frameRate)).BeginInit();
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
            this.textBox_dataname.Location = new System.Drawing.Point(553, 437);
            this.textBox_dataname.Name = "textBox_dataname";
            this.textBox_dataname.Size = new System.Drawing.Size(161, 20);
            this.textBox_dataname.TabIndex = 2;
            this.textBox_dataname.Text = "dataset_name";
            this.textBox_dataname.TextChanged += new System.EventHandler(this.textBox_dataname_TextChanged);
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(450, 439);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 8;
            this.buttonStart.Text = "Start";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(450, 461);
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
            this.label_gain.Location = new System.Drawing.Point(50, 2);
            this.label_gain.Name = "label_gain";
            this.label_gain.Size = new System.Drawing.Size(29, 13);
            this.label_gain.TabIndex = 5;
            this.label_gain.Text = "Gain";
            // 
            // numericUpDown_gain
            // 
            this.numericUpDown_gain.Location = new System.Drawing.Point(13, 18);
            this.numericUpDown_gain.Name = "numericUpDown_gain";
            this.numericUpDown_gain.Size = new System.Drawing.Size(66, 20);
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
            this.panel_gain.Size = new System.Drawing.Size(103, 57);
            this.panel_gain.TabIndex = 6;
            // 
            // label_gainMax
            // 
            this.label_gainMax.AutoSize = true;
            this.label_gainMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_gainMax.Location = new System.Drawing.Point(57, 41);
            this.label_gainMax.Name = "label_gainMax";
            this.label_gainMax.Size = new System.Drawing.Size(15, 12);
            this.label_gainMax.TabIndex = 15;
            this.label_gainMax.Text = "xx";
            // 
            // label_gainMin
            // 
            this.label_gainMin.AutoSize = true;
            this.label_gainMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_gainMin.Location = new System.Drawing.Point(15, 41);
            this.label_gainMin.Name = "label_gainMin";
            this.label_gainMin.Size = new System.Drawing.Size(15, 12);
            this.label_gainMin.TabIndex = 14;
            this.label_gainMin.Text = "xx";
            // 
            // button_triger
            // 
            this.button_triger.Location = new System.Drawing.Point(576, 460);
            this.button_triger.Name = "button_triger";
            this.button_triger.Size = new System.Drawing.Size(109, 23);
            this.button_triger.TabIndex = 10;
            this.button_triger.Text = "Run triger";
            this.button_triger.UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label_exposureTimeMax);
            this.panel1.Controls.Add(this.label_exposureTimeMin);
            this.panel1.Controls.Add(this.checkBox_exposurTimeMax);
            this.panel1.Controls.Add(this.numericUpDown_exposureTime);
            this.panel1.Controls.Add(this.label_exposureTime);
            this.panel1.Location = new System.Drawing.Point(219, 440);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(113, 57);
            this.panel1.TabIndex = 11;
            // 
            // label_exposureTimeMax
            // 
            this.label_exposureTimeMax.AutoSize = true;
            this.label_exposureTimeMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_exposureTimeMax.Location = new System.Drawing.Point(49, 42);
            this.label_exposureTimeMax.Name = "label_exposureTimeMax";
            this.label_exposureTimeMax.Size = new System.Drawing.Size(15, 12);
            this.label_exposureTimeMax.TabIndex = 17;
            this.label_exposureTimeMax.Text = "xx";
            // 
            // label_exposureTimeMin
            // 
            this.label_exposureTimeMin.AutoSize = true;
            this.label_exposureTimeMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_exposureTimeMin.Location = new System.Drawing.Point(7, 42);
            this.label_exposureTimeMin.Name = "label_exposureTimeMin";
            this.label_exposureTimeMin.Size = new System.Drawing.Size(15, 12);
            this.label_exposureTimeMin.TabIndex = 16;
            this.label_exposureTimeMin.Text = "xx";
            // 
            // checkBox_exposurTimeMax
            // 
            this.checkBox_exposurTimeMax.AutoSize = true;
            this.checkBox_exposurTimeMax.Checked = true;
            this.checkBox_exposurTimeMax.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_exposurTimeMax.Location = new System.Drawing.Point(62, 22);
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
            this.numericUpDown_exposureTime.Location = new System.Drawing.Point(6, 20);
            this.numericUpDown_exposureTime.Name = "numericUpDown_exposureTime";
            this.numericUpDown_exposureTime.Size = new System.Drawing.Size(50, 20);
            this.numericUpDown_exposureTime.TabIndex = 8;
            this.numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            // 
            // label_exposureTime
            // 
            this.label_exposureTime.AutoSize = true;
            this.label_exposureTime.Location = new System.Drawing.Point(2, 5);
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
            this.panel2.Location = new System.Drawing.Point(104, 440);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(113, 57);
            this.panel2.TabIndex = 12;
            // 
            // label_frameRateMax
            // 
            this.label_frameRateMax.AutoSize = true;
            this.label_frameRateMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_frameRateMax.Location = new System.Drawing.Point(48, 42);
            this.label_frameRateMax.Name = "label_frameRateMax";
            this.label_frameRateMax.Size = new System.Drawing.Size(15, 12);
            this.label_frameRateMax.TabIndex = 17;
            this.label_frameRateMax.Text = "xx";
            // 
            // label_frameRateMin
            // 
            this.label_frameRateMin.AutoSize = true;
            this.label_frameRateMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label_frameRateMin.Location = new System.Drawing.Point(6, 42);
            this.label_frameRateMin.Name = "label_frameRateMin";
            this.label_frameRateMin.Size = new System.Drawing.Size(15, 12);
            this.label_frameRateMin.TabIndex = 16;
            this.label_frameRateMin.Text = "xx";
            // 
            // checkBox_frameRateMax
            // 
            this.checkBox_frameRateMax.AutoSize = true;
            this.checkBox_frameRateMax.Location = new System.Drawing.Point(65, 22);
            this.checkBox_frameRateMax.Name = "checkBox_frameRateMax";
            this.checkBox_frameRateMax.Size = new System.Drawing.Size(45, 17);
            this.checkBox_frameRateMax.TabIndex = 9;
            this.checkBox_frameRateMax.Text = "max";
            this.checkBox_frameRateMax.UseVisualStyleBackColor = true;
            this.checkBox_frameRateMax.CheckedChanged += new System.EventHandler(this.checkBox_frameRateMax_CheckedChanged);
            // 
            // numericUpDown_frameRate
            // 
            this.numericUpDown_frameRate.Location = new System.Drawing.Point(5, 20);
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
            this.label_frameRate.Location = new System.Drawing.Point(7, 5);
            this.label_frameRate.Name = "label_frameRate";
            this.label_frameRate.Size = new System.Drawing.Size(57, 13);
            this.label_frameRate.TabIndex = 7;
            this.label_frameRate.Text = "Frame rate";
            // 
            // FormWindow
            // 
            this.ClientSize = new System.Drawing.Size(880, 503);
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
            numericUpDown_frameRate.Value = 25m;
            numericUpDown_frameRate.DecimalPlaces = 0;
            numericUpDown_frameRate.Maximum = 100m;
            numericUpDown_frameRate.Minimum = 1.0m;
            numericUpDown_frameRate.Increment = 1m;
            numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);


            numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            numericUpDown_exposureTime.Value = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin,1);
            numericUpDown_exposureTime.DecimalPlaces = 1;
            numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin,1);
            numericUpDown_exposureTime.Minimum = 0.1m;
            numericUpDown_exposureTime.Increment = 0.1m;
            numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);


            label_gainMin.Text = numericUpDown_gain.Minimum.ToString();
            label_gainMax.Text = numericUpDown_gain.Maximum.ToString();

            label_frameRateMin.Text = numericUpDown_frameRate.Minimum.ToString();
            label_frameRateMax.Text = numericUpDown_frameRate.Maximum.ToString();

            label_exposureTimeMin.Text = numericUpDown_exposureTime.Minimum.ToString();
            label_exposureTimeMax.Text = numericUpDown_exposureTime.Maximum.ToString();

            exposureTimeOld = numericUpDown_exposureTime.Value;
            frameRateOld = numericUpDown_frameRate.Value;

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

            try
            {


                backEnd = new BackEnd();

                backEnd.SetWindowForm(this);

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
            numericUpDown_frameRate.Maximum = Decimal.Round(1000 / (numericUpDown_exposureTime.Value + exposureSafeMargin));

            label_frameRateMax.Text = numericUpDown_frameRate.Maximum.ToString();


            if (checkBox_frameRateMax.Checked)
            {
                
                numericUpDown_frameRate.Value = Decimal.Round(1000 / (numericUpDown_exposureTime.Value + exposureSafeMargin));
                

            }
            numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);

            backEnd.adjustParam("ExposureTime");
            backEnd.adjustParam("AcquisitionFrameRate");


            exposureTimeOld = numericUpDown_exposureTime.Value;
            frameRateOld = numericUpDown_frameRate.Value;
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
            numericUpDown_frameRate.ValueChanged -= new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);

            numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);
            label_exposureTimeMax.Text = numericUpDown_exposureTime.Maximum.ToString();

            if (checkBox_exposurTimeMax.Checked)
            {

                numericUpDown_exposureTime.Value = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);

            }

            numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);



            exposureTimeMaxOld = Decimal.Round(1000 / frameRateOld - exposureSafeMargin, 1);
            frameRateMaxOld = Decimal.Round(1000 / (exposureTimeOld + exposureSafeMargin));
            exposureTimeMaxCurent = Decimal.Round(1000 / numericUpDown_frameRate.Value - exposureSafeMargin, 1);
            frameRateMaxCurent = Decimal.Round(1000 / (numericUpDown_exposureTime.Value + exposureSafeMargin));


            if (frameRateMaxCurent > frameRateMaxOld)
            {
                frameRateTmp = numericUpDown_frameRate.Value;
                numericUpDown_frameRate.Value = frameRateMaxCurent;
                backEnd.adjustParam("AcquisitionFrameRate");
                numericUpDown_frameRate.Value = frameRateTmp
            }


            backEnd.adjustParam("ExposureTime");
            backEnd.adjustParam("AcquisitionFrameRate");

            exposureTimeOld = numericUpDown_exposureTime.Value;
            frameRateOld = numericUpDown_frameRate.Value;

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
    }
}

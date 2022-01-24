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
        private String projectName;
        private String version;
        private PictureBox pictureBox;
        private BackEnd backEnd;
        private BackgroundWorker backgroundWorker1;
        private Panel labelPanel;
        private Label counterLabel;
        private Label infoLabel;
        private bool hasError;

        public FormWindow()
        {
            projectName = "simple_live_windows_forms";
            version = "v1.2.0";

            Debug.WriteLine("--- [FormWindow] Init");
            try
            {
                InitializeComponent();
                this.infoLabel.Text = projectName + " " + version;
                this.Text = projectName;

                backEnd = new BackEnd();

                FormClosing += FormWindow_FormClosing;

                backEnd.ImageReceived += backEnd_ImageReceived;
                backEnd.CountersUpdated += backEnd_CountersUpdated;
                backEnd.MessageBoxTrigger += backEnd_MessageBoxTrigger;

                if (backEnd.start())
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

        private void backEnd_CountersUpdated(object sender, uint frameCounter, uint errorCounter)
        {
            if (counterLabel.InvokeRequired)
            {
                counterLabel.BeginInvoke((MethodInvoker)delegate { counterLabel.Text = "Acquired: " + frameCounter + ", errors: " + errorCounter; });
            }
        }

        private void backEnd_MessageBoxTrigger(object sender, String messageTitle, String messageText)
        {
            MessageBox.Show(messageText, messageTitle);
        }

        private void FormWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            Debug.WriteLine("--- [FormWindow] Closing");
            backEnd.Stop();
        }

        public bool HasError()
        {
            return hasError;
        }

        private void InitializeComponent()
        {
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.labelPanel = new System.Windows.Forms.Panel();
            this.infoLabel = new System.Windows.Forms.Label();
            this.counterLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.labelPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(700, 437);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // labelPanel
            // 
            this.labelPanel.Controls.Add(this.infoLabel);
            this.labelPanel.Controls.Add(this.counterLabel);
            this.labelPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.labelPanel.Location = new System.Drawing.Point(0, 435);
            this.labelPanel.Name = "labelPanel";
            this.labelPanel.Size = new System.Drawing.Size(700, 15);
            this.labelPanel.TabIndex = 2;
            // 
            // infoLabel
            // 
            this.infoLabel.AutoSize = true;
            this.infoLabel.Dock = System.Windows.Forms.DockStyle.Right;
            this.infoLabel.Location = new System.Drawing.Point(596, 0);
            this.infoLabel.Name = "infoLabel";
            this.infoLabel.Size = new System.Drawing.Size(104, 13);
            this.infoLabel.TabIndex = 2;
            this.infoLabel.Text = "Projectname Version";
            // 
            // counterLabel
            // 
            this.counterLabel.AutoSize = true;
            this.counterLabel.Dock = System.Windows.Forms.DockStyle.Left;
            this.counterLabel.Location = new System.Drawing.Point(0, 0);
            this.counterLabel.Name = "counterLabel";
            this.counterLabel.Size = new System.Drawing.Size(69, 13);
            this.counterLabel.TabIndex = 1;
            this.counterLabel.Text = "counterLabel";
            // 
            // FormWindow
            // 
            this.ClientSize = new System.Drawing.Size(700, 450);
            this.Controls.Add(this.labelPanel);
            this.Controls.Add(this.pictureBox);
            this.Name = "FormWindow";
            this.Text = "Projectname";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.labelPanel.ResumeLayout(false);
            this.labelPanel.PerformLayout();
            this.ResumeLayout(false);

        }
    }
}

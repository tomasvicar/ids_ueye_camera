﻿
namespace simple_secondscreen
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button_open = new System.Windows.Forms.Button();
            this.button_close = new System.Windows.Forms.Button();
            this.button_flicker = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.button_create_video = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // button_open
            // 
            this.button_open.Location = new System.Drawing.Point(105, 69);
            this.button_open.Name = "button_open";
            this.button_open.Size = new System.Drawing.Size(158, 109);
            this.button_open.TabIndex = 1;
            this.button_open.Text = "open";
            this.button_open.UseVisualStyleBackColor = true;
            this.button_open.Click += new System.EventHandler(this.button_open_Click);
            // 
            // button_close
            // 
            this.button_close.Location = new System.Drawing.Point(410, 69);
            this.button_close.Name = "button_close";
            this.button_close.Size = new System.Drawing.Size(147, 98);
            this.button_close.TabIndex = 2;
            this.button_close.Text = "close";
            this.button_close.UseVisualStyleBackColor = true;
            this.button_close.Click += new System.EventHandler(this.button_close_Click);
            // 
            // button_flicker
            // 
            this.button_flicker.Location = new System.Drawing.Point(290, 225);
            this.button_flicker.Name = "button_flicker";
            this.button_flicker.Size = new System.Drawing.Size(158, 109);
            this.button_flicker.TabIndex = 3;
            this.button_flicker.Text = "flickering";
            this.button_flicker.UseVisualStyleBackColor = true;
            this.button_flicker.Click += new System.EventHandler(this.button1_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(581, 268);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "video";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(620, 93);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(28, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "freq:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(655, 93);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "label2";
            // 
            // button_create_video
            // 
            this.button_create_video.Location = new System.Drawing.Point(556, 334);
            this.button_create_video.Name = "button_create_video";
            this.button_create_video.Size = new System.Drawing.Size(100, 41);
            this.button_create_video.TabIndex = 7;
            this.button_create_video.Text = "create video";
            this.button_create_video.UseVisualStyleBackColor = true;
            this.button_create_video.Click += new System.EventHandler(this.button_create_video_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.button_create_video);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.button_flicker);
            this.Controls.Add(this.button_close);
            this.Controls.Add(this.button_open);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_open;
        private System.Windows.Forms.Button button_close;
        private System.Windows.Forms.Button button_flicker;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button_create_video;
    }
}


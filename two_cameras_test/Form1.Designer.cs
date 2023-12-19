namespace simple_aquisition
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBox_1 = new System.Windows.Forms.PictureBox();
            this.button_play = new System.Windows.Forms.Button();
            this.button_rec = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.counterLabel1 = new System.Windows.Forms.Label();
            this.pictureBox_2 = new System.Windows.Forms.PictureBox();
            this.counterLabel2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_2)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox_1
            // 
            this.pictureBox_1.Location = new System.Drawing.Point(12, 12);
            this.pictureBox_1.Name = "pictureBox_1";
            this.pictureBox_1.Size = new System.Drawing.Size(472, 321);
            this.pictureBox_1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox_1.TabIndex = 0;
            this.pictureBox_1.TabStop = false;
            // 
            // button_play
            // 
            this.button_play.Location = new System.Drawing.Point(165, 532);
            this.button_play.Name = "button_play";
            this.button_play.Size = new System.Drawing.Size(75, 23);
            this.button_play.TabIndex = 1;
            this.button_play.Text = "play";
            this.button_play.UseVisualStyleBackColor = true;
            this.button_play.Click += new System.EventHandler(this.button_play_Click);
            // 
            // button_rec
            // 
            this.button_rec.Location = new System.Drawing.Point(480, 532);
            this.button_rec.Name = "button_rec";
            this.button_rec.Size = new System.Drawing.Size(75, 23);
            this.button_rec.TabIndex = 2;
            this.button_rec.Text = "rec.";
            this.button_rec.UseVisualStyleBackColor = true;
            this.button_rec.Click += new System.EventHandler(this.button_rec_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(561, 533);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(163, 23);
            this.textBox1.TabIndex = 3;
            this.textBox1.Text = "filename";
            // 
            // counterLabel1
            // 
            this.counterLabel1.AutoSize = true;
            this.counterLabel1.Location = new System.Drawing.Point(296, 478);
            this.counterLabel1.Name = "counterLabel1";
            this.counterLabel1.Size = new System.Drawing.Size(13, 15);
            this.counterLabel1.TabIndex = 4;
            this.counterLabel1.Text = "0";
            // 
            // pictureBox_2
            // 
            this.pictureBox_2.Location = new System.Drawing.Point(490, 12);
            this.pictureBox_2.Name = "pictureBox_2";
            this.pictureBox_2.Size = new System.Drawing.Size(471, 321);
            this.pictureBox_2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox_2.TabIndex = 5;
            this.pictureBox_2.TabStop = false;
            // 
            // counterLabel2
            // 
            this.counterLabel2.AutoSize = true;
            this.counterLabel2.Location = new System.Drawing.Point(726, 478);
            this.counterLabel2.Name = "counterLabel2";
            this.counterLabel2.Size = new System.Drawing.Size(13, 15);
            this.counterLabel2.TabIndex = 6;
            this.counterLabel2.Text = "0";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(973, 564);
            this.Controls.Add(this.counterLabel2);
            this.Controls.Add(this.pictureBox_2);
            this.Controls.Add(this.counterLabel1);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button_rec);
            this.Controls.Add(this.button_play);
            this.Controls.Add(this.pictureBox_1);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private PictureBox pictureBox_1;
        private Button button_play;
        private Button button_rec;
        private TextBox textBox1;
        private Label counterLabel1;
        private PictureBox pictureBox_2;
        private Label counterLabel2;
    }
}

namespace VO_soft
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.pictureBoxWithInterpolationMode1 = new VO_soft.PictureBoxWithInterpolationMode();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.actionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadSettingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSettingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.numericUpDown_gain = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDown_frameRate = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.numericUpDown_exposureTime = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.label_gainMin = new System.Windows.Forms.Label();
            this.label_gainMax = new System.Windows.Forms.Label();
            this.label_frameRateMin = new System.Windows.Forms.Label();
            this.label_frameRateMax = new System.Windows.Forms.Label();
            this.label_exposureTimeMin = new System.Windows.Forms.Label();
            this.label_exposureTimeMax = new System.Windows.Forms.Label();
            this.numericUpDown_IR = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.numericUpDown_red = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.button_pluxStart = new System.Windows.Forms.Button();
            this.button_pluxStop = new System.Windows.Forms.Button();
            this.label_pluxState = new System.Windows.Forms.Label();
            this.checkBox_LED = new System.Windows.Forms.CheckBox();
            this.numericUpDown_LED = new System.Windows.Forms.NumericUpDown();
            this.label_comPortStatus = new System.Windows.Forms.Label();
            this.numericUpDown_pictureBoxTimeDecimation = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.numericUpDown_bufferSize = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.buttonStop = new System.Windows.Forms.Button();
            this.buttonStart = new System.Windows.Forms.Button();
            this.button_stopTriger = new System.Windows.Forms.Button();
            this.button_triger = new System.Windows.Forms.Button();
            this.textBox_dataname = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.numericUpDown_subsampling = new System.Windows.Forms.NumericUpDown();
            this.checkBox_exposurTimeMax = new System.Windows.Forms.CheckBox();
            this.labelCounter = new System.Windows.Forms.Label();
            this.label_fps = new System.Windows.Forms.Label();
            this.label_pixelClock = new System.Windows.Forms.Label();
            this.label_error = new System.Windows.Forms.Label();
            this.panel_plux = new System.Windows.Forms.Panel();
            this.advancedSettingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxWithInterpolationMode1)).BeginInit();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_frameRate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_exposureTime)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_IR)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_red)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_LED)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_pictureBoxTimeDecimation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_bufferSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_subsampling)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxWithInterpolationMode1
            // 
            this.pictureBoxWithInterpolationMode1.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.Default;
            this.pictureBoxWithInterpolationMode1.Location = new System.Drawing.Point(0, 27);
            this.pictureBoxWithInterpolationMode1.Name = "pictureBoxWithInterpolationMode1";
            this.pictureBoxWithInterpolationMode1.Size = new System.Drawing.Size(733, 492);
            this.pictureBoxWithInterpolationMode1.TabIndex = 0;
            this.pictureBoxWithInterpolationMode1.TabStop = false;
            // 
            // statusStrip
            // 
            this.statusStrip.Location = new System.Drawing.Point(0, 609);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(1370, 22);
            this.statusStrip.TabIndex = 1;
            this.statusStrip.Text = "statusStrip1";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.settingsToolStripMenuItem,
            this.actionsToolStripMenuItem,
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1370, 24);
            this.menuStrip1.TabIndex = 2;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.advancedSettingsToolStripMenuItem});
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.settingsToolStripMenuItem.Text = "Settings";
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.settingsToolStripMenuItem_Click);
            // 
            // actionsToolStripMenuItem
            // 
            this.actionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadSettingsToolStripMenuItem,
            this.saveSettingsToolStripMenuItem});
            this.actionsToolStripMenuItem.Name = "actionsToolStripMenuItem";
            this.actionsToolStripMenuItem.Size = new System.Drawing.Size(59, 20);
            this.actionsToolStripMenuItem.Text = "Actions";
            // 
            // loadSettingsToolStripMenuItem
            // 
            this.loadSettingsToolStripMenuItem.Name = "loadSettingsToolStripMenuItem";
            this.loadSettingsToolStripMenuItem.Size = new System.Drawing.Size(141, 22);
            this.loadSettingsToolStripMenuItem.Text = "load settings";
            this.loadSettingsToolStripMenuItem.Click += new System.EventHandler(this.loadSettingsToolStripMenuItem_Click);
            // 
            // saveSettingsToolStripMenuItem
            // 
            this.saveSettingsToolStripMenuItem.Name = "saveSettingsToolStripMenuItem";
            this.saveSettingsToolStripMenuItem.Size = new System.Drawing.Size(141, 22);
            this.saveSettingsToolStripMenuItem.Text = "save settings";
            this.saveSettingsToolStripMenuItem.Click += new System.EventHandler(this.saveSettingsToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(46, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // numericUpDown_gain
            // 
            this.numericUpDown_gain.Location = new System.Drawing.Point(12, 549);
            this.numericUpDown_gain.Name = "numericUpDown_gain";
            this.numericUpDown_gain.Size = new System.Drawing.Size(54, 20);
            this.numericUpDown_gain.TabIndex = 3;
            this.numericUpDown_gain.ValueChanged += new System.EventHandler(this.numericUpDown_gain_ValueChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 533);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Gain";
            // 
            // numericUpDown_frameRate
            // 
            this.numericUpDown_frameRate.Location = new System.Drawing.Point(76, 549);
            this.numericUpDown_frameRate.Name = "numericUpDown_frameRate";
            this.numericUpDown_frameRate.Size = new System.Drawing.Size(54, 20);
            this.numericUpDown_frameRate.TabIndex = 5;
            this.numericUpDown_frameRate.ValueChanged += new System.EventHandler(this.numericUpDown_frameRate_ValueChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(87, 533);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(27, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "FPS";
            // 
            // numericUpDown_exposureTime
            // 
            this.numericUpDown_exposureTime.Location = new System.Drawing.Point(136, 549);
            this.numericUpDown_exposureTime.Name = "numericUpDown_exposureTime";
            this.numericUpDown_exposureTime.Size = new System.Drawing.Size(54, 20);
            this.numericUpDown_exposureTime.TabIndex = 7;
            this.numericUpDown_exposureTime.ValueChanged += new System.EventHandler(this.numericUpDown_exposureTime_ValueChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(140, 533);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Exp. (ms)";
            // 
            // label_gainMin
            // 
            this.label_gainMin.AutoSize = true;
            this.label_gainMin.Location = new System.Drawing.Point(9, 572);
            this.label_gainMin.Name = "label_gainMin";
            this.label_gainMin.Size = new System.Drawing.Size(17, 13);
            this.label_gainMin.TabIndex = 9;
            this.label_gainMin.Text = "xx";
            // 
            // label_gainMax
            // 
            this.label_gainMax.AutoSize = true;
            this.label_gainMax.Location = new System.Drawing.Point(49, 572);
            this.label_gainMax.Name = "label_gainMax";
            this.label_gainMax.Size = new System.Drawing.Size(17, 13);
            this.label_gainMax.TabIndex = 10;
            this.label_gainMax.Text = "xx";
            // 
            // label_frameRateMin
            // 
            this.label_frameRateMin.AutoSize = true;
            this.label_frameRateMin.Location = new System.Drawing.Point(73, 572);
            this.label_frameRateMin.Name = "label_frameRateMin";
            this.label_frameRateMin.Size = new System.Drawing.Size(17, 13);
            this.label_frameRateMin.TabIndex = 11;
            this.label_frameRateMin.Text = "xx";
            // 
            // label_frameRateMax
            // 
            this.label_frameRateMax.AutoSize = true;
            this.label_frameRateMax.Location = new System.Drawing.Point(113, 572);
            this.label_frameRateMax.Name = "label_frameRateMax";
            this.label_frameRateMax.Size = new System.Drawing.Size(17, 13);
            this.label_frameRateMax.TabIndex = 12;
            this.label_frameRateMax.Text = "xx";
            // 
            // label_exposureTimeMin
            // 
            this.label_exposureTimeMin.AutoSize = true;
            this.label_exposureTimeMin.Location = new System.Drawing.Point(133, 572);
            this.label_exposureTimeMin.Name = "label_exposureTimeMin";
            this.label_exposureTimeMin.Size = new System.Drawing.Size(17, 13);
            this.label_exposureTimeMin.TabIndex = 13;
            this.label_exposureTimeMin.Text = "xx";
            // 
            // label_exposureTimeMax
            // 
            this.label_exposureTimeMax.AutoSize = true;
            this.label_exposureTimeMax.Location = new System.Drawing.Point(173, 572);
            this.label_exposureTimeMax.Name = "label_exposureTimeMax";
            this.label_exposureTimeMax.Size = new System.Drawing.Size(17, 13);
            this.label_exposureTimeMax.TabIndex = 14;
            this.label_exposureTimeMax.Text = "xx";
            // 
            // numericUpDown_IR
            // 
            this.numericUpDown_IR.Location = new System.Drawing.Point(839, 586);
            this.numericUpDown_IR.Name = "numericUpDown_IR";
            this.numericUpDown_IR.Size = new System.Drawing.Size(45, 20);
            this.numericUpDown_IR.TabIndex = 15;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(845, 572);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 13);
            this.label4.TabIndex = 16;
            this.label4.Text = "IR";
            // 
            // numericUpDown_red
            // 
            this.numericUpDown_red.Location = new System.Drawing.Point(890, 586);
            this.numericUpDown_red.Name = "numericUpDown_red";
            this.numericUpDown_red.Size = new System.Drawing.Size(45, 20);
            this.numericUpDown_red.TabIndex = 17;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(894, 571);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(27, 13);
            this.label5.TabIndex = 18;
            this.label5.Text = "Red";
            // 
            // button_pluxStart
            // 
            this.button_pluxStart.Location = new System.Drawing.Point(1027, 560);
            this.button_pluxStart.Name = "button_pluxStart";
            this.button_pluxStart.Size = new System.Drawing.Size(75, 23);
            this.button_pluxStart.TabIndex = 19;
            this.button_pluxStart.Text = "plux start";
            this.button_pluxStart.UseVisualStyleBackColor = true;
            this.button_pluxStart.Click += new System.EventHandler(this.button_pluxStart_Click);
            // 
            // button_pluxStop
            // 
            this.button_pluxStop.Location = new System.Drawing.Point(1027, 583);
            this.button_pluxStop.Name = "button_pluxStop";
            this.button_pluxStop.Size = new System.Drawing.Size(75, 23);
            this.button_pluxStop.TabIndex = 20;
            this.button_pluxStop.Text = "plux stop";
            this.button_pluxStop.UseVisualStyleBackColor = true;
            this.button_pluxStop.Click += new System.EventHandler(this.button_pluxStop_Click);
            // 
            // label_pluxState
            // 
            this.label_pluxState.AutoSize = true;
            this.label_pluxState.Location = new System.Drawing.Point(873, 549);
            this.label_pluxState.Name = "label_pluxState";
            this.label_pluxState.Size = new System.Drawing.Size(27, 13);
            this.label_pluxState.TabIndex = 21;
            this.label_pluxState.Text = "N/A";
            this.label_pluxState.Click += new System.EventHandler(this.label_pluxState_Click);
            // 
            // checkBox_LED
            // 
            this.checkBox_LED.AutoSize = true;
            this.checkBox_LED.Location = new System.Drawing.Point(1126, 550);
            this.checkBox_LED.Name = "checkBox_LED";
            this.checkBox_LED.Size = new System.Drawing.Size(47, 17);
            this.checkBox_LED.TabIndex = 22;
            this.checkBox_LED.Text = "LED";
            this.checkBox_LED.UseVisualStyleBackColor = true;
            this.checkBox_LED.CheckedChanged += new System.EventHandler(this.checkBox_LED_CheckedChanged);
            // 
            // numericUpDown_LED
            // 
            this.numericUpDown_LED.Location = new System.Drawing.Point(1125, 569);
            this.numericUpDown_LED.Name = "numericUpDown_LED";
            this.numericUpDown_LED.Size = new System.Drawing.Size(48, 20);
            this.numericUpDown_LED.TabIndex = 23;
            this.numericUpDown_LED.ValueChanged += new System.EventHandler(this.numericUpDown_LED_ValueChanged);
            // 
            // label_comPortStatus
            // 
            this.label_comPortStatus.AutoSize = true;
            this.label_comPortStatus.Location = new System.Drawing.Point(236, 533);
            this.label_comPortStatus.Name = "label_comPortStatus";
            this.label_comPortStatus.Size = new System.Drawing.Size(27, 13);
            this.label_comPortStatus.TabIndex = 24;
            this.label_comPortStatus.Text = "N/A";
            this.label_comPortStatus.Click += new System.EventHandler(this.label_comPortStatus_Click);
            // 
            // numericUpDown_pictureBoxTimeDecimation
            // 
            this.numericUpDown_pictureBoxTimeDecimation.Location = new System.Drawing.Point(324, 543);
            this.numericUpDown_pictureBoxTimeDecimation.Name = "numericUpDown_pictureBoxTimeDecimation";
            this.numericUpDown_pictureBoxTimeDecimation.Size = new System.Drawing.Size(43, 20);
            this.numericUpDown_pictureBoxTimeDecimation.TabIndex = 25;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(321, 527);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(33, 13);
            this.label6.TabIndex = 26;
            this.label6.Text = "t sub.";
            // 
            // numericUpDown_bufferSize
            // 
            this.numericUpDown_bufferSize.Location = new System.Drawing.Point(324, 585);
            this.numericUpDown_bufferSize.Name = "numericUpDown_bufferSize";
            this.numericUpDown_bufferSize.Size = new System.Drawing.Size(52, 20);
            this.numericUpDown_bufferSize.TabIndex = 27;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(321, 569);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(34, 13);
            this.label7.TabIndex = 28;
            this.label7.Text = "buffer";
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(213, 574);
            this.buttonStop.Name = "buttonStop";
            this.buttonStop.Size = new System.Drawing.Size(75, 23);
            this.buttonStop.TabIndex = 30;
            this.buttonStop.Text = "video stop";
            this.buttonStop.UseVisualStyleBackColor = true;
            this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(213, 551);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 29;
            this.buttonStart.Text = "video start";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // button_stopTriger
            // 
            this.button_stopTriger.Location = new System.Drawing.Point(480, 581);
            this.button_stopTriger.Name = "button_stopTriger";
            this.button_stopTriger.Size = new System.Drawing.Size(75, 23);
            this.button_stopTriger.TabIndex = 32;
            this.button_stopTriger.Text = "stop";
            this.button_stopTriger.UseVisualStyleBackColor = true;
            this.button_stopTriger.Click += new System.EventHandler(this.button_stopTriger_Click);
            // 
            // button_triger
            // 
            this.button_triger.Location = new System.Drawing.Point(480, 558);
            this.button_triger.Name = "button_triger";
            this.button_triger.Size = new System.Drawing.Size(75, 23);
            this.button_triger.TabIndex = 31;
            this.button_triger.Text = "start";
            this.button_triger.UseVisualStyleBackColor = true;
            this.button_triger.Click += new System.EventHandler(this.button_triger_Click);
            // 
            // textBox_dataname
            // 
            this.textBox_dataname.Location = new System.Drawing.Point(480, 533);
            this.textBox_dataname.Name = "textBox_dataname";
            this.textBox_dataname.Size = new System.Drawing.Size(217, 20);
            this.textBox_dataname.TabIndex = 33;
            this.textBox_dataname.Text = "filename";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(966, 568);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(33, 13);
            this.label8.TabIndex = 35;
            this.label8.Text = "t sub.";
            // 
            // numericUpDown_subsampling
            // 
            this.numericUpDown_subsampling.Location = new System.Drawing.Point(969, 584);
            this.numericUpDown_subsampling.Name = "numericUpDown_subsampling";
            this.numericUpDown_subsampling.Size = new System.Drawing.Size(43, 20);
            this.numericUpDown_subsampling.TabIndex = 34;
            // 
            // checkBox_exposurTimeMax
            // 
            this.checkBox_exposurTimeMax.AutoSize = true;
            this.checkBox_exposurTimeMax.Location = new System.Drawing.Point(136, 588);
            this.checkBox_exposurTimeMax.Name = "checkBox_exposurTimeMax";
            this.checkBox_exposurTimeMax.Size = new System.Drawing.Size(45, 17);
            this.checkBox_exposurTimeMax.TabIndex = 36;
            this.checkBox_exposurTimeMax.Text = "max";
            this.checkBox_exposurTimeMax.UseVisualStyleBackColor = true;
            this.checkBox_exposurTimeMax.CheckedChanged += new System.EventHandler(this.checkBox_exposurTimeMax_CheckedChanged);
            // 
            // labelCounter
            // 
            this.labelCounter.AutoSize = true;
            this.labelCounter.Location = new System.Drawing.Point(562, 560);
            this.labelCounter.Name = "labelCounter";
            this.labelCounter.Size = new System.Drawing.Size(22, 13);
            this.labelCounter.TabIndex = 37;
            this.labelCounter.Text = "xxx";
            // 
            // label_fps
            // 
            this.label_fps.AutoSize = true;
            this.label_fps.Location = new System.Drawing.Point(624, 560);
            this.label_fps.Name = "label_fps";
            this.label_fps.Size = new System.Drawing.Size(31, 13);
            this.label_fps.TabIndex = 38;
            this.label_fps.Text = "xxfps";
            // 
            // label_pixelClock
            // 
            this.label_pixelClock.AutoSize = true;
            this.label_pixelClock.Location = new System.Drawing.Point(561, 589);
            this.label_pixelClock.Name = "label_pixelClock";
            this.label_pixelClock.Size = new System.Drawing.Size(57, 13);
            this.label_pixelClock.TabIndex = 39;
            this.label_pixelClock.Text = "xxxMclock";
            // 
            // label_error
            // 
            this.label_error.AutoSize = true;
            this.label_error.Location = new System.Drawing.Point(624, 589);
            this.label_error.Name = "label_error";
            this.label_error.Size = new System.Drawing.Size(12, 13);
            this.label_error.TabIndex = 40;
            this.label_error.Text = "x";
            // 
            // panel_plux
            // 
            this.panel_plux.Location = new System.Drawing.Point(740, 28);
            this.panel_plux.Name = "panel_plux";
            this.panel_plux.Size = new System.Drawing.Size(618, 516);
            this.panel_plux.TabIndex = 41;
            // 
            // advancedSettingsToolStripMenuItem
            // 
            this.advancedSettingsToolStripMenuItem.Name = "advancedSettingsToolStripMenuItem";
            this.advancedSettingsToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.advancedSettingsToolStripMenuItem.Text = "advanced settings";
            this.advancedSettingsToolStripMenuItem.Click += new System.EventHandler(this.advancedSettingsToolStripMenuItem_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1370, 631);
            this.Controls.Add(this.panel_plux);
            this.Controls.Add(this.label_error);
            this.Controls.Add(this.label_pixelClock);
            this.Controls.Add(this.label_fps);
            this.Controls.Add(this.labelCounter);
            this.Controls.Add(this.checkBox_exposurTimeMax);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.numericUpDown_subsampling);
            this.Controls.Add(this.textBox_dataname);
            this.Controls.Add(this.button_stopTriger);
            this.Controls.Add(this.button_triger);
            this.Controls.Add(this.buttonStop);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.numericUpDown_bufferSize);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.numericUpDown_pictureBoxTimeDecimation);
            this.Controls.Add(this.label_comPortStatus);
            this.Controls.Add(this.numericUpDown_LED);
            this.Controls.Add(this.checkBox_LED);
            this.Controls.Add(this.label_pluxState);
            this.Controls.Add(this.button_pluxStop);
            this.Controls.Add(this.button_pluxStart);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.numericUpDown_red);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.numericUpDown_IR);
            this.Controls.Add(this.label_exposureTimeMax);
            this.Controls.Add(this.label_exposureTimeMin);
            this.Controls.Add(this.label_frameRateMax);
            this.Controls.Add(this.label_frameRateMin);
            this.Controls.Add(this.label_gainMax);
            this.Controls.Add(this.label_gainMin);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.numericUpDown_exposureTime);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.numericUpDown_frameRate);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.numericUpDown_gain);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.pictureBoxWithInterpolationMode1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "VO software";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxWithInterpolationMode1)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_gain)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_frameRate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_exposureTime)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_IR)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_red)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_LED)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_pictureBoxTimeDecimation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_bufferSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_subsampling)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.ToolStripMenuItem loadSettingsToolStripMenuItem;
        public System.Windows.Forms.ToolStripMenuItem saveSettingsToolStripMenuItem;
        public System.Windows.Forms.NumericUpDown numericUpDown_IR;
        public System.Windows.Forms.Label label4;
        public System.Windows.Forms.NumericUpDown numericUpDown_red;
        public System.Windows.Forms.Label label5;
        public System.Windows.Forms.Button button_pluxStart;
        public System.Windows.Forms.Button button_pluxStop;
        public System.Windows.Forms.Label label_pluxState;
        public System.Windows.Forms.CheckBox checkBox_LED;
        public System.Windows.Forms.NumericUpDown numericUpDown_LED;
        public System.Windows.Forms.Label label_comPortStatus;
        public System.Windows.Forms.NumericUpDown numericUpDown_pictureBoxTimeDecimation;
        public System.Windows.Forms.Label label6;
        public System.Windows.Forms.NumericUpDown numericUpDown_bufferSize;
        public System.Windows.Forms.Label label7;
        public System.Windows.Forms.Button buttonStop;
        public System.Windows.Forms.Button buttonStart;
        public System.Windows.Forms.Button button_stopTriger;
        public System.Windows.Forms.Button button_triger;
        public System.Windows.Forms.TextBox textBox_dataname;
        public System.Windows.Forms.Label label8;
        public System.Windows.Forms.NumericUpDown numericUpDown_subsampling;
        public System.Windows.Forms.CheckBox checkBox_exposurTimeMax;
        public System.Windows.Forms.Label labelCounter;
        public System.Windows.Forms.Label label_fps;
        public System.Windows.Forms.Label label_pixelClock;
        public System.Windows.Forms.Label label_error;
        public System.Windows.Forms.Panel panel_plux;
        public PictureBoxWithInterpolationMode pictureBoxWithInterpolationMode1;
        public System.Windows.Forms.StatusStrip statusStrip;
        public System.Windows.Forms.MenuStrip menuStrip1;
        public System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        public System.Windows.Forms.ToolStripMenuItem actionsToolStripMenuItem;
        public System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        public System.Windows.Forms.NumericUpDown numericUpDown_gain;
        public System.Windows.Forms.Label label1;
        public System.Windows.Forms.NumericUpDown numericUpDown_frameRate;
        public System.Windows.Forms.Label label2;
        public System.Windows.Forms.NumericUpDown numericUpDown_exposureTime;
        public System.Windows.Forms.Label label3;
        public System.Windows.Forms.Label label_gainMin;
        public System.Windows.Forms.Label label_gainMax;
        public System.Windows.Forms.Label label_frameRateMin;
        public System.Windows.Forms.Label label_frameRateMax;
        public System.Windows.Forms.Label label_exposureTimeMin;
        public System.Windows.Forms.Label label_exposureTimeMax;
        private System.Windows.Forms.ToolStripMenuItem advancedSettingsToolStripMenuItem;
    }
}


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VO_soft
{
    public partial class FormSettings : Form
    {
        private Form1 form1;

        public FormSettings(Form1 form1)
        {
            this.form1 = form1;
            InitializeComponent();
        }

        private void numericUpDown_x_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_x.ValueChanged -= new System.EventHandler(this.numericUpDown_x_ValueChanged);
            numericUpDown_x.Value = numericUpDown_x.Value - (numericUpDown_x.Value % numericUpDown_x.Increment);
            numericUpDown_x.ValueChanged += new System.EventHandler(this.numericUpDown_x_ValueChanged);
            positionCheck();
        }

        private void positionCheck()
        {
            form1.buttonStart.Enabled = true;
            label_xSumMax.Enabled = true;
            label_ySumMax.Enabled = true;

            if ((numericUpDown_x.Value + numericUpDown_w.Value) > Convert.ToDecimal(label_xSumMax.Text))
            {
                form1.buttonStart.Enabled = false;
                label_xSumMax.Enabled = false;
            }
            if ((numericUpDown_y.Value + numericUpDown_h.Value) > Convert.ToDecimal(label_ySumMax.Text))
            {
                form1.buttonStart.Enabled = false;
                label_ySumMax.Enabled = false;
            }

        }

        private void numericUpDown_y_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_y.ValueChanged -= new System.EventHandler(this.numericUpDown_y_ValueChanged);
            numericUpDown_y.Value = numericUpDown_y.Value - (numericUpDown_y.Value % numericUpDown_y.Increment);
            numericUpDown_y.ValueChanged += new System.EventHandler(this.numericUpDown_y_ValueChanged);
            positionCheck();
        }

        private void numericUpDown_w_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_w.ValueChanged -= new System.EventHandler(this.numericUpDown_w_ValueChanged);
            numericUpDown_w.Value = numericUpDown_w.Value - (numericUpDown_w.Value % numericUpDown_w.Increment);
            numericUpDown_w.ValueChanged += new System.EventHandler(this.numericUpDown_w_ValueChanged);
            positionCheck();
        }

        private void numericUpDown_h_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_h.ValueChanged -= new System.EventHandler(this.numericUpDown_h_ValueChanged);
            numericUpDown_h.Value = numericUpDown_h.Value - (numericUpDown_h.Value % numericUpDown_h.Increment);
            numericUpDown_h.ValueChanged += new System.EventHandler(this.numericUpDown_h_ValueChanged);
            positionCheck();
            form1.updatePixelClock();
        }

        public void comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Form1Setter.SetCharts(form1);
            form1.pluxBackEnd.closePlux();
            Thread.Sleep(100);
            form1.label_pluxState_Click(this, EventArgs.Empty);
        }

        public void checkBox_visible_led_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_visible_led.Checked)
            {
                form1.checkBox_LED.Visible = true;
                form1.numericUpDown_LED.Visible = true;
            }
            else 
            {
                form1.checkBox_LED.Visible = false;
                form1.numericUpDown_LED.Visible = false;
            }
        }

        public void checkBox_visible_two_wl_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_visible_two_wl.Checked)
            {
                form1.checkBox_show_1_wl.Visible = true;
                form1.numericUpDown_wl_to_show.Visible = true;
                form1.checkBox_2xfps.Visible = true;
                form1.numericUpDown_gain2xfps.Visible = true;
                form1.checkBox_sum2frames.Visible = true;
                form1.checkBox_one_wl_stable.Visible = true;

            }
            else
            {
                form1.checkBox_show_1_wl.Visible = false;
                form1.numericUpDown_wl_to_show.Visible = false;
                form1.checkBox_2xfps.Visible = false;
                form1.numericUpDown_gain2xfps.Visible = false;
                form1.checkBox_sum2frames.Visible = false;
                form1.checkBox_one_wl_stable.Visible = false;
                form1.checkBox_one_wl_stable.Checked = false;

            }
            form1.checkBox_show_1_wl_CheckedChanged(null, EventArgs.Empty);
        }

        public void checkBox_display_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_display.Checked)
            {
                form1.checkBox_showDot.Visible = true;
                form1.numericUpDown_dotX.Visible = true;
                form1.numericLeftRight_dotY.Visible = true;
                form1.label10.Visible = true;
                form1.numericUpDown_R.Visible = true;
                form1.pictureBox_secondScreen.Visible = true;
                form1.secondScreenUpdater.updateDot();
            }
            else
            {
                form1.checkBox_showDot.Visible = false;
                form1.numericUpDown_dotX.Visible = false;
                form1.numericLeftRight_dotY.Visible = false;
                form1.label10.Visible = false;
                form1.numericUpDown_R.Visible = false;
                form1.pictureBox_secondScreen.Visible = false;
                form1.secondScreenUpdater.Close();
            }
        }

        private void button_select_save_path_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog myFolderBrowserDialog = new FolderBrowserDialog();
            if (!(myFolderBrowserDialog.ShowDialog() == DialogResult.OK))
            {
                return;
            }

            textBox_save_path.Text = myFolderBrowserDialog.SelectedPath;
        }

        private void FormSettings_Load(object sender, EventArgs e)
        {

        }


        private void checkBox_flicker_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_flicker.Checked)
            {
                form1.button_flicker.Visible = true;
                form1.label_flicker_freq.Visible = true;
                checkBox_display.Checked = true;
            }
            else
            {
                form1.button_flicker.Visible = false;
                form1.label_flicker_freq.Visible = false;
            }

        }
        private void comboBox_pixelClock_SelectedIndexChanged(object sender, EventArgs e)
        {
            form1.updatePixelClock();
        }

        private void numericUpDown_freq_ValueChanged(object sender, EventArgs e)
        {

        }
    }
}

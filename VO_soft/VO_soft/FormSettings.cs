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
    }
}

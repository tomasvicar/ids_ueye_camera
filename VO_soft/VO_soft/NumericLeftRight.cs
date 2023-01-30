using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VO_soft
{
    public partial class NumericLeftRight : UserControl
    {
        public NumericLeftRight()
        {
            InitializeComponent();
        }
        public NumericUpDown NumericUpDown
        {

            get 
            {
                return this.numericUpDown1;
            }
        
        
        }

        private void button1_Click(object sender, EventArgs e)
        {
            NumericUpDown.UpButton();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            NumericUpDown.DownButton();
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {

        }
    }
}

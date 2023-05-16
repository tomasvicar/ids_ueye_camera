using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simple_secondscreen
{
    public partial class Form1 : Form
    {
        Form f2;
        Form f3;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            

        }

        private void button_open_Click(object sender, EventArgs e)
        {


            f3 = new Form3();
            f3.Show();
        }

        private void button_close_Click(object sender, EventArgs e)
        {
            if (f2 != null)
            {
                if (!f2.IsDisposed)
                {
                    f2.Close();
                }
            }

            if (f3 != null)
            {
                if (!f3.IsDisposed)
                {
                    f3.Close();
                }
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            f2 = new Form2();
            f2.Show();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {

        }
    }
}

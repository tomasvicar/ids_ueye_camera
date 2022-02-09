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

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            

        }

        private void button_open_Click(object sender, EventArgs e)
        {


            f2 = new Form2();
            f2.Show();
        }

        private void button_close_Click(object sender, EventArgs e)
        {
            f2.Close();
        }
    }
}

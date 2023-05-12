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
    public partial class FormSecondScreen : Form
    {

        public Form1 form1 { get; set; }

        public FormSecondScreen()
        {
            InitializeComponent();

        }

        private void FormSecondScreen_Load(object sender, EventArgs e)
        {
            BackColor = Color.Black;
            FormBorderStyle = FormBorderStyle.None;
            Bounds = Screen.AllScreens[1].Bounds;
            TopMost = true;
        }

    }
}

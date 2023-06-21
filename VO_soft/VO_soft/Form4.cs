using AxWMPLib;
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
    public partial class Form4 : Form
    {
        private Form1 form1;
        private AxWindowsMediaPlayer player;

        public Form4(Form1 form1)
        {
            this.form1 = form1;
            InitializeComponent();
            player = new AxWindowsMediaPlayer();
            ((ISupportInitialize)(player)).BeginInit();  // Add this line
            Controls.Add(player);
            player.Dock = DockStyle.Fill;
            ((ISupportInitialize)(player)).EndInit();   // Add this line

            Load += Form4_Load;
            player.PlayStateChange += Player_PlayStateChange;

            player.URL = @".\test.avi";


            if (Screen.AllScreens.Length > 1)
            {
                // Set this form to be displayed on the second monitor.
                Screen secondaryScreen = Screen.AllScreens[1];
                this.Left = secondaryScreen.WorkingArea.Left;
                this.Top = secondaryScreen.WorkingArea.Top;
            }
        }

        private void axWindowsMediaPlayer1_Enter(object sender, EventArgs e)
        {

        }

        private void Form4_Load(object sender, EventArgs e)
        {
            player.settings.autoStart = true;
            player.uiMode = "none"; // this hides the controls
            player.stretchToFit = true; // this makes the video to fill the form
            player.settings.setMode("loop", true);
        }
        private void Player_PlayStateChange(object sender, AxWMPLib._WMPOCXEvents_PlayStateChangeEvent e)
        {
            if (e.newState == (int)WMPLib.WMPPlayState.wmppsPlaying)
            {
                player.fullScreen = true;
            }
            //else if (e.newState == (int)WMPLib.WMPPlayState.wmppsMediaEnded)
            //{
            //    player.Ctlcontrols.play();
            //}
        }

    }

}
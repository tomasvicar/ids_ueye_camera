using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;


//using System.Runtime.InteropServices;
//using System.Drawing;

namespace simple_secondscreen
{
    static class Program
    {

        //[DllImport("User32.dll")]
        //public static extern IntPtr GetDC(IntPtr hwnd);
        //[DllImport("User32.dll")]
        //public static extern void ReleaseDC(IntPtr hwnd, IntPtr dc);

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //IntPtr desktopPtr = GetDC(IntPtr.Zero);
            //Graphics g = Graphics.FromHdc(desktopPtr);

            //SolidBrush b = new SolidBrush(Color.Red);
            //g.FillRectangle(b, new Rectangle(0, 0, 1920, 1080));

            //g.Dispose();
            //ReleaseDC(IntPtr.Zero, desktopPtr);

            Form f1 = new Form1();


            Application.EnableVisualStyles();
            Application.Run(f1);


        }
    }
}

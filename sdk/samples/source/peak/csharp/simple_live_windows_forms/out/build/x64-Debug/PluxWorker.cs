using System;
using System.IO;

namespace simple_live_windows_forms
{


    public class PluxWorker
    {

        public MyDevice dev { get; set; }

        public void DoWork()
        {
            dev.textfile = new StreamWriter("tmp.txt");
            dev.Loop();
            dev.Stop();
            dev.textfile.Close();
        }


    }
}

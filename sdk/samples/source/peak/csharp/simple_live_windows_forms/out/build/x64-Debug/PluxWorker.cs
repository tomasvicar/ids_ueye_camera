using System;
using System.IO;

namespace simple_live_windows_forms
{


    public class PluxWorker
    {

        public MyDevice dev { get; set; }

        public void DoWork()
        {

            Console.WriteLine("Start loop");
            dev.to_write = "";
            dev.Loop();
            Console.WriteLine("Close loop");
            dev.Stop();
            if (dev.textfile != null)
            {
                if (dev.textfile.BaseStream != null)
                {
                    dev.textfile.Close();
                }
            }
        }


    }
}

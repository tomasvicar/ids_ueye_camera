using System;
using System.IO;

namespace VO_soft
{


    public class PluxWorker
    {

        public MyPluxDevice dev { get; set; }

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

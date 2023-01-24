using System;
using System.Windows.Forms;
using System.IO;

namespace VO_soft
{
    public class MyPluxDevice : PluxDotNet.SignalsDev
    {
        public int freq = 1000;
        public int show_time_range_s = 10;
        public bool running = false;
        public Form1 form1 { get; set; }
        public StreamWriter textfile { get; set; }
        public string to_write = "";
        public bool record { get; set; }
        public string filename { get; set; }

        public int subsample_plot { get; set; }
        public int subsample_write = 10;
        internal int chart_count;

        public MyPluxDevice(string path) : base(path)
        {
        }
        public override bool OnRawFrame(int nSeq, int[] data)
        {

            //Console.WriteLine("OnFrame");

            if (!running)
                return true;

            if (record)
            {
                to_write += DateTime.Now.ToString("HH:mm:ss.fff");
                to_write += ";" + nSeq.ToString();
                foreach (int val in data)
                    to_write += ";" + val.ToString();

                to_write += "\n";

                if (nSeq % subsample_write == 0)
                {
                    textfile.Write(to_write);
                    to_write = "";
                }

            }
            else
            {
                to_write = "";
            }

            

            if (nSeq % subsample_plot == 0)
            {

                decimal t = (decimal)nSeq / (decimal)freq;

                float remove = ((float)show_time_range_s / (float)subsample_plot) * (float)freq;


                for (int i = 0; i < chart_count; i++)
                {


                }


            return false;
        }


    }
}

using System;
using System.Windows.Forms;
using System.IO;



namespace simple_live_windows_forms
{
    public class MyDevice : PluxDotNet.SignalsDev
    {
        public int freq = 1000;
        public bool running;
        public FormWindow form1 { get; set; }
        public StreamWriter textfile { get; set; }
        private string to_write = "";
        public bool record { get; set; }
        public string filename { get; set; }

        public MyDevice(string path) : base(path)
        {
        }
        public override bool OnRawFrame(int nSeq, int[] data)
        {

            

            to_write += DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
            to_write += "; " + nSeq.ToString();
            foreach (int val in data)
                to_write += "; " + val.ToString();

            to_write += "\n";

            //System.Console.WriteLine(to_write);

            if (nSeq % 20 == 0)
            {  // here once per second
               // print one frame
               //System.Console.Write("[" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + "]");
               //System.Console.Write(" {0} -", nSeq);

                //foreach (int val in data)
                //System.Console.Write(" {0}", val);


                textfile.Write(to_write);
                to_write = "";



                form1.chart1.BeginInvoke((MethodInvoker)delegate {
                    form1.chart1.Series["test1"].Points.AddXY(nSeq, data[0]);
                    if (form1.chart1.Series["test1"].Points.Count > 500)
                    {
                        form1.chart1.Series["test1"].Points.RemoveAt(0);
                        
                    }
                    form1.chart1.ChartAreas[0].RecalculateAxesScale();


                });


            }

            if (!running)
                return true;  

            return false;
        }


    }
}

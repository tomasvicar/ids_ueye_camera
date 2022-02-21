using System;
using System.Windows.Forms;
using System.IO;



namespace simple_live_windows_forms
{
    public class MyDevice : PluxDotNet.SignalsDev
    {
        public int freq = 1000;
        public int show_time_range_s = 10;
        public bool running = false;
        public FormWindow form1 { get; set; }
        public StreamWriter textfile { get; set; }
        private string to_write = "";
        public bool record { get; set; }
        public string filename { get; set; }

        public int subsample_plot { get; set; }
        public int subsample_write = 10;

        public MyDevice(string path) : base(path)
        {
        }
        public override bool OnRawFrame(int nSeq, int[] data)
        {

            //Console.WriteLine("OnFrame");

            if (!running)
                return true;

            if (record)
            {
                to_write += DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                to_write += "; " + nSeq.ToString();
                foreach (int val in data)
                    to_write += "; " + val.ToString();

                to_write += "\n";

                if (nSeq % subsample_write == 0)

                    textfile.Write(to_write);
                    to_write = "";

            }
            else 
            {
                to_write = "";
            }

           

            //System.Console.WriteLine(to_write);

            if (nSeq % subsample_plot == 0)
            {
                //Console.WriteLine("Plot");

                float t = (float)nSeq / (float)freq;

                float remove = ((float)show_time_range_s / (float)subsample_plot) * (float)freq;
                //Console.WriteLine(remove);


                //System.Console.Write("[" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + "]");
                //System.Console.Write(" {0} -", nSeq);

                //foreach (int val in data)
                //    System.Console.Write(" {0}", val);


                double ecg = data[0];
                double spiro = data[1];
                double triger = data[2];
                double red = data[3];
                double ir = data[4];

                ecg = (ecg / 65536.0 - 0.5) * 3.0;
                spiro = (spiro / 65536.0 - 0.5) * 100;
                triger = (triger / 65536.0) * 3 - 1.5;
                red = (red / 65536.0) * 1.2;
                ir = (ir / 65536.0) * 1.2;


                form1.chart1.BeginInvoke((MethodInvoker)delegate {
                    form1.chart1.Series[0].Points.AddXY(t, ecg);
                    if (form1.chart1.Series[0].Points.Count > remove)
                    {
                        form1.chart1.Series[0].Points.RemoveAt(0);
                        
                    }
                    form1.chart1.ChartAreas[0].RecalculateAxesScale();
                });

                form1.chart2.BeginInvoke((MethodInvoker)delegate {
                    form1.chart2.Series[0].Points.AddXY(t, spiro);
                    if (form1.chart2.Series[0].Points.Count > remove)
                    {
                        form1.chart2.Series[0].Points.RemoveAt(0);

                    }
                    form1.chart2.ChartAreas[0].RecalculateAxesScale();
                });

                form1.chart3.BeginInvoke((MethodInvoker)delegate {
                    form1.chart3.Series[0].Points.AddXY(t, triger);
                    if (form1.chart3.Series[0].Points.Count > remove)
                    {
                        form1.chart3.Series[0].Points.RemoveAt(0);

                    }
                    form1.chart3.ChartAreas[0].RecalculateAxesScale();
                });

                form1.chart4.BeginInvoke((MethodInvoker)delegate {
                    form1.chart4.Series[0].Points.AddXY(t, red);
                    if (form1.chart4.Series[0].Points.Count > remove)
                    {
                        form1.chart4.Series[0].Points.RemoveAt(0);

                    }
                    form1.chart4.ChartAreas[0].RecalculateAxesScale();
                });

                form1.chart5.BeginInvoke((MethodInvoker)delegate {
                    form1.chart5.Series[0].Points.AddXY(t, ir);
                    if (form1.chart5.Series[0].Points.Count > remove)
                    {
                        form1.chart5.Series[0].Points.RemoveAt(0);

                    }
                    form1.chart5.ChartAreas[0].RecalculateAxesScale();
                });



            }
 

            return false;
        }


    }
}

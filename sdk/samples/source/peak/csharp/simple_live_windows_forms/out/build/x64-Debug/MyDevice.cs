using System;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using System.Windows.Forms.DataVisualization.Charting;


namespace simple_live_windows_forms
{
    public class MyDevice : PluxDotNet.SignalsDev
    {
        public int freq = 1000;
        public int show_time_range_s = 10;
        public bool running = false;
        public FormWindow form1 { get; set; }
        public StreamWriter textfile { get; set; }
        public string to_write = "";
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

            //to_write = "";
            //to_write += DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
            //to_write += "; " + nSeq.ToString();
            //foreach (int val in data)
            //    to_write += "; " + val.ToString();

            //textfile.WriteLine(to_write);

            //System.Console.WriteLine(to_write);

            if (nSeq % subsample_plot == 0)
            {
                //Console.WriteLine("Plot");

                //float t = (float)nSeq / (float)freq;
                //t = (float)(Math.Round((double)t, 2));
                decimal t = (decimal)nSeq / (decimal)freq;

                float remove = ((float)show_time_range_s / (float)subsample_plot) * (float)freq;
                //Console.WriteLine(remove);


                //System.Console.Write("[" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + "]");
                //System.Console.Write(" {0} -", nSeq);

                //foreach (int val in data)
                //    System.Console.Write(" {0}", val);


                //decimal ecg = Convert.ToDecimal(data[0]);
                //decimal spiro = Convert.ToDecimal(data[1]);
                //decimal triger = Convert.ToDecimal(data[2]);
                //decimal red = Convert.ToDecimal(data[3]);
                //decimal ir = Convert.ToDecimal(data[4]);

                //ecg = (ecg / 65536.0m - 0.5m) * 3.0m;
                //spiro = (spiro / 65536.0m - 0.5m) * 100m;
                //triger = (triger / 65536.0m) * 3m - 1.5m;
                //red = (red / 65536.0m) * 1.2m;
                //ir = (ir / 65536.0m) * 1.2m;

                List<Chart> ChartList = new List<Chart>() { };
                ChartList.Add(form1.chart1);
                ChartList.Add(form1.chart2);
                ChartList.Add(form1.chart3);
                ChartList.Add(form1.chart4);
                ChartList.Add(form1.chart5);
                //ChartList.Add(form1.chart6);
                //ChartList.Add(form1.chart7);
                //ChartList.Add(form1.chart8);


                ChartList[0].BeginInvoke((MethodInvoker)delegate
                {
                    ChartList[0].Series[0].Points.AddXY(t, Convert.ToDecimal(data[0]));
                    if (ChartList[0].Series[0].Points.Count > remove)
                        ChartList[0].Series[0].Points.RemoveAt(0);
                    ChartList[0].ChartAreas[0].RecalculateAxesScale();
                });

                ChartList[1].BeginInvoke((MethodInvoker)delegate
                {
                    ChartList[1].Series[0].Points.AddXY(t, Convert.ToDecimal(data[1]));
                    if (ChartList[1].Series[0].Points.Count > remove)
                        ChartList[1].Series[0].Points.RemoveAt(0);
                    ChartList[1].ChartAreas[0].RecalculateAxesScale();
                });

                ChartList[2].BeginInvoke((MethodInvoker)delegate
                {
                    ChartList[2].Series[0].Points.AddXY(t, Convert.ToDecimal(data[2]));
                    if (ChartList[2].Series[0].Points.Count > remove)
                        ChartList[2].Series[0].Points.RemoveAt(0);
                    ChartList[2].ChartAreas[0].RecalculateAxesScale();
                });

                ChartList[3].BeginInvoke((MethodInvoker)delegate
                {
                    ChartList[3].Series[0].Points.AddXY(t, Convert.ToDecimal(data[3]));
                    if (ChartList[3].Series[0].Points.Count > remove)
                        ChartList[3].Series[0].Points.RemoveAt(0);
                    ChartList[3].ChartAreas[0].RecalculateAxesScale();
                });

                ChartList[4].BeginInvoke((MethodInvoker)delegate
                {
                    ChartList[4].Series[0].Points.AddXY(t, Convert.ToDecimal(data[4]));
                    if (ChartList[4].Series[0].Points.Count > remove)
                        ChartList[4].Series[0].Points.RemoveAt(0);
                    ChartList[4].ChartAreas[0].RecalculateAxesScale();
                });

                //ChartList[5].BeginInvoke((MethodInvoker)delegate
                //{
                //    ChartList[5].Series[0].Points.AddXY(t, Convert.ToDecimal(data[5]));
                //    if (ChartList[5].Series[0].Points.Count > remove)
                //        ChartList[5].Series[0].Points.RemoveAt(0);
                //    ChartList[5].ChartAreas[0].RecalculateAxesScale();
                //});

                //ChartList[6].BeginInvoke((MethodInvoker)delegate
                //{
                //    ChartList[6].Series[0].Points.AddXY(t, Convert.ToDecimal(data[6]));
                //    if (ChartList[6].Series[0].Points.Count > remove)
                //        ChartList[6].Series[0].Points.RemoveAt(0);
                //    ChartList[6].ChartAreas[0].RecalculateAxesScale();
                //});

                //ChartList[7].BeginInvoke((MethodInvoker)delegate
                //{
                //    ChartList[7].Series[0].Points.AddXY(t, Convert.ToDecimal(data[7]));
                //    if (ChartList[7].Series[0].Points.Count > remove)
                //        ChartList[7].Series[0].Points.RemoveAt(0);
                //    ChartList[7].ChartAreas[0].RecalculateAxesScale();
                //});



            }


            return false;
        }


    }
}

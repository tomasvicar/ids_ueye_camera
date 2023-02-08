using System;
using System.Windows.Forms;
using System.IO;
using System.Windows.Forms.DataVisualization.Charting;

namespace VO_soft
{
    public class MyPluxDevice : PluxDotNet.SignalsDev
    {
        public int show_time_range_s;
        public bool running = false;
        public Form1 form1 { get; set; }
        public StreamWriter textfile { get; set; }
        public string to_write = "";
        public bool record { get; set; }
        public string filename { get; set; }

        public int subsample_plot { get; set; }
        public int subsample_write = 10;
        internal int chart_count;
        internal int freq;

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
                to_write += DateTime.Now.ToString("HH:mm:ss.ffff");
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


                for (int i = 1; i <= data.Length; i++)
                {
                    var d = Convert.ToDecimal(data[i - 1]);
                    switch (i)
                    {
                        case 1:
                            form1.chart1.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart1.Series[0].Points.AddXY(t, d);
                                if (form1.chart1.Series[0].Points.Count > remove)
                                {
                                    form1.chart1.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart1.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 2:
                            form1.chart2.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart2.Series[0].Points.AddXY(t, d);
                                if (form1.chart2.Series[0].Points.Count > remove)
                                {
                                    form1.chart2.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart2.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 3:
                            form1.chart3.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart3.Series[0].Points.AddXY(t, d);
                                if (form1.chart3.Series[0].Points.Count > remove)
                                {
                                    form1.chart3.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart3.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 4:
                            form1.chart4.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart4.Series[0].Points.AddXY(t, d);
                                if (form1.chart4.Series[0].Points.Count > remove)
                                {
                                    form1.chart4.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart4.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 5:
                            form1.chart5.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart5.Series[0].Points.AddXY(t, d);
                                if (form1.chart5.Series[0].Points.Count > remove)
                                {
                                    form1.chart5.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart5.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 6:
                            form1.chart6.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart6.Series[0].Points.AddXY(t, d);
                                if (form1.chart6.Series[0].Points.Count > remove)
                                {
                                    form1.chart6.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart6.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 7:
                            form1.chart7.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart7.Series[0].Points.AddXY(t, d);
                                if (form1.chart7.Series[0].Points.Count > remove)
                                {
                                    form1.chart7.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart7.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;
                        case 8:
                            form1.chart8.BeginInvoke((MethodInvoker)delegate
                            {
                                form1.chart8.Series[0].Points.AddXY(t, d);
                                if (form1.chart8.Series[0].Points.Count > remove)
                                {
                                    form1.chart8.Series[0].Points.RemoveAt(0);

                                }
                                form1.chart8.ChartAreas[0].RecalculateAxesScale();
                            });
                            break;


                    }

                }


            }
            return false;

        }


    }
}

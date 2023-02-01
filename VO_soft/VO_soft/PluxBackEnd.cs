using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace VO_soft
{
    public class PluxBackEnd
    {
        public Form1 form1;
        public MyPluxDevice dev = null;
        public PluxWorker pluxWorker;
        public List<PluxDotNet.Source> srcs;
        private int chart_count;
        private bool use_oxi;

        public PluxBackEnd(Form1 form1)
        {
            this.form1 = form1;
        }

        public void closePlux()
        {
            if (dev != null)
            {
                try
                {
                    dev.Dispose();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }

        internal void startRecordPlux(string filename)
        {
            if (dev.running)
            {
                form1.button_pluxStop_Click(this, EventArgs.Empty);
                
            }

            Thread.Sleep(150);

            dev.textfile = new StreamWriter(filename);
            dev.subsample_plot = Convert.ToInt32(form1.numericUpDown_subsampling.Value);


            Console.WriteLine("StreamWriter txt opened");

            dev.record = true;

            form1.button_pluxStart_Click(this, EventArgs.Empty);

            //Thread.Sleep(2000);
        }

        internal bool openPlux()
        {
            try
            {
                dev = new MyPluxDevice(form1.formSettings.textBox_MAC.Text);
                Dictionary<string, object> props = dev.GetProperties();

                dev.freq = Decimal.ToInt32(form1.formSettings.numericUpDown_pluxfreq.Value);  // acquisition base frequency of 1000 Hz
                dev.form1 = form1;
                dev.show_time_range_s = Decimal.ToInt32(form1.formSettings.numericUpDown_pluxShowS.Value);


                srcs = new List<PluxDotNet.Source>() { };
                PluxDotNet.Source src = new PluxDotNet.Source();

                chart_count = 0;
                use_oxi = false;


                List<ComboBox> comboBoxes = new List<ComboBox>();
                comboBoxes.Add(form1.formSettings.comboBox1);
                comboBoxes.Add(form1.formSettings.comboBox2);
                comboBoxes.Add(form1.formSettings.comboBox3);
                comboBoxes.Add(form1.formSettings.comboBox4);
                comboBoxes.Add(form1.formSettings.comboBox5);
                comboBoxes.Add(form1.formSettings.comboBox6);
                comboBoxes.Add(form1.formSettings.comboBox7);
                comboBoxes.Add(form1.formSettings.comboBox8);

                foreach (ComboBox obj_converted in comboBoxes)
                {

                    if (obj_converted.Text == "None")
                    {
                        continue;
                    }
                    else if (obj_converted.Text == "Oxi R + IR")
                    {
                        PluxDotNet.Source src_spo2_R_IR = new PluxDotNet.Source();
                        src_spo2_R_IR.port = 9;
                        src_spo2_R_IR.chMask = 0x03;
                        srcs.Add(src_spo2_R_IR);
                        use_oxi = true;
                        chart_count++;
                    }
                    else
                    {
                        src = new PluxDotNet.Source();
                        src.port = chart_count +1 ;
                        srcs.Add(src);
                    }
                    chart_count++;

                }

                if (use_oxi)
                {
                    int[] LED_param = { Decimal.ToInt32(form1.numericUpDown_red.Value), Decimal.ToInt32(form1.numericUpDown_IR.Value) };
                    dev.SetParameter(0x09, 0x03, LED_param, 2);
                }


                dev.Start(dev.freq, srcs);

                dev.Stop();

                return true;
            }
            catch
            {
                return false;
            }

        }

        internal void startPlux()
        {

            foreach (Control obj in form1.panel_plux.Controls)
            {
                if (obj.GetType() == typeof(Chart))
                {
                    Chart obj_converted = (Chart)Convert.ChangeType(obj, typeof(Chart));
                    if (obj_converted != null)
                    {
                        obj_converted.BeginInvoke((MethodInvoker)delegate
                        {
                            obj_converted.Series[0].Points.Clear();
                        });
                    }
                }
            }




            dev.subsample_plot = Convert.ToInt32(form1.numericUpDown_subsampling.Value);
            dev.chart_count = chart_count;

            if (use_oxi)
            {
                int[] LED_param = { Decimal.ToInt32(form1.numericUpDown_red.Value), Decimal.ToInt32(form1.numericUpDown_IR.Value) };
                dev.SetParameter(0x09, 0x03, LED_param, 2);
            }
            dev.Start(dev.freq, srcs);


            pluxWorker = new PluxWorker();
            pluxWorker.dev = dev;
            Thread t = new Thread(new ThreadStart(pluxWorker.DoWork));

            pluxWorker.dev.running = true;


            t.Start();

        }

        internal void stop()
        {
            pluxWorker.dev.running = false;
            dev.record = false;
        }

    }
}
using System;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace VO_soft
{
    internal class Form1Setter
    {
        private static Chart chart;

        public static void LoadMySetting(Form1 form1, CameraParameters cameraParameters)
        {
            form1.numericUpDown_gain.DecimalPlaces = 1;
            form1.numericUpDown_gain.Maximum = cameraParameters.gain_max;
            form1.numericUpDown_gain.Minimum = cameraParameters.gain_min;
            form1.numericUpDown_gain.Increment = 0.1m;
            form1.numericUpDown_gain.Value = 1.0m;


            form1.numericUpDown_gain2xfps.DecimalPlaces = 1;
            form1.numericUpDown_gain2xfps.Maximum = cameraParameters.gain_max;
            form1.numericUpDown_gain2xfps.Minimum = cameraParameters.gain_min;
            form1.numericUpDown_gain2xfps.Increment = 0.1m;
            form1.numericUpDown_gain2xfps.Value = 1.0m;

            form1.numericUpDown_wl_to_show.Maximum = 1;
            form1.numericUpDown_wl_to_show.Minimum = 0;


            cameraParameters.exposureSafeMargin = 0.5m;
            cameraParameters.frameRateHardMax = Decimal.Floor(cameraParameters.acquisitionFrameRate_max);

            form1.numericUpDown_frameRate.ValueChanged -= new System.EventHandler(form1.numericUpDown_frameRate_ValueChanged);
            form1.numericUpDown_frameRate.DecimalPlaces = 0;
            form1.numericUpDown_frameRate.Maximum = cameraParameters.frameRateHardMax;
            form1.numericUpDown_frameRate.Minimum = 5.0m;
            form1.numericUpDown_frameRate.Increment = 1m;
            form1.numericUpDown_frameRate.Value = 10m;
            form1.numericUpDown_frameRate.ValueChanged += new System.EventHandler(form1.numericUpDown_frameRate_ValueChanged);


            form1.numericUpDown_exposureTime.ValueChanged -= new System.EventHandler(form1.numericUpDown_exposureTime_ValueChanged);
            form1.numericUpDown_exposureTime.DecimalPlaces = 1;
            form1.numericUpDown_exposureTime.Maximum = Decimal.Round(1000 / form1.numericUpDown_frameRate.Value - cameraParameters.exposureSafeMargin, 1);
            form1.numericUpDown_exposureTime.Minimum = 0.1m;
            form1.numericUpDown_exposureTime.Increment = 0.1m;
            form1.numericUpDown_exposureTime.Value = Decimal.Round(1000 / form1.numericUpDown_frameRate.Value - cameraParameters.exposureSafeMargin, 1);
            form1.numericUpDown_exposureTime.ValueChanged += new System.EventHandler(form1.numericUpDown_exposureTime_ValueChanged);


            form1.label_gainMin.Text = form1.numericUpDown_gain.Minimum.ToString();
            form1.label_gainMax.Text = form1.numericUpDown_gain.Maximum.ToString();

            form1.label_frameRateMin.Text = form1.numericUpDown_frameRate.Minimum.ToString();
            form1.label_frameRateMax.Text = form1.numericUpDown_frameRate.Maximum.ToString();

            form1.label_exposureTimeMin.Text = form1.numericUpDown_exposureTime.Minimum.ToString();
            form1.label_exposureTimeMax.Text = form1.numericUpDown_exposureTime.Maximum.ToString();



            form1.formSettings.label_xSumMax.Text = cameraParameters.w_max.ToString();
            form1.formSettings.label_ySumMax.Text = cameraParameters.h_max.ToString();

            form1.formSettings.numericUpDown_x.Minimum = cameraParameters.x_min;
            form1.formSettings.numericUpDown_x.Maximum = cameraParameters.x_max;
            form1.formSettings.numericUpDown_x.Increment = cameraParameters.x_inc;
            form1.formSettings.numericUpDown_x.Value = 420m;


            form1.formSettings.numericUpDown_w.Minimum = cameraParameters.w_min;
            form1.formSettings.numericUpDown_w.Maximum = cameraParameters.w_max;
            form1.formSettings.numericUpDown_w.Increment = cameraParameters.w_inc;
            form1.formSettings.numericUpDown_w.Value = 1096;



            form1.formSettings.numericUpDown_y.Minimum = cameraParameters.y_min;
            form1.formSettings.numericUpDown_y.Maximum = cameraParameters.y_max;
            form1.formSettings.numericUpDown_y.Increment = cameraParameters.y_inc;
            form1.formSettings.numericUpDown_y.Value = 208m;


            form1.formSettings.numericUpDown_h.Minimum = cameraParameters.h_min;
            form1.formSettings.numericUpDown_h.Maximum = cameraParameters.h_max;
            form1.formSettings.numericUpDown_h.Increment = cameraParameters.h_inc;
            form1.formSettings.numericUpDown_h.Value = 800m;

            form1.formSettings.numericUpDown_pluxfreq.Minimum = 1m;
            form1.formSettings.numericUpDown_pluxfreq.Maximum = 1000m;
            form1.formSettings.numericUpDown_pluxfreq.Increment = 100m;
            form1.formSettings.numericUpDown_pluxfreq.Value = 1000m;

            form1.formSettings.numericUpDown_pluxShowS.Minimum = 1m;
            form1.formSettings.numericUpDown_pluxShowS.Maximum = 60m;
            form1.formSettings.numericUpDown_pluxShowS.Increment = 1m;
            form1.formSettings.numericUpDown_pluxShowS.Value = 5m;

            form1.pictureBoxWithInterpolationMode1.InterpolationMode = InterpolationMode.NearestNeighbor;
            form1.pictureBoxWithInterpolationMode2.InterpolationMode = InterpolationMode.NearestNeighbor;
            form1.pictureBoxWithInterpolationMode3.InterpolationMode = InterpolationMode.NearestNeighbor;

            form1.pictureBoxWithInterpolationMode1.SizeMode = PictureBoxSizeMode.Zoom;
            form1.pictureBoxWithInterpolationMode2.SizeMode = PictureBoxSizeMode.Zoom;
            form1.pictureBoxWithInterpolationMode3.SizeMode = PictureBoxSizeMode.Zoom;

            form1.pictureBoxWithInterpolationMode2.Visible = false;
            form1.pictureBoxWithInterpolationMode3.Visible = false;

            form1.numericUpDown_subsampling.Minimum = 1m;
            form1.numericUpDown_subsampling.Maximum = 100m;
            form1.numericUpDown_subsampling.Value = 10m;

            form1.numericUpDown_pictureBoxTimeDecimation.Minimum = 1m;
            form1.numericUpDown_pictureBoxTimeDecimation.Maximum = 100m;
            form1.numericUpDown_pictureBoxTimeDecimation.Value = 3m;

            form1.numericUpDown_pictureBoxTimeDecimation_play.Minimum = 1m;
            form1.numericUpDown_pictureBoxTimeDecimation_play.Maximum = 100m;
            form1.numericUpDown_pictureBoxTimeDecimation_play.Value = 3m;


            form1.numericUpDown_bufferSize.Minimum = 100;
            form1.numericUpDown_bufferSize.Maximum = 999999;
            form1.numericUpDown_bufferSize.Value = 500;

            form1.numericUpDown_IR.Minimum = 1;
            form1.numericUpDown_IR.Maximum = 20;
            form1.numericUpDown_IR.Value = 2;

            form1.numericUpDown_red.Minimum = 1;
            form1.numericUpDown_red.Maximum = 20;
            form1.numericUpDown_red.Value = 2;


            form1.formSettings.numericUpDown_bits.Minimum = 8;
            form1.formSettings.numericUpDown_bits.Maximum = 10;
            form1.formSettings.numericUpDown_bits.Increment = 2;
            form1.formSettings.numericUpDown_bits.Value = 8;

            form1.checkBox_exposurTimeMax.Checked = true;

            foreach (Control obj in form1.formSettings.panel1.Controls)
            {
                if (obj.GetType() == typeof(ComboBox))
                {
                    ComboBox obj_converted = (ComboBox)Convert.ChangeType(obj, typeof(ComboBox));
                    obj_converted.Items.AddRange(new object[] { "None", "ECG", "Triger", "Respiration", "Oxi R + IR", "ABP", "ICP", "VBP", "BVP", "EOG", "Other"});
                    obj_converted.SelectedIndexChanged -= form1.formSettings.comboBox_SelectedIndexChanged;
                    obj_converted.Text = "Other";
                    obj_converted.SelectedIndexChanged += form1.formSettings.comboBox_SelectedIndexChanged;
                }
            }

            form1.formSettings.comboBox_pixelClock.SelectedIndexChanged -= form1.formSettings.comboBox_SelectedIndexChanged;
            form1.formSettings.comboBox_pixelClock.Items.AddRange(new object[] { "59", "118", "237", "474" });
            form1.formSettings.comboBox_pixelClock.Text = "118";
            form1.formSettings.comboBox_pixelClock.SelectedIndexChanged += form1.formSettings.comboBox_SelectedIndexChanged;


            form1.numericLeftRight_dotY.NumericUpDown.ValueChanged += form1.numericLeftRight_dotY_ValueChanged;

            form1.numericUpDown_dotX.Minimum = -1000;
            form1.numericUpDown_dotX.Maximum = 1000;

            form1.numericLeftRight_dotY.NumericUpDown.Minimum = -1000;
            form1.numericLeftRight_dotY.NumericUpDown.Maximum = 1000;

            form1.numericUpDown_R.Value = 12;

        }


        public static void SetCharts(Form1 form1)
        {
            var chart_count = 0;
            foreach (Control obj in form1.formSettings.panel1.Controls)
            {
                if (obj.GetType() == typeof(ComboBox))
                {
                    ComboBox obj_converted = (ComboBox)Convert.ChangeType(obj, typeof(ComboBox));

                    if (obj_converted.Text == "None")
                        continue;
                    chart_count++;
                    if (obj_converted.Text == "Oxi R + IR")
                        chart_count++;

                }
            }

            int overlap = (int)form1.formSettings.numericUpDown_chartOverlap.Value;
            int panel_w = form1.panel_plux.Width;
            int panel_h = form1.panel_plux.Height;


            int h = (panel_h + (chart_count - 1) * overlap) / chart_count;
            int w = panel_w;


            int[] positions = new int[chart_count];
            positions[0] = 0;


            int actual_pos = 0;
            for (int i = 1; i <= chart_count; i++)
            {


                
                if (i == 1)
                    chart = form1.chart1;
                if (i == 2)
                    chart = form1.chart2;
                if (i == 3)
                    chart = form1.chart3;
                if (i == 4)
                    chart = form1.chart4;
                if (i == 5)
                    chart = form1.chart5;
                if (i == 6)
                    chart = form1.chart6;
                if (i == 7)
                    chart = form1.chart7;
                if (i == 8)
                    chart = form1.chart8;

                if (chart != null)
                {
                    chart.Hide();
                    chart.Dispose();
                }

                ChartArea chartArea = new ChartArea();
                Legend legend = new Legend();
                Series series = new Series();
                chart = new Chart();
                ((System.ComponentModel.ISupportInitialize)(chart)).BeginInit();
                chartArea.Name = "ChartArea";
                chart.ChartAreas.Add(chartArea);
                legend.Name = "Legend";
                chart.Legends.Add(legend);
                chart.Location = new System.Drawing.Point(0, actual_pos);
                chart.Name = "chart0";
                series.ChartArea = "ChartArea";
                series.ChartType = SeriesChartType.FastLine;
                series.IsVisibleInLegend = false;
                series.Legend = "Legend";
                series.Name = "Series";
                chart.Series.Add(series);
                chart.Size = new System.Drawing.Size(w, h);
                chart.TabIndex = 30;
                chart.Text = "chart0";
                form1.panel_plux.Controls.Add(chart);
                chart.ChartAreas[0].AxisY.IsStartedFromZero = false;

                chart.Series[0].Points.AddXY(0, 1);
                chart.Series[0].Points.AddXY(1, 2);
                chart.Series[0].Points.AddXY(2, 1);
                chart.Series[0].Points.AddXY(3, 2);

                chart.BringToFront();

                
                if (i == 1)
                    form1.chart1 = chart;
                if (i == 2)
                    form1.chart2 = chart;
                if (i == 3)
                    form1.chart3 = chart;
                if (i == 4)
                    form1.chart4 = chart;
                if (i == 5)
                    form1.chart5 = chart;
                if (i == 6)
                    form1.chart6 = chart;
                if (i == 7)
                    form1.chart7 = chart;
                if (i == 8)
                    form1.chart8 = chart;

                actual_pos = actual_pos + h - overlap;

            }





            


        }


    }
}
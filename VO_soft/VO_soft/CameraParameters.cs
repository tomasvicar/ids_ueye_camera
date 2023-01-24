using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VO_soft
{
    public class CameraParameters
    {
        public decimal x_min { get; set; }
        public decimal y_min { get; internal set; }
        public decimal w_min { get; internal set; }
        public decimal h_min { get; internal set; }
        public decimal x_max { get; internal set; }
        public decimal y_max { get; internal set; }
        public decimal w_max { get; internal set; }
        public decimal h_max { get; internal set; }
        public decimal x_inc { get; internal set; }
        public decimal y_inc { get; internal set; }
        public decimal w_inc { get; internal set; }
        public decimal h_inc { get; internal set; }
        public decimal deviceClockFrequency_min { get; internal set; }
        public decimal deviceClockFrequency_max { get; internal set; }
        public decimal deviceClockFrequency_inc { get; internal set; }
        public decimal acquisitionFrameRate_min { get; internal set; }
        public decimal acquisitionFrameRate_max { get; internal set; }
        public decimal acquisitionFrameRate_inc { get; internal set; }
        public decimal exposureTime_min { get; internal set; }
        public decimal exposureTime_max { get; internal set; }
        public decimal exposureTime_inc { get; internal set; }
        public decimal gain_min { get; internal set; }
        public decimal gain_max { get; internal set; }
        public decimal gain_inc { get; internal set; }
        public decimal c { get; internal set; }
        public decimal frameRateHardMax { get; internal set; }
        public decimal exposureSafeMargin { get; internal set; }

        public CameraParameters()
        {
        }

    }
}

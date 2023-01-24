using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VO_soft
{
    class ImageUpdater
    {
        private Image previousImage;

        internal void updateImage(Bitmap image,uint counter, Form1 form1)
        {

            previousImage = form1.pictureBoxWithInterpolationMode1.Image;
            form1.pictureBoxWithInterpolationMode1.Image = image;

            if (previousImage != null)
            {
                previousImage.Dispose();
            }



        }
    }
}

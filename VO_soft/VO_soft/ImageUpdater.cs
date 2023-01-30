using Accord.Imaging.Filters;
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
        Bitmap previousImageBitmap1;
        Bitmap previousImageBitmap2;
        Bitmap previousImageBitmap3;
        private Bitmap previousImageBitmap0;

        internal void updateImage(Bitmap image, uint counter, Form1 form1)
        {
            if (form1.formSettings.checkBox_visible_two_wl.Checked)
            {
                if (form1.checkBox_show_1_wl.Checked)
                {
                    if ((int)counter % 2 == form1.numericUpDown_wl_to_show.Value)
                    {
                        previousImageBitmap1 = showImgpictureBox(image, counter, form1, form1.pictureBoxWithInterpolationMode1, previousImageBitmap1);
                    }
                }
                else
                {
                    if ((int)counter % 2 == 0)
                    {
                        previousImageBitmap2 = showImgpictureBox(image, counter, form1, form1.pictureBoxWithInterpolationMode2, previousImageBitmap2);
                    }
                    else
                    {
                        previousImageBitmap3 = showImgpictureBox(image, counter, form1, form1.pictureBoxWithInterpolationMode3, previousImageBitmap3);
                    }

                }
                return;
            }

            previousImageBitmap0 = showImgpictureBox(image, counter, form1, form1.pictureBoxWithInterpolationMode1, previousImageBitmap0);
            return;

        }

        private Bitmap showImgpictureBox(Bitmap image, uint counter, Form1 form1, PictureBoxWithInterpolationMode pictureBox, Bitmap previousImageBitmap)
        {
            Image previousImage = (Image)pictureBox.Image;

            if (form1.formSettings.checkBox_visible_two_wl.Checked)
            {
                if (form1.checkBox_sum2frames.Checked)
                {

                    if (previousImageBitmap == null)
                        previousImageBitmap = (Bitmap)image.Clone();


                    Add filter = new Add(image);
                    Bitmap resultImg = filter.Apply(previousImageBitmap);

                    pictureBox.Image = resultImg;

                    if (previousImage != null)
                    {
                        previousImage.Dispose();
                    }

                    previousImageBitmap.Dispose();
                    previousImageBitmap = (Bitmap)image.Clone();
                    return previousImageBitmap;
                }
            }


            pictureBox.Image = image;

            if (previousImage != null)
            {
                previousImage.Dispose();
            }
            return null;
        }



    }
}

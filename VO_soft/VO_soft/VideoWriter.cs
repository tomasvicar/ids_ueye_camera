using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VO_soft
{
    class VideoWriter
    {

        string filename;
        string ffprobe;
        string ffmpeg;
        private Process process;
        private Stream ffmpegIn;

        public VideoWriter()
        {
            

            ffprobe = "ffprobe.exe";
            if (!File.Exists(ffprobe))
            {
                MessageBox.Show("ffprobe not avaliable", "ffprobe  error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            ffmpeg = "ffmpeg.exe";
            if (!File.Exists(ffmpeg))
            {
                MessageBox.Show("FFMPEG not avaliable", "FFMPEG error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        public void openVideo(string filename, int fps, string ffmpegPixelFormat_in, string ffmpegPixelFormat_out, int width, int height)
        {
            this.filename = filename;
            string arguments = " -y -framerate " + fps.ToString() + " -s " + width.ToString() + "x" + height.ToString() + " -pixel_format " + ffmpegPixelFormat_in + " -f rawvideo -i pipe: -vcodec ffv1 -pix_fmt " + ffmpegPixelFormat_out + " " + filename;

            process = new Process
            {
                StartInfo =
                {
                    FileName = ffmpeg,
                    Arguments = arguments,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardInput = true,
                    RedirectStandardError = true,
                },
            };
            process.ErrorDataReceived += (sender, eventArgs) => Console.Write(eventArgs.Data);
            process.Start();
            process.BeginErrorReadLine();


            ffmpegIn = process.StandardInput.BaseStream;

        }
        public void writeFrame(IntPtr ptr, int len)
        {

            byte[] arr = new byte[len];
            Marshal.Copy((IntPtr)ptr, arr, 0, len);
            ffmpegIn.Write(arr, 0, len);
        }

        public void closeVideo()
        {
            ffmpegIn.Flush();
            ffmpegIn.Close();

            process.WaitForExit();
            process.Dispose();

        }


    }
}

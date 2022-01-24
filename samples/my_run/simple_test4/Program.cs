using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using peak;

namespace simple_test4
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            
            try
            {
                Library.Initialize();
                var deviceManager = DeviceManager.Instance();
                deviceManager.Update();
                var deviceDescriptors = deviceManager.Devices();
                foreach (var deviceDescriptor in deviceDescriptors)
                {
                    Console.WriteLine(deviceDescriptor.DisplayName());
                }
            }
            catch (System.ApplicationException e)
            {
                Console.WriteLine("Exception: " + e.Message);
            }
        }
    }
}

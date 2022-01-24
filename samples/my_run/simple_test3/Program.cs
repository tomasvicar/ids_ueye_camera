using System;
using peak;

namespace simple_test3
{
    class Program
    {
        static void Main(string[] args)
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

using System;
using peak;

namespace simple_test
{
    class Program
    {
        static void Main(string[] args)
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
    }
}

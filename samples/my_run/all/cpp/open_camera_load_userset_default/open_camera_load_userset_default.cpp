/*!
 * \file    open_camera_load_userset_default.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-04
 * \since   1.2.2
 *
 * \brief   This application demonstrates how to use the device manager to open a camera
 *          and how to load the UserSet Default to restore default camera configuration
 *
 * \version 1.0.0
 *
 * Copyright (C) 2019 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#define VERSION "1.0.0"

#include <cstddef>
#include <iostream>

#include <peak/peak.hpp>


/*! \bief Wait for enter function
 *
 * The function waits for the user pressing the enter key.
 *
 * This function is called from main() whenever the program exits,
 * either in consequence of an error or after normal termination.
 */
void wait_for_enter();


int main()
{
    std::cout << "IDS peak API \"open_camera_load_userset_default\" Sample v" << VERSION << std::endl;

    // initialize peak library
    peak::Library::Initialize();

    // create a device manager object
    auto& deviceManager = peak::DeviceManager::Instance();

    try
    {
        // update the deviceManager
        deviceManager.Update();

        // exit program if no device was found
        if (deviceManager.Devices().empty())
        {
            std::cout << "No device found. Exiting program." << std::endl << std::endl;
            wait_for_enter();
            // close peak library
            peak::Library::Close();
            return 0;
        }

        // list all available devices
        size_t i = 0;
        std::cout << "Devices available: " << std::endl;
        for (const auto& deviceDescriptor : deviceManager.Devices())
        {
            std::cout << i << ": " << deviceDescriptor->ModelName() << " ("
                      << deviceDescriptor->ParentInterface()->DisplayName() << "; "
                      << deviceDescriptor->ParentInterface()->ParentSystem()->DisplayName() << " v."
                      << deviceDescriptor->ParentInterface()->ParentSystem()->Version() << ")" << std::endl;
            ++i;
        }

        // select a device to open
        size_t selectedDevice = 0;

        if (deviceManager.Devices().size() > 1)
        {
            // if multiple devices have been found, select a device to open via user input
            std::cout << std::endl << "Select device to open: ";
            std::cin >> selectedDevice;
        }
        
        // open the selected device
        auto device = deviceManager.Devices().at(selectedDevice)->OpenDevice(peak::core::DeviceAccessType::Control);

        // get the remote device node map
        auto nodeMapRemoteDevice = device->RemoteDevice()->NodeMaps().at(0);

        try
        {
            // print model name, not knowing if device has the node "DeviceModelName"
            std::cout
                << "Model Name: "
                << nodeMapRemoteDevice->FindNode<peak::core::nodes::StringNode>("DeviceModelName")->Value()
                << std::endl;
        }
        catch (const std::exception&)
        {
            // if "DeviceModelName" is not a valid node name, do error handling here...
            std::cout << "Model Name: (unknown)" << std::endl;
        }

        try
        {
            // print user ID, not knowing if device has the node "DeviceUserID"
            std::cout << "User ID: "
                      << nodeMapRemoteDevice->FindNode<peak::core::nodes::StringNode>("DeviceUserID")->Value()
                      << std::endl;
        }
        catch (const std::exception&)
        {
            // if "DeviceUserID" is not a valid node name, do error handling here...
            std::cout << "User ID: (unknown)" << std::endl;
        }

        try
        {
            // Loading UserSet Default
            nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                ->SetCurrentEntry("Default");
            nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
            nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
            std::cout << "Successfully loaded UserSet Default." << std::endl;
        }
        catch (const std::exception& e)
        {
            // Error handling
            std::cout << "Loading UserSet Default failed." << std::endl;
            std::cout << "EXCEPTION: " << e.what() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
    }

    wait_for_enter();
    // close peak library
    peak::Library::Close();
    return 0;
}


void wait_for_enter()
{
    std::cout << std::endl;
#if defined(WIN32)
    system("pause");
#elif defined(__linux__)
    std::cout << "Press enter to exit." << std::endl;
    system("read _");
#else
#    warning("Operating system not implemented!")
#endif
}

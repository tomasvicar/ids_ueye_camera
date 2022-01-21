/*!
 * \file    get_first_pixel.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-05
 * \since   1.0.0
 *
 * \brief   This application demonstrates how to use the device manager to open a camera
 *          and to display the first pixel value using the IDS peak IPL
 *
 * \version 1.2.0
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

#define VERSION "1.2.0"

#include <cstdint>
#include <iostream>

#include <peak/converters/peak_buffer_converter_ipl.hpp>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>


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
    std::cout << "IDS peak API \"get_first_pixel\" Sample v" << VERSION << std::endl;

    // initialize peak library
    peak::Library::Initialize();

    // create a camera manager object
    auto& deviceManager = peak::DeviceManager::Instance();

    try
    {
        // update the cameraManager
        deviceManager.Update();

        // exit program if no camera was found
        if (deviceManager.Devices().empty())
        {
            std::cout << "No camera found. Exiting program." << std::endl << std::endl;
            wait_for_enter();
            // close library before exiting program
            peak::Library::Close();
            return 0;
        }

        // list all available devices
        uint64_t i = 0;
        std::cout << "Devices available: " << std::endl;
        for (const auto& deviceDescriptor : deviceManager.Devices())
        {
            std::cout << i << ": " << deviceDescriptor->ModelName() << " ("
                      << deviceDescriptor->ParentInterface()->DisplayName() << "; "
                      << deviceDescriptor->ParentInterface()->ParentSystem()->DisplayName() << " v."
                      << deviceDescriptor->ParentInterface()->ParentSystem()->Version() << ")" << std::endl;
            ++i;
        }

        // select a camera to open
        size_t selectedDevice = 0;
        // select a camera to open via user input or remove these lines to always open the first available camera
        std::cout << std::endl << "Select camera to open: ";
        std::cin >> selectedDevice;

        // open the selected camera
        auto device = deviceManager.Devices().at(selectedDevice)->OpenDevice(peak::core::DeviceAccessType::Control);
        // get the remote device node map
        auto nodeMapRemoteDevice = device->RemoteDevice()->NodeMaps().at(0);
        // open the data stream
        auto dataStream = device->DataStreams().at(0)->OpenDataStream();

        // general preparations for untriggered continuous image acquisition
        // load the default user set, if available, to reset the device to a defined parameter set
        try
        {
            nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                ->SetCurrentEntry("Default");
            nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
            // wait until the UserSetLoad command has been finished
            nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
        }
        catch (const std::exception&)
        {
            // UserSet is not available, try to disable ExposureStart or FrameStart trigger manually
            std::cout << "Failed to load UserSet Default. Manual freerun configuration." << std::endl;

            try
            {
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("Off");
            }
            catch (const std::exception&)
            {
                try
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                        ->SetCurrentEntry("FrameStart");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                        ->SetCurrentEntry("Off");
                }
                catch (const std::exception&)
                {
                    // There is no known trigger available, continue anyway.
                }
            }
        }

        // allocate and announce image buffers
        auto payloadSize = nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();
        auto bufferCountMax = dataStream->NumBuffersAnnouncedMinRequired();
        for (uint64_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        {
            auto buffer = dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
            dataStream->QueueBuffer(buffer);
        }

        // set a frame rate to 10fps (or max value) since some of the trigger cases require a defined frame rate
        auto frameRateMax = nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                                ->Maximum();
        nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
            ->SetValue(std::min(10.0, frameRateMax));

        // define the number of images to acquire
        uint64_t imageCountMax = 10;
        // get number of images to acquire via user input or remove these lines to always acquire 10 images
        std::cout << std::endl << "Enter number of images to acquire: ";
        std::cin >> imageCountMax;

        // Lock critical features to prevent them from changing during acquisition
        nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);

        // start acquisition
        dataStream->StartAcquisition(peak::core::AcquisitionStartMode::Default, imageCountMax);
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        // process the acquired images
        uint64_t imageCount = 0;
        std::cout << std::endl << "First pixel value of each image: " << std::endl;
        while (imageCount < imageCountMax)
        {
            // get buffer from datastream and create IDS peak IPL image from it
            auto buffer = dataStream->WaitForFinishedBuffer(5000);
            auto image = peak::BufferTo<peak::ipl::Image>(buffer);

            // output first pixel value
            std::cout << static_cast<uint16_t>(*image.PixelPointer(0, 0)) << " ";

            // queue buffer
            dataStream->QueueBuffer(buffer);
            ++imageCount;
        }
        std::cout << std::endl << std::endl;

        // stop acquistion of camera
        try
        {
            dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
        }
        catch (const std::exception&)
        {
            // Some transport layers need no explicit acquisition stop of the datastream when starting its
            // acquisition with a finite number of images. Ignoring Errors due to that TL behavior.

            std::cout << "WARNING: Ignoring that TL failed to stop acquisition on datastream." << std::endl;
        }
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();

        // Unlock parameters after acquisition stop
        nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);

        // flush and revoke all buffers
        dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
        for (const auto& buffer : dataStream->AnnouncedBuffers())
        {
            dataStream->RevokeBuffer(buffer);
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
    }

    wait_for_enter();
    // close library before exiting program
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

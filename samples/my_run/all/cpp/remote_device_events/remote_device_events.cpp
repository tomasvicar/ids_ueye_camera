/*!
 * \file    remote_device_events.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-24
 * \since   1.2.0
 *
 * \brief   This application demonstrates how to register and receive Remote Device Events
 *          via the IDS peak API
 *
 * \version 1.0.1
 *
 * Copyright (C) 2021, IDS Imaging Development Systems GmbH.
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

#define VERSION "1.0.1"

#include <peak/peak.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>


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
    std::cout << "IDS peak API \"remote_device_events\" Sample v" << VERSION << std::endl;

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
        auto device =
            deviceManager.Devices().at(selectedDevice)->OpenDevice(peak::core::DeviceAccessType::Control);
        // get the remote device node map
        auto nodeMapRemoteDevice = device->RemoteDevice()->NodeMaps().at(0);
        // open the data stream
        auto dataStream = device->DataStreams().at(0)->OpenDataStream();

        std::unique_ptr<peak::core::EventController> eventController;
        try
        {
            // enable the exposure start event
            eventController = device->EnableEvents(peak::core::EventType::RemoteDevice);
        }
        catch (const peak::core::NotAvailableException&)
        {
            std::cout << "The camera doesn't support RemoteDeviceEvents. Exiting program." << std::endl << std::endl;
            wait_for_enter();
            peak::Library::Close();
            return 0;
        }
        catch (const peak::core::NotImplementedException&)
        {
            std::cout << "The transport layer doesn't support RemoteDeviceEvents. Exiting program." << std::endl
                      << std::endl;
            wait_for_enter();
            peak::Library::Close();
            return 0;
        }

        try
        {
            nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("EventSelector")
                ->SetCurrentEntry("ExposureStart");
        }
        catch (const peak::core::NotFoundException&)
        {
            std::cout << "The camera doesn't support RemoteDeviceEvents. Exiting program." << std::endl
                      << std::endl;
            wait_for_enter();
            peak::Library::Close();
            return 0;
        }
        catch (const peak::core::BadAccessException&)
        {
            std::cout << "The camera doesn't support RemoteDeviceEvent \"ExposureStart\". Exiting program." << std::endl
                      << std::endl;
            wait_for_enter();
            peak::Library::Close();
            return 0;
        }

        nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("EventNotification")
            ->SetCurrentEntry("On");

        // allocate and announce image buffers
        auto payloadSize = nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                               ->Value();
        auto bufferCountMax = dataStream->NumBuffersAnnouncedMinRequired();
        for (uint64_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        {
            auto buffer = dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
            dataStream->QueueBuffer(buffer);
        }

        // prepare for untriggered continuous image acquisition
        nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
            ->SetCurrentEntry("ExposureStart");
        nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
            ->SetCurrentEntry("Off");

        // limit the acquisition framerate because buffers can be skipped in this sample if the next image is acquired
        // before the event was delivered
        nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->SetValue(5);

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
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();

        // process the acquired images
        uint64_t imageCount = 0;

        while (imageCount < imageCountMax)
        {
            // wait for buffer
            auto buffer = dataStream->WaitForFinishedBuffer(5000);

            // wait for the event after buffer was received
            auto event = eventController->WaitForEvent(5000);

            // update the nodemap with the received event
            nodeMapRemoteDevice->UpdateEventNodes(event);

            // get the frameID and timestamp of the buffer
            auto bufferFrameID = buffer->FrameID();
            auto bufferTimestamp = buffer->Timestamp_ns();

            // get the frameID and timestamp from the event
            auto eventFrameID = nodeMapRemoteDevice
                                    ->FindNode<peak::core::nodes::IntegerNode>("EventExposureStartFrameID")
                                    ->Value();
            auto eventTimeStamp = nodeMapRemoteDevice
                                      ->FindNode<peak::core::nodes::IntegerNode>("EventExposureStartTimestamp")
                                      ->Value();

            // output the frameID and timestamp of the buffer an the event
            std::cout << "----------------------------------------------------------" << std::endl;
            std::cout << "[Frame Data] Frame ID: " << bufferFrameID << " | Timestamp: " << bufferTimestamp << std::endl;
            std::cout << "[Event Data] Frame ID: " << eventFrameID << " | Timestamp: " << eventTimeStamp << std::endl;

            // queue buffer
            dataStream->QueueBuffer(buffer);
            ++imageCount;
        }

        // stop acquistion of camera
        dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
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

    // close library before exiting program
    peak::Library::Close();

    wait_for_enter();

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

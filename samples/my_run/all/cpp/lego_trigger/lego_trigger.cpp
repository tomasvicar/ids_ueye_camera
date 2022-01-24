/*!
 * \file    lego_trigger.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-25
 * \since   1.0.0
 *
 * \brief   This application demonstrates how to use the device manager to open a camera,
 *          how to set parameters for triggering and how to display the first pixel value
 *          using the IDS peak IPL.
 *
 * \version 1.2.2
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

#define VERSION "1.2.2"

#include <cstdint>
#include <iostream>
#include <math.h>
#include <thread>

#include <peak_ipl/peak_ipl.hpp>
#include <peak/converters/peak_buffer_converter_ipl.hpp>
#include <peak/peak.hpp>


/*! \bief Wait for enter function
 *
 * The function waits for the user pressing the enter key.
 *
 * This function is called from main() whenever the program exits,
 * either in consequence of an error or after normal termination.
 */
void wait_for_enter();

/*! \bief Load UserSet Default function
 *
 * The function loads the UserSet Default.
 *
 * This function is called from main() whenever the program needs
 * to load UserSet Default, either on program start or in consequence
 * of an error or after normal termination.
 */
void load_userset_default(std::shared_ptr<peak::core::NodeMap> nodeMapRemoteDevice);

void SoftwareTriggerWorker(std::shared_ptr<bool> triggerActive,
    std::shared_ptr<peak::core::NodeMap> nodeMapRemoteDevice, std::string triggerTypeStart,
    std::string triggerTypeEnd, uint64_t sleep_ms, uint64_t sleep2_ms)
{
    while (*triggerActive)
    {
        try
        {
            // do the start trigger
            if (triggerTypeStart == "Counter0")
            {
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                    ->SetCurrentEntry("Counter0");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("CounterReset")->Execute();
            }
            else if (triggerTypeStart == "Timer0")
            {
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TimerReset")->Execute();
            }
            else
            {
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry(triggerTypeStart);
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TriggerSoftware")->Execute();
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TriggerSoftware")
                    ->WaitUntilDone();
            }

            // indicate the start trigger by printing a '.'
            std::cout << ". ";

            // wait for sleep_ms, but at least for two frames before proceeding
            auto const frameRate =
                nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Value();
            auto const frameTime_ms = static_cast<uint64_t>(std::ceil((1.0 / frameRate) * 1000.0)) * 2;
            if (sleep_ms == 0)
            {
                // random exposure time (ms) for trigger case 'e'
                uint64_t exposureTime_ms = rand() % 100 + 1;
                std::this_thread::sleep_for(std::chrono::milliseconds(std::max(frameTime_ms, exposureTime_ms)));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(std::max(sleep_ms, frameTime_ms)));
            }

            // if an end trigger is defined, do the end trigger (trigger cases 'a' or 'e')
            if (!triggerTypeEnd.empty())
            {
                // in case of AcquisitionEnd also set triggerEnabled to false to stop the image processing loop in the
                // main thread
                if (triggerTypeEnd == "AcquisitionEnd")
                {
                    *triggerActive = false;


                    // allow the image processing loop in main thread to terminate
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                // indicate the end trigger by printing an 'x'
                std::cout << "x ";
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry(triggerTypeEnd);
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TriggerSoftware")->Execute();
                nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TriggerSoftware")
                    ->WaitUntilDone();
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "EXCEPTION: " << e.what() << std::endl;
        }

        // wait before proceeding (trigger case 'e')
        if (sleep2_ms != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep2_ms));
        }
    }
    return;
}

int main()
{
    std::cout << "IDS peak API \"lego_trigger\" Sample v" << VERSION << std::endl;

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

        // select a device to open
        size_t selectedDevice = 0;
        // select a device to open via user input or remove these lines to always open the first available device
        std::cout << std::endl << "Select device to open: ";
        std::cin >> selectedDevice;

        // open the selected device
        auto device =
            deviceManager.Devices().at(selectedDevice)->OpenDevice(peak::core::DeviceAccessType::Control);
        // get the remote device node map
        auto nodeMapRemoteDevice = device->RemoteDevice()->NodeMaps().at(0);
        // open the data stream
        auto dataStream = device->DataStreams().at(0)->OpenDataStream();

        // general preparations for triggered image acquisition
        // load the default user set, if available, to reset the device to a defined parameter set
        load_userset_default(nodeMapRemoteDevice);

        // allocate and announce image buffers
        auto payloadSize = nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                               ->Value();
        auto bufferCountMax = dataStream->NumBuffersAnnouncedMinRequired();
        for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        {
            auto buffer = dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
            dataStream->QueueBuffer(buffer);
        }

        // Disable trigger temporarily if already selected
        if (nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->CurrentEntry()
                ->StringValue()
            == "On")
        {
            nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("Off");
        }
        // set a frame rate to 10fps (or max value) since some of the trigger cases require a defined frame rate
        auto frameRateMax = nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                                ->Maximum();
        nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
            ->SetValue(std::min(10.0, frameRateMax));

        // user input to choose trigger case
        char triggerCase = ' ';
        std::vector<char> triggerCases = { 'b', 'd', 's', 'a', 'e', 'n', 'm', 'p', 'q' };
        while (std::find(triggerCases.begin(), triggerCases.end(), triggerCase) >= triggerCases.end())
        {
            std::cout << std::endl << "Choose one of the following trigger cases: " << std::endl;
            std::cout << "'b': \t basic, non-cyclic image acquisition" << std::endl;
            std::cout << "'d': \t delayed, non-cyclic image acquisition" << std::endl;
            std::cout << "'s': \t scaled, non-cyclic image acquisition with trigger divider" << std::endl;
            std::cout << "'a': \t acquisition period trigger" << std::endl;
            std::cout << "'e': \t triggered exposure time" << std::endl;
            std::cout << "'n': \t n images in fastest possible counted image sequence" << std::endl;
            std::cout << "'m': \t m images in counted image sequence with fixed frame rate" << std::endl;
            std::cout << "'p': \t p images in fastest possible time-controlled image sequence" << std::endl;
            std::cout << "'q': \t q images in time-controlled image sequence with fixed frame rate" << std::endl;
            std::cin >> triggerCase;
        }

        // user input to choose trigger source
        char triggerSource = ' ';
        std::vector<char> triggerSources = { 's', '0', '2', '3' };
        while (std::find(triggerSources.begin(), triggerSources.end(), triggerSource) >= triggerSources.end())
        {
            std::cout << std::endl << "Choose one of the following trigger sources: " << std::endl;
            std::cout << "'s': \t software trigger" << std::endl;
            std::cout << "'0': \t hardware trigger on line 0 (opto IN)" << std::endl;
            std::cout << "'2': \t hardware trigger on line 2 (GPIO 1)" << std::endl;
            std::cout << "'3': \t hardware trigger on line 3 (GPIO 2)" << std::endl;
            std::cin >> triggerSource;
        }

        std::string triggerTypeStart = "";
        std::string triggerTypeEnd = "";

        // do the parameter settings for the different trigger cases
        try
        {
            switch (triggerCase)
            {
            case 'b': // basic, non-cyclic image acquisition
                std::cout << "Parametrizing for trigger case 'basic'" << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeStart = "ExposureStart";
                break;
            case 'd': // delayed, non-cyclic image acquisition
                std::cout << "Parametrizing for trigger case 'delayed'" << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("TriggerDelay")
                    ->SetValue(100000.0);
                triggerTypeStart = "ExposureStart";
                break;
            case 's': // scaled, non-cyclic image acquisition with trigger divider
                std::cout << "Parametrizing for trigger case 'scaled'" << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TriggerDivider")->SetValue(3);
                triggerTypeStart = "ExposureStart";
                break;
            case 'a': // acquisition period trigger
                std::cout << "Parametrizing for trigger case 'acquisition period'" << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeStart = "AcquisitionStart";
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionEnd");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeEnd = "AcquisitionEnd";
                break;
            case 'e': // triggered exposure time
                std::cout << "Parametrizing for trigger case 'triggered exposure time'" << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeStart = "ExposureStart";
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureEnd");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeEnd = "ExposureEnd";
                break;
            case 'n': // n images in fastest possible counted image sequence
                std::cout << "Parametrizing for trigger case 'n images in fastest possible counted image sequence'"
                          << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                    ->SetCurrentEntry("Counter0Active");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                    ->SetCurrentEntry("LevelHigh");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                    ->SetCurrentEntry("Counter0");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterEventSource")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterEventActivation")
                    ->SetCurrentEntry("RisingEdge");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("CounterDuration")->SetValue(3);
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterResetSource")
                    ->SetCurrentEntry("CounterTrigger");
                triggerTypeStart = "Counter0";
                break;
            case 'm': // m images in counted image sequence with fixed frame rate
                std::cout << "Parametrizing for trigger case 'm images in counted image sequence with fixed frame rate'"
                          << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeStart = "AcquisitionStart";
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionEnd");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                    ->SetCurrentEntry("Counter0End");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                    ->SetCurrentEntry("Counter0");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterTriggerSource")
                    ->SetCurrentEntry("AcquisitionStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterEventSource")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("CounterDuration")->SetValue(3);
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterResetSource")
                    ->SetCurrentEntry("CounterTrigger");
                break;
            case 'p': // p images in fastest possible time-controlled image sequence
                std::cout
                    << "Parametrizing for trigger case 'p images in fastest possible time-controlled image sequence'"
                    << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("ExposureStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                    ->SetCurrentEntry("Timer0Active");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                    ->SetCurrentEntry("LevelHigh");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerSelector")
                    ->SetCurrentEntry("Timer0");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("TimerDuration")
                    ->SetValue(500000.0);
                triggerTypeStart = "Timer0";
                break;
            case 'q': // q images in time-controlled image sequence with fixed frame rate
                std::cout << "Parametrizing for trigger case 'q images in time-controlled image sequence with fixed "
                             "frame rate'"
                          << std::endl;
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionStart");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                triggerTypeStart = "AcquisitionStart";
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                    ->SetCurrentEntry("AcquisitionEnd");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                    ->SetCurrentEntry("On");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                    ->SetCurrentEntry("Timer0End");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerSelector")
                    ->SetCurrentEntry("Timer0");
                nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("TimerDuration")
                    ->SetValue(500000.0);
                nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerTriggerSource")
                    ->SetCurrentEntry("AcquisitionStart");
                break;
            }

            switch (triggerSource)
            {
            case 's': // software trigger
                std::cout << "Parametrizing for software trigger " << std::endl;
                if (triggerTypeStart == "Counter0")
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                        ->SetCurrentEntry("Counter0");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterResetSource")
                        ->SetCurrentEntry("Off");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterTriggerSource")
                        ->SetCurrentEntry("Off");
                }
                else if (triggerTypeStart == "Timer0")
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerSelector")
                        ->SetCurrentEntry("Timer0");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerTriggerSource")
                        ->SetCurrentEntry("Off");
                }
                else
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                        ->SetCurrentEntry(triggerTypeStart);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                        ->SetCurrentEntry("Software");
                }
                if (!triggerTypeEnd.empty())
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                        ->SetCurrentEntry(triggerTypeEnd);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                        ->SetCurrentEntry("Software");
                }
                break;
            default:
                std::string lineIn = "Line";
                lineIn.push_back(triggerSource);
                std::cout << "Parametrizing for hardware trigger on " << lineIn << std::endl;
                if (triggerTypeStart == "Counter0")
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                        ->SetCurrentEntry("Counter0");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterTriggerSource")
                        ->SetCurrentEntry(lineIn);
                    nodeMapRemoteDevice
                        ->FindNode<peak::core::nodes::EnumerationNode>("CounterTriggerActivation")
                        ->SetCurrentEntry("RisingEdge");
                }
                else if (triggerTypeStart == "Timer0")
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerSelector")
                        ->SetCurrentEntry("Timer0");
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerTriggerSource")
                        ->SetCurrentEntry(lineIn);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TimerTriggerActivation")
                        ->SetCurrentEntry("RisingEdge");
                }
                else
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                        ->SetCurrentEntry(triggerTypeStart);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                        ->SetCurrentEntry(lineIn);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                        ->SetCurrentEntry("RisingEdge");
                }
                if (!triggerTypeEnd.empty())
                {
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                        ->SetCurrentEntry(triggerTypeEnd);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                        ->SetCurrentEntry(lineIn);
                    nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                        ->SetCurrentEntry("FallingEdge");
                }
                break;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << std::endl;
            std::cout << "EXCEPTION: " << e.what() << std::endl << std::endl;

            std::cout << "Failed to set parameters for selected trigger case '" << triggerCase
                      << "' with trigger source '" << triggerSource << "'" << std::endl;
            std::cout << "Most likely the camera doesn't support the selected trigger settings." << std::endl;

            // restore defined camera configuration by loading UserSet Default
            std::cout << "Loading UserSet Default to restore consistent camera configuration." << std::endl;
            load_userset_default(nodeMapRemoteDevice);

            wait_for_enter();
            // close library before exiting program
            peak::Library::Close();
            return 0;
        }


        // define stop condition for the program (number of images to acquire or duration of the acquisition period)
        uint64_t imageCountMax = 10000;
        uint64_t acquisitionTimeMax_s = 10;
        switch (triggerCase)
        {
        case 'a':
            // get number of seconds for acquisition duration via user input
            std::cout << std::endl << "Enter number of seconds for acquisition duration: ";
            std::cin >> acquisitionTimeMax_s;
            // imageCountMax should be higher than acquisitionTimeMax * frameRate
            imageCountMax = acquisitionTimeMax_s * 20;
            break;
        default:
            // get number of images to acquire via user input
            std::cout << std::endl << "Enter number of images to acquire: ";
            std::cin >> imageCountMax;
            break;
        }

        // Lock critical features to prevent them from changing during acquisition
        nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);

        // start acquisition
        dataStream->StartAcquisition();
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();

        std::thread t1;
        auto triggerActive = std::make_shared<bool>(true);
        // if software trigger is enabled, start a separate thread to do the triggering
        if (triggerSource == 's')
        {
            switch (triggerCase)
            {
            case 'a':
                t1 = std::thread(SoftwareTriggerWorker, triggerActive, nodeMapRemoteDevice, triggerTypeStart,
                    triggerTypeEnd, acquisitionTimeMax_s * 1000, 0);
                break;
            case 'e':
                t1 = std::thread(SoftwareTriggerWorker, triggerActive, nodeMapRemoteDevice, triggerTypeStart,
                    triggerTypeEnd, 0, 500);
                break;
            case 'n':
            case 'm':
            case 'p':
            case 'q':
                t1 = std::thread(SoftwareTriggerWorker, triggerActive, nodeMapRemoteDevice, triggerTypeStart,
                    triggerTypeEnd, 1000, 0);
                break;
            default:
                t1 = std::thread(
                    SoftwareTriggerWorker, triggerActive, nodeMapRemoteDevice, triggerTypeStart, "", 100, 0);
                break;
            }
        }

        // process the acquired images
        uint64_t imageCount = 0;
        std::cout << std::endl << "First pixel value of each image: " << std::endl;
        while ((imageCount < imageCountMax) && (*triggerActive))
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

        // stop software trigger task
        *triggerActive = false;
        if (t1.joinable())
        {
            t1.join();
        }
        // stop acquistion of device
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->WaitUntilDone();
        dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);

        // Unlock parameters after acquisition stop
        nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);

        // flush and revoke all buffers
        dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
        for (const auto& buffer : dataStream->AnnouncedBuffers())
        {
            dataStream->RevokeBuffer(buffer);
        }

        // ask, if camera configuration should be kept or UserSet Default should be loaded
        std::cout << "Closing the application.\n"
                     "Do you want to keep the trigger configuration or to restore default parameters (UserSet Default)."
                  << std::endl;
        std::cout << "'y': \t Keep trigger configuration" << std::endl;
        std::cout << "'n': \t Do not keep trigger configuration, restore default parameters. (or any other key)"
                  << std::endl;
        char selection = ' ';
        std::cin >> selection;

        switch (selection)
        {
        case 'y':
        case 'Y':
            std::cout << "Parameters will be kept." << std::endl;
            break;
        default:
            std::cout << "Restoring default parameters (load UserSet Default)." << std::endl;
            load_userset_default(nodeMapRemoteDevice);
            break;
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


void load_userset_default(std::shared_ptr<peak::core::NodeMap> nodeMapRemoteDevice)
{
    try
    {
        nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
            ->SetCurrentEntry("Default");
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
        // wait until the UserSetLoad command has been finished
        nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
    }
    catch (const peak::core::NotFoundException&)
    {
        // UserSet is not available
        std::cout << "WARNING: Failed to load UserSet Default." << std::endl;
    }
}

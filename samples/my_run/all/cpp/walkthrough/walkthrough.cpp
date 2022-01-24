/*!
 * \file   walkthrough.cpp
 * \author IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \brief  This application demonstrates how to operate a GenICam device by the use of
 *         the IDS peak API
 *
 * \version 1.1.1
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

#define VERSION "1.1.1"

#include <peak/peak.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/* The following values can be specified or modified by command line arguments. */
std::string cti_selection;
uint32_t num_frames_to_acquire = 100;
bool no_key_for_exit = false;
bool print_help_text_and_exit = false;
bool print_version_and_exit = false;

/*! \bief Wait for enter function
 *
 * The function waits for the user pressing the enter key, unless the command line option --nokey is specified.
 *
 * This function is called from main() whenever the program exits,
 * either in consequence of an error or after normal termination.
 */
void wait_for_enter();

/*! \brief Parse command line for program arguments.
 *
 * The program can be called with none or with any combination of these arguments:
 * -'--cti filepath'  Specify the full path to a *.cti file to be instrumented.
 * -'--frmcnt number' Specify the number of images to acquire from the camera device.
 * -'--nokey'         Turn off the 'wait for key' at the end of the program.
 * -'--help'          Request help text output. Program will exit after printing the help text.
 * -'--version'       Request the version of the sample. Program will exit after printing the version.
 *
 * \note The parser is kept as simple as possible.
 */
bool parse_command_line(int argc, char* argv[]);

/*! \brief The sample implementation.
 *
 * The program will select a GenICam Producer library (cti) to work with.
 * It will then open the first available device and acquire a number of images from it.
 */
int main(int argc, char* argv[])
{
    std::cout << "IDS peak API Walkthrough Sample v" << VERSION << std::endl;

    peak::Library::Initialize();

    /* The example catches the exceptions which may reasonably occur in the immediate context of an operation.
     * This is for demonstration purposes.
     *
     * The following outer try block cares for catching the exceptions which may occur in theory but are quite unlikely.
     * In addition the try scope makes sure that the instances of IDS peak API objects will be automatically cleaned
     * up and deleted before waiting for a user input if the program does not return early.
     */
    try
    {
        uint32_t index = 0;

        /* Check for command line arguments and apply them. if appropriate. */
        if (!parse_command_line(argc, argv))
        {
            std::cout << "ERROR: Parsing command line failed." << std::endl;
            wait_for_enter();
            return 1;
        }

        if (print_version_and_exit)
        {
            wait_for_enter();
            return 0;
        }

        if (print_help_text_and_exit)
        {
            /* Caller requested help text. Print it and exit. */
            std::cout << std::endl;
            std::cout << "Command line args:" << std::endl;
            std::cout << "--cti filepath  : Specify the full path to a *.cti file to be instrumented." << std::endl;
            std::cout << "--frmcnt number : Specify the number of images to acquire from the camera device."
                      << std::endl;
            std::cout << "--nokey         : Turn off the 'wait for key' at the end of the program." << std::endl;
            std::cout << "--help          : Request this help text. Program will exit after printing the help text."
                      << std::endl;
            std::cout
                << "--version       : Request the version of the sample. Program will exit after printing the version."
                << std::endl;
            wait_for_enter();
            return 0;
        }

        std::cout << std::endl;

        std::string cti_path;

        if (cti_selection.empty())
        {
            /* No GenTL Producer (*.cti) selection by command line argument.
             * List the available *.ctis and let the user select one. */

            /* Info: The GenTL standard defines the environment variable GENICAM_GENTL{32/64}_PATH to list the filepaths
             *       in the system where GenICam Producer libraries have been installed to.
             *
             * The IDS peak API's EnvironmentInspector provides the function CollectCTIPaths() which evaluates this
             * environment variable and scans the indicated paths for *.cti files.
             */
            auto cti_path_list = peak::core::EnvironmentInspector::CollectCTIPaths();

            if (cti_path_list.empty())
            {
                std::cout << "No GenTL libraries detected!" << std::endl;
                wait_for_enter();
                return 1;
            }

            index = 0;
            std::cout << "Available GenTL libraries:" << std::endl;
            for (const auto a_cti_path : cti_path_list)
            {
                std::cout << ++index << ") " << a_cti_path << std::endl;
            }

            if (cti_path_list.size() > 1)
            {
                /* More than one library found: Let the user select a library. */
                std::string selectedIndex;
                do
                {
                    std::cout << "Select a library by its index: ";
                    try
                    {
                        selectedIndex.clear();
                        std::getline(std::cin, selectedIndex);

                        index = std::stoul(selectedIndex);
                        if (index > 0 && index <= cti_path_list.size())
                        {
                            break;
                        }
                    }
                    catch (...)
                    {
                        ;
                    }
                } while (true);
            }
            else
            {
                /* One library found: Auto-select it. */
                index = 1;
            }

            cti_path = cti_path_list[index - 1];
        }
        else
        {
            /* The user specified the full path to the *.cti file to use by command line argument. */
            cti_path = cti_selection;
        }

        /* Info: GenTL is organized in a hierarchical design of several modules:
         *       System
         *       --> Interface 0
         *       --> ...
         *       --> Interface k
         *          --> Device 0
         *          --> ...
         *          --> Device l
         *              --> DataStream 0
         *              --> ...
         *              --> DataStream m
         *                  --> Buffer 0
         *                  --> ...
         *                  --> Buffer n
         *
         *       The root level of this hierarchy in a *.cti is a single System module.
         *       The System module may enumerate a number of Interface modules.
         *       An Interface module typically represents a physical interface like a network card.
         *       Each Interface module may enumerate one or more Device modules.
         *       A Device module typically represents a physical device that is connected to the interface.
         *       Each Device module may enumerate a number of DataStream modules.
         *       A DataStream module represents a data source on the device.
         *       E.g. a camera device will provide a DataStream module for the image data transfer.
         *       Each DataStream module needs to be provided with a number of data buffers for the arising data.
         *       The DataStream module enumerates a Buffer module for each of the announced buffers.
         *
         *       In order to receive image data from a camera device the GenICam Consumer application has to:
         *        1. Open the System module.
         *        2. Make the System module update its Interface module list.
         *        3. Query the Interface module list from the System module.
         *        4. Select the Interface module to use.
         *        5. Open the Interface module.
         *        6. Make the Interface module update its Device module list.
         *        7. Query the Device module list from the Interface module.
         *        8. Select the Device module to use.
         *        9. Open the Device module.
         *       10. Query the DataStream module list from the Device module.
         *       11. Select the DataStream module to use.
         *       12. Provide a number of data buffers to the DataStream module.
         *       13. Configure the (remote) device and its mode of operation.
         *       14. Start the acquisition in the DataStream module.
         *       15. Start the acquisition in the (remote) device.
         *
         * The IDS peak API provides two classes for each of the GenTL module entities.
         * One class is the module descriptor, the other one is the module itself.
         * A module descriptor instance ..
         * .. represents a specific available module in the parent modules' child list.
         * .. provides information on the module that is available without opening the module.
         * .. provides a function to open the module.
         * In order to work with a specific module, an instance of the module has to be created by
         * calling the module descriptor's open function.
         * A module instance ..
         * .. represents a specific opened module.
         * .. provides all information on the module.
         * .. can enumerate its child modules (as a list of child module descriptors).
         * .. provides the functional methods for the operation of the module.
         * .. is automatically closed/cleaned up in the context of module destruction.
         *
         * Example: An instance of peak::core::Interface can be asked for the list of it's child Device modules
         *          by the Devices() function. Interface::Devices() returns a list of DeviceDescriptor objects. A
         *          DeviceDescriptor can be asked for information like its display name. A DeviceDescriptor is not able
         *          to enumerate its child (data stream) modules. A call to DeviceDescriptor::OpenDevice() returns an
         *          instance of peak::core::Device. This Device instance can the be asked for its
         *          DataStreams() list.
         *
         * To simplify the above procedure up to step 7, the IDS peak API provides the class DeviceManager.
         * The DeviceManager::Update() function, if called with default arguments, ..
         * .. scans the environment,
         * .. for each *.cti ..
         *    .. opens the System module,
         *    .. makes the System module update its Interface module list,
         *    .. for each Interface module ..
         *       .. opens the Interface module,
         *       .. makes the Interface module update its Device module list,
         *       .. for each Device module ..
         *          .. adds a DeviceDescriptor object to the DeviceManager's Devices() list.
         *
         * Note: The DeviceManager::AddProducerLibrary() function can be used to announce a specific *.cti file
         *       to the device manager. If a *.cti was announced by this way, the DeviceManager::Update() function
         *       by default considers this *.cti file in addition to the *.cti files from the environment variable.
         *
         *       To ignore the *.ctis files from the environment variable and only consider the ones announce via
         *       DeviceManager::AddProducerLibrary(), call the DeviceManager::Update() function with:
         *       DeviceManager::UpdatePolicy::DontScanEnvironmentForProducerLibraries
         */

        auto& device_manager = peak::DeviceManager::Instance();

        try
        {
            /* Make the device manager update its device list. */
            auto on_update_error = [](const auto& errmsg) {
                std::cout << "WARNING: Updating device manager failed." << std::endl;
                std::cout << "         Msg: " << errmsg << std::endl;
            };

            /* The user already selected a *.cti, so only use that one and ignore the other ones from
             * the environment variable.
             */
            device_manager.AddProducerLibrary(cti_path);
            device_manager.Update(
                peak::DeviceManager::UpdatePolicy::DontScanEnvironmentForProducerLibraries, on_update_error);
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Updating device manager failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        /* Note: The device manager enumerates exactly one System module.
         *       (We called AddProducerLibrary() once and we called Update() with
         *       DontScanEnvironmentForProducerLibraries.)
         */
        auto system_module = device_manager.Systems().at(0);

        std::cout << "Will use" << (!cti_selection.empty() ? " selected " : " ") << "library "
                  << system_module->CTIFullPath() << '.' << std::endl;

        /* The device manager has already ..
         * .. loaded the selected Producer library, and
         * .. opened all modules of the selected *.cti up to the device level.
         *
         * So, the lists DeviceManager::Systems() and DeviceManager::Interfaces() contain ready-to-use module objects.
         * (System and Interface modules have been opened.)
         * Whereas the DeviceManager::Devices() list contains DeviceDescriptor objects.
         * (Device modules have not been opened yet.)
         */

        std::cout << "The library reports to implement GenTL V" << system_module->GenTLVersionMajor() << '.'
                  << system_module->GenTLVersionMinor() << '.' << std::endl;

        std::cout << "TL Vendor name is '" << system_module->VendorName() << "'." << std::endl;
        std::cout << "TL Version is '" << system_module->Version() << "'." << std::endl;

        /* The following region demonstrates the enumeration of the device module. */
#pragma region enumerate_the_available_devices

        auto cti_devicelist = device_manager.Devices();

        if (cti_devicelist.size() == 0)
        {
            std::cout << "ERROR: No device present at the selected library!" << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "The selected library enumerates " << cti_devicelist.size() << " device(s)." << std::endl;

        index = 0;
        for (const auto& device_descriptor : cti_devicelist)
        {
            std::cout << "Device #" << ++index << ':' << std::endl;
            std::cout << "----------" << std::endl;
            std::cout << "TLType:             " << device_descriptor->TLType() << std::endl;
            std::cout << "Model Name:         " << device_descriptor->ModelName() << std::endl;
            std::cout << "Display name:       " << device_descriptor->DisplayName() << std::endl;
            std::cout << "ID:                 " << device_descriptor->ID() << std::endl;
            std::cout << "@Interface:         " << device_descriptor->ParentInterface()->DisplayName() << std::endl;
            std::cout << "Access status:      " << peak::core::ToString(device_descriptor->AccessStatus())
                      << std::endl;
            std::cout << "Control Access avl: "
                      << (device_descriptor->IsOpenable(peak::core::DeviceAccessType::Control) ? "yes" : "no")
                      << std::endl;
        }

        std::cout << std::endl;

#pragma endregion enumerate_the_available_devices

        /* There is at least one device. Let's open it! */
#pragma region open_the_first_available_device

        /* Open the first Device module that is available for the desired 'Control' access.
         *
         * Info: In GenICam there are two 'Device' entities for each physical device,
         *       these are the 'Local Device' and the 'Remote Device'.
         *       For each physical device connected to an interface the GenTL Interface module maintains
         *       a Local Device module that represents the physical device. The physical device is then referred to as
         *       Remote Device.
         *       In other words the local device acts as a proxy object to connect the GenTL client to a remote device.
         *
         *       The opening of the GenTL's Local Device module actually affects the Remote Device also.
         *       I.e. in the procedure of instantiating a Local Device instance the GenTL implementation cares for
         *       opening the Remote Device.
         *
         *       By the time of opening the device we have to specify the access we desire to have on the device.
         *       This has implications on the availability of the device for other GenICam applications which discover
         *       the same device. See the GenTL enumeration DEVICE_ACCESS_FLAGS_LIST for the details.
         *       The 'Control' access (that we chose here) will grant ourself read/write access on the device.
         *       Other applications are then allowed to open the device for 'ReadOnly' access in parallel.
         *       The other way around, if any other application has currently opened the device in 'Control' or
         *       in 'Exclusive' access mode, our 'Control' mode open request will be rejected.
         *
         * Note: In the IDS peak API the Local Device is referred to as 'Device'
         *       and the Remote Device is referred to as 'RemoteDevice'.
         *       The RemoteDevice instance can be queried from the Device instance by Device::RemoteDevice().
         */

        std::shared_ptr<peak::core::Device> device;
        index = 0;

        for (const auto& device_descriptor : cti_devicelist)
        {
            ++index;
            /* A DeviceDescriptor object knows whether the device module is available for a specific access mode. */
            if (device_descriptor->IsOpenable(peak::core::DeviceAccessType::Control))
            {
                try
                {
                    /* The device descriptor declared the device to be available for 'Control' access.
                     * So, we can open the device module now, unless some other process has opened the device
                     * since our call to DeviceDescriptor::IsOpenable().
                     * We open the device by the device descriptor's OpenDevice() function.
                     * This function allocates and returns a peak::core::Device object.
                     */
                    device = device_descriptor->OpenDevice(peak::core::DeviceAccessType::Control);
                    break;
                }
                catch (const std::exception& e)
                {
                    std::cout << "ERROR: Opening device for 'Control' access failed." << std::endl;
                    std::cout << "       Msg: " << e.what() << std::endl;
                    wait_for_enter();
                    return 1;
                }
            }
        }

        if (device == nullptr)
        {
            std::cout << "ERROR: No device available for 'Control' access!" << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "Did successfully open the first available device (#" << index << ") for 'Control' access."
                  << std::endl;

#pragma endregion open_the_first_available_device

        /* For the purpose of this example we require the device to offer at least one data stream.
         * Let's validate that.
         *
         * Note: Device does not offer an UpdateDataStreams() function, because the data streams are statically declared
         *       by the remote device. The data stream list is built up at the time of opening the local device module.
         */
        if (device->DataStreams().size() == 0)
        {
            std::cout << "ERROR: The device does not offer a data stream!" << std::endl;
            wait_for_enter();
            return 1;
        }

        /* There is at least one DataStream module. Let's open the first one. */
#pragma region open_the_first_datastream

        std::shared_ptr<peak::core::DataStream> datastream;

        try
        {
            datastream = device->DataStreams()[0]->OpenDataStream();
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Opening device's first datastream failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "Did successfully open the device's first datastream." << std::endl;

#pragma endregion open_the_first_datastream

        /* The following region demonstrates the configuration of the remote device. */
#pragma region configure_the_remote_device

        std::shared_ptr<peak::core::NodeMap> remote_device_nodemap;
        try
        {
            /* Info: All GenICam modules, including the remote device, provide their features in the form of so
             *       called nodes.
             *       The nodes are organized in a tree structure and held in a so called NodeMap.
             *
             * Note: A remote device may provide more than one NodeMap.
             *       Here we decide to use the first one.
             */
            remote_device_nodemap = device->RemoteDevice()->NodeMaps()[0];
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Getting remote device's first NodeMap failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        /* We make sure the "AcquisitionMode" is "Continuous" */
#pragma region set_acquisition_mode
        /* The Standard (SFNC - GenICam Standard Features Naming Convention) defines the 'AcquisitionMode' feature
         * as 'Recommended'.
         * So there is a good chance that the node is not implemented and that we get a NotFoundException exception
         * when we ask the NodeMap for this node.
         * If we don't get an exception, we will assume that setting the AcquisitionMode is possible.
         */

        try
        {
            /* SFNC tells us that the 'AcquisitionMode' setting is of type Enumeration.
             * The enumeration values are actually integer type values.
             * We can ask the EnumerationNode for any of its values by its symbolic name or by its integer
             * representation.
             * The currently selected entry is delivered by EnumerationNode::CurrentEntry() and is of type
             * peak::core::nodes::EnumerationEntryNode.
             * EnumerationNode and EnumerationEntryNode provide translation functions that map
             * the symbolic enum entry values to their integer representations.
             */
            const std::string current_acquisition_mode = remote_device_nodemap
                                                             ->FindNode<peak::core::nodes::EnumerationNode>(
                                                                 "AcquisitionMode")
                                                             ->CurrentEntry()
                                                             ->SymbolicValue();

            if (current_acquisition_mode != "Continuous")
            {
                std::cout << "Current value of remote device node 'AcquisitionMode' is '"
                          << current_acquisition_mode.c_str() << "'. Will change it to 'Continuous'." << std::endl;

                /* The node 'AcquisitionMode' is defined to have Read/Write access.
                 * So we will not explicitly test for write access to be allowed here.
                 *
                 * Note: We could check for the node to allow write access by
                 *       comparing peak::core::nodes::Node::AccessStatus() with
                 *       peak::core::nodes::NodeAccessStatus::WriteOnly and
                 *       peak::core::nodes::NodeAccessStatus::ReadWrite.
                 */
                try
                {
                    remote_device_nodemap->FindNode<peak::core::nodes::EnumerationNode>("AcquisitionMode")
                        ->SetCurrentEntry("Continuous");
                    std::cout << "Did set the value of remote device node 'AcquisitionMode' to 'Continuous'."
                              << std::endl;
                }
                catch (const std::exception& e)
                {
                    std::cout
                        << "WARNING: Setting the remote device node 'AcquisitionMode' to value 'Continuous' failed."
                        << std::endl;
                    std::cout << "         Msg: " << e.what() << std::endl;
                }
            }
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "INFO: The remote device does not provide the node 'AcquisitionMode'." << std::endl;
            (void)e;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Getting the current selection of remote device node 'AcquisitionMode' failed."
                      << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }


#pragma endregion set_acquisition_mode

        /* We try to configure an exposure time of 1ms. */
#pragma region set_exposure_time

        /* Setting a user-defined exposure time is allowed in the exposure mode 'Timed' only.
         * So we check if the exposure mode is set to 'Timed' first.
         */

        /* The Standard (SFNC - GenICam Standard Features Naming Convention) defines the 'ExposureMode' feature
         * as 'Recommended'.
         * So there is a good chance that the node is not implemented and that we get a NotFoundException exception
         * when we ask the NodeMap for this node.
         * If we don't get an exception, we will assume that setting the exposure time is possible.
         */

        try
        {
            /* SFNC tells us that the 'ExposureMode' setting is of type Enumeration. */
            const std::string current_exposure_mode = remote_device_nodemap
                                                          ->FindNode<peak::core::nodes::EnumerationNode>(
                                                              "ExposureMode")
                                                          ->CurrentEntry()
                                                          ->SymbolicValue();

            if (current_exposure_mode == "Timed")
            {
                /* Here we assume that the current exposure mode allows setting a exposure time of 1ms. */

                /* SFNC defines the 'ExposureTime' feature as 'Recommended'.
                 * Like the 'ExposureMode' node we tolerate an error here.
                 *
                 * SFNC tells us that the 'ExposureTime' setting is a Float and that its unit is us.
                 *
                 * Note: In a real-world application we would probably want to make sure that we obey the
                 *       node's valid range of values.
                 *       This can be done by the functions Minimum(), Maximum() and Increment() of
                 *       peak::core::nodes::FloatNode.
                 */
                try
                {
                    remote_device_nodemap->FindNode<peak::core::nodes::FloatNode>("ExposureTime")
                        ->SetValue(1'000.0);
                    std::cout << "Did set the remote device exposure time to 1,000us." << std::endl;
                }
                catch (const peak::core::NotFoundException& e)
                {
                    std::cout << "INFO: The remote device does not provide the node 'ExposureTime'." << std::endl;
                    (void)e;
                }
                catch (const std::exception& e)
                {
                    std::cout << "WARNING: Setting the exposure time to the value 1,000us failed." << std::endl;
                    std::cout << "         Msg: " << e.what() << std::endl;
                }
            }
            else
            {
                std::cout << "Current value of 'ExposureMode' node is '" << current_exposure_mode
                          << "' instead of 'Timed'. Therefore we can't set exposure time." << std::endl;
            }
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "INFO: The remote device does not provide the node 'ExposureMode'." << std::endl;
            (void)e;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Getting the current selection of remote device node 'ExposureMode' failed."
                      << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }

#pragma endregion set_exposure_time

        /* As we want to operate the device in not-triggered continuous acquisition mode with
         * a self-defined frame rate, we have to disable a possibly enabled trigger mode. */
#pragma region disable_trigger_mode

        /* The trigger mode is one of the features that may implement the so-called Selector concept.
         *
         * Info: The GenICam Selector concept enables one to address a certain feature in respect to a certain subject.
         *       The selector can be seen as an index into an array of configuration subjects that a configuration item
         *       can be applied to.
         *       E.g. a selected trigger mode can be applied to a number of different events that a trigger signal
         *       starts.
         *       The procedure for configuring a feature that is implemented using the Selector concept is as follows:
         *       1. Set the selector to the subject that you want the configuration to be applied to.
         *       2. Set the configuration.
         *
         * Here we want to disable the trigger mode for the start of the acquisition as we want to operate the
         * image sensor in the free-running mode.
         */

        /* SFNC defines the 'TriggerMode' feature as 'Recommended'.
         * So we tolerate an error here.
         *
         * If there is a 'TriggerMode' feature we need to check if it is selected and if so, we need to set the selector
         * to 'AcquisitionStart' before we set the trigger mode.
         * This is because we want to specify the trigger related behavior of the acquisition start.
         *
         * SFNC tells us that the 'TriggerSelector' setting is an Enumeration.
         */

        /* SFNC defines the 'TriggerSelector' feature as 'Recommended'.
         * So there is a good chance that the node is not implemented and that we get an exception when we ask
         * the NodeMap for this node.
         * If we don't get an exception, we will assume that setting the TriggerSelector is possible.
         */

        /* A node can be asked to enumerate the features (nodes) that it selects by SelectedNodes().
         * But as we know which node we are looking for we can simply ask the node to
         * FindSelectedNode("TriggerMode"). If the selector node does not select the 'TriggerMode' feature we will
         * get an exception.
         *
         * Let's verify that the 'TriggerSelector' node selects the 'TriggerMode' feature that we want to manipulate.
         * If the 'TriggerSelector' node does not select the 'TriggerMode' feature we do not have to adjust the selector
         * for the manipulation of the trigger mode.
         */
        try
        {
            /* Note: Here we just want to check if there is a 'TriggerSelector' node that selects the 'TriggerMode'.
             *       We are not interested in the node instance, but in the NotFoundException which is thrown by
             *       Node::FindSelectedNode() if 'TriggerMode' is not selected by 'TriggerSelector'.
             */
            (void)remote_device_nodemap->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->FindSelectedNode<peak::core::nodes::EnumerationNode>("TriggerMode");

            std::string current_trigger_selector_setting;

            /* We want to turn the trigger mode for the subject acquisition start to 'Off'.
             * So we have to make sure the selector is set to 'AcquisitionStart'.
             */
            try
            {
                current_trigger_selector_setting = remote_device_nodemap
                                                       ->FindNode<peak::core::nodes::EnumerationNode>(
                                                           "TriggerSelector")
                                                       ->CurrentEntry()
                                                       ->SymbolicValue();
                if (current_trigger_selector_setting != "AcquisitionStart")
                {
                    std::cout << "Current value of remote device node 'TriggerSelector' is '"
                              << current_trigger_selector_setting << "'. Will change it to 'AcquisitionStart'."
                              << std::endl;

                    /* The node 'TriggerSelector' is defined to have Read/Write access.
                     * So we will not explicitly test for write access to be allowed here.
                     */
                    try
                    {
                        remote_device_nodemap->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                            ->SetCurrentEntry("AcquisitionStart");
                        std::cout << "Did set the value of remote device node 'TriggerSelector' to 'AcquisitionStart'."
                                  << std::endl;
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "WARNING: Setting the remote device node 'TriggerSelector' to value "
                                     "'AcquisitionStart' failed."
                                  << std::endl;
                        std::cout << "         Msg: " << e.what() << std::endl;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "WARNING: Getting the current selection of remote device node 'TriggerSelector' failed."
                          << std::endl;
                std::cout << "         Msg: " << e.what() << std::endl;
            }
        }
        catch (const peak::core::NotFoundException& e)
        {
            /* The TriggerSelector node does not select the 'TriggerMode' feature
             * -> we do not have to adjust the selector.
             */
            std::cout << "INFO: The remote device does not provide the node 'TriggerSelector' or 'TriggerMode' is not "
                         "selected by it."
                      << std::endl;
            std::cout << "      Msg: " << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Checking if remote device's node 'TriggerSelector' selects 'TriggerMode' failed."
                      << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        /* At this point we have set the trigger selector to 'AcquisitionStart', if required.
         * Next is to disable the trigger mode for this subject.
         */

        /* SFNC tells us that the 'TriggerMode' setting is an Enumeration and that its values are 'On' and 'Off'. */
        try
        {
            remote_device_nodemap->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("Off");
            std::cout << "Did set the trigger mode for acquisition start to 'Off'." << std::endl;
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "INFO: The remote device does not provide the node 'TriggerMode'." << std::endl;
            (void)e;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Setting the trigger mode for acquisition start to 'Off' failed." << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }

#pragma endregion disable_trigger_mode

        /* We configure the frame rate to be the maximum supported frame rate. */
#pragma region set_frame_rate

        /* In order to set a user-defined frame rate we have to make sure that this is allowed.
         * So we will (try to) make sure that the 'AcquisisionFrameRateEnable' feature is set to 'On' first.
         *
         * SFNC tells us that the 'AcquisitionFrameRateEnable' setting is a Boolean.
         */
        bool current_acquisition_frame_rate_enable_setting = false;

        try
        {
            current_acquisition_frame_rate_enable_setting = remote_device_nodemap
                                                                ->FindNode<peak::core::nodes::BooleanNode>(
                                                                    "AcquisitionFrameRateEnable")
                                                                ->Value();

            if (current_acquisition_frame_rate_enable_setting != true)
            {
                std::cout << "Current value of remote device node 'AcquisitionFrameRateEnable' is 'False'. Will change "
                             "it to 'True'."
                          << std::endl;

                /* The node 'AcquisitionFrameRateEnable' is defined to have Read/Write access.
                 * So we will not explicitly test for write access to be allowed here.
                 */
                try
                {
                    remote_device_nodemap
                        ->FindNode<peak::core::nodes::BooleanNode>("AcquisitionFrameRateEnable")
                        ->SetValue(true);
                    std::cout << "Did set the value of remote device node 'AcquisitionFrameRateEnable' to 'True'."
                              << std::endl;
                }
                catch (const std::exception& e)
                {
                    std::cout << "WARNING: Setting the remote device node 'AcquisitionFrameRateEnable' to value 'True' "
                                 "failed."
                              << std::endl;
                    std::cout << "         Msg: " << e.what() << std::endl;
                }
            }
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "INFO: The remote device does not provide the node 'AcquisitionFrameRateEnable'." << std::endl;
            (void)e;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Getting the current value of remote device node 'AcquisitionFrameRateEnable' failed."
                      << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }

        /* Here we assume that setting a user-defined frame rate is possible. */

        /* The Standard defines the 'AcquisitionFrameRate' feature as 'Recommended'.
         * So we tolerate an error here.
         *
         * SFNC tells us that the 'AcquisitionFrameRate' setting is a Float and that its unit is Hz.
         */

        /* The following variable is used later in the acquisition loop. */
        double frame_rate_hz = 0.0;

        try
        {
            double current_frame_rate =
                remote_device_nodemap->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Value();
            std::cout << "Current frame rate is " << current_frame_rate << "Hz." << std::endl;
            frame_rate_hz = current_frame_rate;

            try
            {
                double maximum_frame_rate = remote_device_nodemap
                                                ->FindNode<peak::core::nodes::FloatNode>(
                                                    "AcquisitionFrameRate")
                                                ->Maximum();

                if (current_frame_rate != maximum_frame_rate)
                {
                    std::cout << "Current frame rate is " << current_frame_rate
                              << "Hz. Will change it to the maximum "
                                 "frame rate of "
                              << maximum_frame_rate << "Hz." << std::endl;

                    /* The node 'AcquisitionFrameRate' is defined to have Read/Write access.
                     * So we will not explicitly test for write access to be allowed here.
                     */

                    try
                    {
                        remote_device_nodemap->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                            ->SetValue(maximum_frame_rate);
                        std::cout << "Did set the value of remote device node 'AcquisitionFrameRate' to "
                                  << maximum_frame_rate << "Hz." << std::endl;
                        frame_rate_hz = maximum_frame_rate;
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "WARNING: Setting the remote device node 'AcquisitionFrameRate' to value "
                                  << maximum_frame_rate << "Hz failed." << std::endl;
                        std::cout << "         Msg: " << e.what() << std::endl;
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cout << "WARNING: Getting the maximum frame rate failed." << std::endl;
                std::cout << "         Msg: " << e.what() << std::endl;
            }
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "INFO: The remote device does not provide the node 'AcquisitionFrameRate'." << std::endl;
            (void)e;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Getting the current frame rate failed." << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }

#pragma endregion set_frame_rate

#pragma endregion configure_the_remote_device

        /* Here we have configured the remote device with the settings we desire.
         * Next is to prepare and start the image acquisition.
         * We have to
         * - provide the data stream with memory buffers for the images.
         * - start the acquisition in the transport layer and in the remote device.
         */
#pragma region prepare_and_start_infinite_acquisition

        /* At first we have to supply memory for the images to the DataStream module. */
#pragma region announce_and_queue_image_buffers

        /* Query the minimum number of image buffers that the data stream requires for operation.
         *
         * Note: The Producer has to implement the query for STREAM_INFO_BUF_ANNOUNCE_MIN as it is defined to be
         * mandatory. However the Producer is allowed to return GC_ERR_NOT_AVAILABLE if this info is not available a
         * priori. In case the Producer rejects our query we will assume that 10 buffers will suffice for proper
         * operation.
         */
        size_t min_num_buffers = 0;

        try
        {
            min_num_buffers = datastream->NumBuffersAnnouncedMinRequired();
            std::cout << "Did successfully inquire the minimum number of required image buffers from the datastream."
                      << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Getting the minimum number of buffers to announce failed." << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
            min_num_buffers = 10;
        }

        /* Next info of interest is the required size of a single memory buffer.
         * It depends on the Producer implementation and the underlying transport layer if it is possible
         * for the Producer to provide this information. If the Producer does not provide the size, we need to
         * query it from the remote device.
         * So, next we will ask the Producer if it knows the required buffer size.
         *
         * \note The implementation of STREAM_INFO_DEFINES_PAYLOADSIZE is defined as mandatory for the Producer.
         *       But, as there is a fallback, we will tolerate the Producer failing the query.
         */
        bool stream_defines_payload_size = false;

        try
        {
            stream_defines_payload_size = datastream->DefinesPayloadSize();
        }
        catch (const std::exception& e)
        {
            std::cout << "WARNING: Asking the datastream whether it defines the payload size failed." << std::endl;
            std::cout << "         Msg: " << e.what() << std::endl;
        }

        size_t payload_size = 0;

        if (stream_defines_payload_size)
        {
            /* We can inquire the payload size from the DataStream module. */
            std::cout << "Will inquire the payload size from the datastream." << std::endl;
            try
            {
                payload_size = datastream->PayloadSize();
            }
            catch (const std::exception& e)
            {
                std::cout << "ERROR: Getting the payload size from the datastream failed." << std::endl;
                std::cout << "       Msg: " << e.what() << std::endl;
                wait_for_enter();
                return 1;
            }
        }
        else
        {
            /* We need to inquire the payload size from the remote device. */
            std::cout << "Will inquire the payload size from the remote device." << std::endl;

            /* The Standard defines the 'PayloadSize' feature as a 'Recommended' node of type Integer.
             * But, as we absolutely need to know the payload size, we do not tolerate a failure here.
             *
             * Note: We could allocate memory of very large size instead and expect the memory size to be
             *       sufficient then.
             *       BUT there are Producer implementations (like the IDS Imaging Development Systems GmbH GenTL up to V1.1) that do not
             *       operate correctly if the buffer sizes differ from the received amount of data for an image.
             */
            try
            {
                payload_size = remote_device_nodemap->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                                   ->Value();
            }
            catch (const peak::core::NotFoundException& e)
            {
                std::cout << "ERROR: The remote device does not provide the node 'PayloadSize'." << std::endl;
                (void)e;
                wait_for_enter();
                return 1;
            }
            catch (const std::exception& e)
            {
                std::cout << "ERROR: Getting the current value of remote device node 'PayloadSize' failed."
                          << std::endl;
                std::cout << "       Msg: " << e.what() << std::endl;
                wait_for_enter();
                return 1;
            }
        }

        std::cout << "Did determine the payload size of " << payload_size << " bytes and the minimum number of "
                  << min_num_buffers << " required buffers." << std::endl;

        /* Now that we know the payload size and the number of required buffers we can prepare and provide
         * the image buffers.
         */

        /* (Alloc and) Announce the buffers that shall receive the image data to the DataStream module.
         *
         * Note: We have two options here:
         *       1. Allocate the image data memory by ourselves.
         *       2. Let the Producer provide the memory for the image data.
         *       The following code block demonstrates both methods.
         *       The variable consumer_provides_image_buffer_memory depicts which method is used.
         */
        bool consumer_provides_image_buffer_memory = true;

        for (uint32_t u = 0; u < min_num_buffers; u++)
        {
            if (consumer_provides_image_buffer_memory)
            {
                /* In this case we allocate the image data memory by ourselves.
                 *
                 * Note: The third argument of DataStream::AnnounceBuffer() takes a user context pointer.
                 *       That value will be reported back to us by the Producer with the 'buffer filled' indication for
                 *       the corresponding buffer.
                 *       In this example we do not use this concept and pass (the default) nullptr for the context
                 *       argument. Note: We do not need to store the pointer to the allocated memory although we need to
                 *       know it in order to free the memory at the time of cleanup. This is possible because the
                 *       Producer will provide us with the pointer when we revoke the buffer by
                 *       DataStream::RevokeBuffer(), which is the antagonist function of DataStream::AnnounceBuffer().
                 *       (In addition to that we could ask the Producer for the pointer at any time via the buffer
                 *       handle.)
                 */
                try
                {
                    /* We don't care for the returned buffer proxy. There is no benefit if we would remember it. */
                    char* image_data = new char[payload_size];
                    (void)datastream->AnnounceBuffer(
                        image_data, payload_size, nullptr, [](void* buffer, void* userPtr) {
                            std::cout << "Will free buffer " << buffer << '.' << std::endl;
                            delete[] static_cast<char*>(buffer);
                        });
                    std::cout << "Did allocate and announce buffer " << static_cast<void*>(image_data) << '.'
                              << std::endl;
                }
                catch (const std::exception& e)
                {
                    std::cout << "ERROR: Announcing buffer #" << u + 1 << " to the datastream failed." << std::endl;
                    std::cout << "       Msg: " << e.what() << std::endl;
                    wait_for_enter();
                    return 1;
                }
            }
            else
            {
                /* In this case we let the Producer care for the image data memory. The producer will allocate the
                 * memory here and clean it up when the buffer is revoked.
                 *
                 * Note: The second argument of DataStream::AllocAndAnnounceBuffer() takes a user context pointer.
                 *       That value will be reported back to us by the Producer with the 'buffer filled' indication for
                 *       the corresponding buffer.
                 *       In this example we do not use this concept and pass nullptr for the context argument.
                 */
                try
                {
                    /* We don't care for the returned buffer proxy. There is no benefit if we would remember it. */
                    (void)datastream->AllocAndAnnounceBuffer(payload_size, nullptr);
                }
                catch (const std::exception& e)
                {
                    std::cout << "ERROR: Alloc and Announcing buffer #" << u + 1 << " to the datastream failed."
                              << std::endl;
                    std::cout << "       Msg: " << e.what() << std::endl;
                    wait_for_enter();
                    return 1;
                }
            }
        }

        std::cout << "Did successfully (alloc and) announce " << datastream->NumBuffersAnnounced() << " buffers of "
                  << payload_size << " bytes each." << std::endl;

        /* The GenTL Producer's acquisition engine organizes the image buffers in so-called 'Buffer Pools'.
         * The standard defines three pools:
         * - 'Announced Buffer Pool':
         *   All buffers reside in this pool as long as they are known by the Producer, i.e. in the time between the
         * calls of DataStream::(AllocAnd)AnnounceBuffer() and DataStream::RevokeBuffer().
         * - 'Input Buffer Pool':
         *   This pool contains all buffers that are ready to receive image data.
         *   A buffer is moved to the Input Buffer Pool by calling DataStream::QueueBuffer().
         *   This operation is referred to as "queueing" a buffer.
         * - 'Output Buffer Queue':
         *   This queue holds all buffers that were filled by the stream channel and wait for delivery to the Consumer.
         *
         * Note: In the Announced Buffer POOL and in the Input Buffer POOL the buffers are kept unordered.
         *       It is not defined which one of the buffers is provided on a pull request.
         *       Whereas in the Output Buffer QUEUE the buffers are kept in a certain order.
         *       They are for example ordered by the timestamp of the contained data.
         * Note: The Input Buffer Pool's unordered set character implicates that it is not predictable which one of
         *       the buffers is filled next.
         * Note: The possibility of a move of a buffer from one pool to an other pool depends on certain conditions.
         *       For example: A buffer can only be "queued" to the input pool if it is not located in the output pool.
         */

        /* Queue all buffers, i.e. allow the DataStream module to fill the buffers with acquired data. */
        index = 1;
        for (const auto& buffer : datastream->AnnouncedBuffers())
        {
            try
            {
                datastream->QueueBuffer(buffer);
            }
            catch (const std::exception& e)
            {
                std::cout << "ERROR: Queueying buffer #" << index << " to the datastream failed." << std::endl;
                std::cout << "       Msg: " << e.what() << std::endl;
                wait_for_enter();
                return 1;
            }
            index++;
        }

        std::cout << "Did successfully queue all " << datastream->NumBuffersQueued()
                  << " buffers for image acquisition." << std::endl;

#pragma endregion announce_and_queue_image_buffers

        /* Finally we will start the acquisition.
         *
         * Note: We have to start the acquisition in the datastream's acquisition engine
         *       AND in the remote device to receive images.
         */
#pragma region start_infinite_acquisition

        /* Lock critical features to prevent them from changing during acquisition.
         * After acquisition stop they should be unlocked again.
         */

        /* SFNC tells us that the 'TLParamsLocked' feature is an Integer.
         * The value '0' means that there are no features locked and the value '1' means that transport layer and
         * device critical features arelocked and cannot be changed.
         *
         * The node 'TLParamsLocked' is defined to have Read/Write access.
         */
        try
        {
            remote_device_nodemap->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Setting the value of remote device node 'TLParamsLocked' failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "Did successfully lock transport layer parameters." << std::endl;

        /* Start the acquisition engine in the transport layer. */
        try
        {
            datastream->StartAcquisition();
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Starting acquisition in the datastream failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "Did successfully start the datastream's acquisition engine." << std::endl;

        /* Start the acquisition in the Remote Device. */

        /* SFNC tells us that the 'AcquisitionStart' feature is a Command. */
        try
        {
            remote_device_nodemap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        }
        catch (const peak::core::NotFoundException& e)
        {
            std::cout << "ERROR: The remote device does not provide the node 'AcquisitionStart'." << std::endl;
            (void)e;
            wait_for_enter();
            return 1;
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: Triggering the remote device command node 'AcquisitionStart' failed." << std::endl;
            std::cout << "       Msg: " << e.what() << std::endl;
            wait_for_enter();
            return 1;
        }

        std::cout << "Did successfully start the remote device's acquisition engine." << std::endl;

#pragma endregion start_infinite_acquisition

#pragma endregion prepare_and_start_infinite_acquisition

        /* We have set up and started an infinite image acquisition.
         * This acquisition runs until we stop it.
         * What we need to do now is to repeatedly wait for filled buffers and process the image data.
         *
         * Note: Typically this acquisition loop is run in a dedicated thread.
         *       In a real world application we would not start the acquisition in the device
         *       until the acquisition loop had been entered.
         */
#pragma region the_acquisition_loop

        uint64_t new_buffer_event_wait_timeout = 1000;

        uint32_t num_frames_received = 0;
        uint32_t num_incomplete_frames = 0;
        uint32_t num_wait_timeouts = 0;

        std::cout << std::endl;
        std::cout << "Will acquire " << num_frames_to_acquire << " frames.";
        if (frame_rate_hz != 0.0)
        {
            /* frame_rate_hz is non-zero if we successfully managed to read (and perhaps adjust) the acquisition
             * frame rate earlier.
             */

            /* We will set the timeout for the frame event to three times the frame time. */
            double three_frame_times_timeout_ms = 3000.0 / frame_rate_hz;
            new_buffer_event_wait_timeout = static_cast<uint64_t>(three_frame_times_timeout_ms);

            std::cout << " Framerate is " << frame_rate_hz << "fps. Wait timeout is " << new_buffer_event_wait_timeout
                      << "ms." << std::endl;
        }
        else
        {
            std::cout << " Wait timeout is " << new_buffer_event_wait_timeout << "ms." << std::endl;
        }

        while (num_frames_received < num_frames_to_acquire)
        {
            /* For event signaling the GenTL standard defines the blocking call concept, i.e. the consumer calls a
             * function that blocks until the event of interest has occurred.
             * For retrieving new images, this function is DataStream::WaitForFinishedBuffer().
             */

            decltype(datastream->WaitForFinishedBuffer(0)) finished_buffer;

            try
            {
                /* Blocking call until a new buffer arrives. */
                finished_buffer = datastream->WaitForFinishedBuffer(new_buffer_event_wait_timeout);

                /* If we get to here a buffer did receive image data.
                 * We can now inquire inquire information on the buffer's state.
                 */

                /* Check for the buffer to contain valid data. */
                bool buffer_has_new_data = false;
                try
                {
                    buffer_has_new_data = finished_buffer->HasNewData();
                }
                catch (const std::exception& e)
                {
                    std::cout << std::endl;
                    std::cout << "WARNING: Asking buffer if it has new data failed." << std::endl;
                    std::cout << "         Msg: " << e.what() << std::endl;
                }

                /* Check if the buffer is incomplete.
                 *
                 * Note: A buffer is considered incomplete if the stream channel was not able to receive all
                 *       packets for the image.
                 */
                bool buffer_is_incomplete = false;
                try
                {
                    buffer_is_incomplete = finished_buffer->IsIncomplete();
                }
                catch (const std::exception& e)
                {
                    std::cout << std::endl;
                    std::cout << "WARNING: Asking buffer if it is incomplete failed." << std::endl;
                    std::cout << "         Msg: " << e.what() << std::endl;
                }

                if (buffer_is_incomplete)
                {
                    num_incomplete_frames++;
                    std::cout << 'i' << std::flush;
                }
                else if (buffer_has_new_data)
                {
                    /* The buffer contains valid image data.
                     * Get the memory address of the image data and process the image.
                     */
                    char* image_data = nullptr;
                    try
                    {
                        image_data = reinterpret_cast<char*>(finished_buffer->BasePtr());
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << std::endl;
                        std::cout << "WARNING: Getting buffer's memory pointer failed." << std::endl;
                        std::cout << "         Msg: " << e.what() << std::endl;
                    }

                    /* Here we would process the image data at image_data.
                     *
                     * In this example we print out a dot for each completely received image.
                     */
                    std::cout << '.' << std::flush;
                }
                else
                {
                    std::cout << 'x' << std::flush;
                }

                /* When a buffer is delivered to the Consumer (us), the Consumer owns the buffer and it is not
                 * available for the Producer to receive new image data into it.
                 */

                /* The situation where the stream channel receives data for a new image and
                 * there is no image buffer available to be filled in the Input Buffer Pool is referred to as
                 * 'Queue Underrun'.
                 * This typically happens if the time that the Consumer needs to process an image
                 * is larger than the time between two images on the wire.
                 * This situation can be simulated by commenting in the following code block.
                 * The DataStream module counts these situations (if implemented by the Producer).
                 * We will query and print this counter after the acquisition sequence has ended.
                 */
                // if (frame_rate_hz != 0.0)
                //{
                //    uint32_t sleep_time_ms = static_cast<uint32_t>(1000.0 / frame_rate_hz);
                //    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
                //}

                /* So it is important that we re-queue the buffer as soon as we are done with it to make it available
                 * for the stream channel again.
                 */
                try
                {
                    datastream->QueueBuffer(finished_buffer);
                }
                catch (const std::exception& e)
                {
                    std::cout << std::endl;
                    std::cout << "ERROR: Re-Queuing buffer to the datastream failed." << std::endl;
                    std::cout << "       Msg: " << e.what() << std::endl;
                    wait_for_enter();
                    return 1;
                }
            }
            catch (const peak::core::TimeoutException& e)
            {
                num_wait_timeouts++;
                std::cout << 't' << std::flush;
                (void)e;
            }
            catch (const peak::core::AbortedException& e)
            {
                /* An AbortedException is only thrown when the current call to DataStream::WaitForFinishedBuffer()
                 * was canceled via DataStream::KillWait(). */
                std::cout << 'a' << std::flush;
                (void)e;
            }
            catch (const std::exception& e)
            {
                std::cout << std::endl;
                std::cout << "ERROR: Waiting for finished buffer with timeout of " << new_buffer_event_wait_timeout
                          << "ms failed." << std::endl;
                std::cout << "       Msg: " << e.what() << std::endl;
                wait_for_enter();
                return 1;
            }

            num_frames_received++;
        }

#pragma endregion the_acquisition_loop

        /* Inquire the number of 'Queue Underruns' from the DataStream module (if implemented).
         * See comment in the acquisition loop above.
         */
        bool num_underruns_is_valid = true;
        uint64_t num_underruns = 0;
        try
        {
            num_underruns = datastream->NumUnderruns();
        }
        catch (const std::exception&)
        {
            num_underruns_is_valid = false;
        }

        /* Print the acquisition results. */
        std::cout << std::endl;
        std::cout << "Did acquire " << num_frames_received << " frames. Incompletes: " << num_incomplete_frames
                  << ", Timeouts: " << num_wait_timeouts;
        if (num_underruns_is_valid)
        {
            std::cout << ", Underruns: " << num_underruns << '.' << std::endl;
        }
        else
        {
            std::cout << '.' << std::endl;
        }
        std::cout << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: Exception occurred." << std::endl;
        std::cout << "       Msg: " << e.what() << std::endl;
        wait_for_enter();
        return 1;
    }

    wait_for_enter();
    return 0;
}

void wait_for_enter()
{
    std::cout << std::endl;

    if (!no_key_for_exit)
    {
#if defined(WIN32)
        system("pause");
#elif defined(__linux__)
        std::cout << "Press enter to exit." << std::endl;
        system("read _");
#else
#    warning("Operating system not implemented!")
#endif
    }

    // close library before exiting program
    peak::Library::Close();
}

bool parse_command_line(int argc, char* argv[])
{
    if (argc > 1)
    {
        std::cout << std::endl;
        int32_t i = 1;
        while (i < argc)
        {
            std::string argname = argv[i];
            if (argname == "--cti")
            {
                if ((i + 1) >= argc)
                {
                    std::cout << "ERROR: Arg '--cti' requires filepath as value." << std::endl;
                    return false;
                }
                cti_selection = argv[i + 1];
                std::cout << "Detected command line argument '--cti' with value '" << cti_selection << "'."
                          << std::endl;
                i += 2;
            }
            else if (argname == "--frmcnt")
            {
                if ((i + 1) >= argc)
                {
                    std::cout << "ERROR: Arg '--frmcnt' requires number of frames to acquire as value." << std::endl;
                    return false;
                }
                int val = 0;
                try
                {
                    val = std::stoi(argv[i + 1]);
                }
                catch (...)
                {
                    std::cout << "ERROR: Value for arg '--frmcnt' required to be an unsigned integer. Provided value "
                                 "is '"
                              << argv[i + 1] << "'." << std::endl;
                    return false;
                }
                if (val < 0)
                {
                    std::cout << "ERROR: Value for arg '--frmcnt' required to be an unsigned integer. Provided value "
                                 "is '"
                              << argv[i + 1] << "'." << std::endl;
                    return false;
                }
                num_frames_to_acquire = static_cast<uint32_t>(val);
                std::cout << "Detected command line argument '--frmcnt' with value '" << num_frames_to_acquire << "'."
                          << std::endl;
                i += 2;
            }
            else if (argname == "--nokey")
            {
                std::cout << "Detected command line argument '--nokey'." << std::endl;
                no_key_for_exit = true;
                i += 1;
            }
            else if (argname == "--help")
            {
                std::cout << "Detected command line argument '--help'." << std::endl;
                print_help_text_and_exit = true;
                i += 1;
            }
            else if (argname == "--version")
            {
                std::cout << "Detected command line argument '--version'. Exiting." << std::endl;
                print_version_and_exit = true;
                i += 1;
            }
            else
            {
                std::cout << "ERROR: Invalid command line arg '" << argname << "'." << std::endl;
                return false;
            }
        }
    }

    return true;
}

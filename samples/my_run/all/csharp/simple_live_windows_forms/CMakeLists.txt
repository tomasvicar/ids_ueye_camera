cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project ("simple_live_windows_forms_csharp" CSharp)

message (STATUS "[${PROJECT_NAME}] Processing ${CMAKE_CURRENT_LIST_FILE}")

set (SAMPLE_TARGET_NAME ${PROJECT_NAME})
set (CMAKE_SCRIPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../_cmake_scripts" CACHE STRING "The path of the cmake scripts directory.")
set (SAMPLE_OUPUT_PATH "${CMAKE_BINARY_DIR}/output/bin")

include (${CMAKE_SCRIPTS_PATH}/cmake_detect_architecture.cmake)
detect_target_architecture (ARCH)

include (CSharpUtilities)
csharp_set_windows_forms_properties (
    FormWindow.cs
    FormWindow.Designer.cs
    FormWindow.resx
)

if (MSVC)
    set (SAMPLE_TARGET_DEFINITION_ADDENDUM WIN32)
endif ()

add_executable (${SAMPLE_TARGET_NAME} WIN32
    Program.cs
    BackEnd.cs
    AcquisitionWorker.cs
    FormWindow.cs
    FormWindow.Designer.cs
    FormWindow.resx
)

set (LIBRARY_NAME_VISION_API "ids_peak")
string (TOUPPER ${LIBRARY_NAME_VISION_API} LIBRARY_NAME_UPPER_VISION_API)

if (NOT TARGET ids_peak)
    file (TO_CMAKE_PATH "$ENV{IDS_PEAK_SDK_PATH}/api" ${LIBRARY_NAME_UPPER_VISION_API}_PACKAGE_DIR)

    set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${LIBRARY_NAME_UPPER_VISION_API}_PACKAGE_DIR}/cmake_finder")
    message (STATUS "[${PROJECT_NAME}] Will find IDS peak API library.. CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    find_package (ids_peak REQUIRED)
endif ()

set (LIBRARY_NAME_LIBIMG "ids_peak_ipl")
string (TOUPPER ${LIBRARY_NAME_LIBIMG} LIBRARY_NAME_LIBIMG_UPPER)
if (NOT TARGET ids_peak_ipl)
    file (TO_CMAKE_PATH "$ENV{IDS_PEAK_SDK_PATH}/ipl" ${LIBRARY_NAME_LIBIMG_UPPER}_PACKAGE_DIR)

    set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${LIBRARY_NAME_LIBIMG_UPPER}_PACKAGE_DIR}/cmake_finder")
    message (STATUS "[${PROJECT_NAME}] Will find IDS peak IPL library.. CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    find_package (ids_peak_ipl REQUIRED)
endif ()

target_include_directories (${SAMPLE_TARGET_NAME}
    PRIVATE ${${LIBRARY_NAME_UPPER_VISION_API}_INCLUDE_DIR}
    PRIVATE ${${LIBRARY_NAME_LIBIMG_UPPER}_INCLUDE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries (${SAMPLE_TARGET_NAME}
    ids_peak_dotnet
    ids_peak_ipl_dotnet
)

# Set output directories for all configuration types (Debug, Release, etc.)
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif()
if (NOT CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
endif ()
if (CMAKE_CONFIGURATION_TYPES)
    foreach (CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string (TOUPPER ${CONFIGURATION_TYPE} CONFIGURATION_TYPE_UPPER)
        if (CONFIGURATION_TYPE_UPPER STREQUAL "RELEASE")
            set (SAMPLE_RUNTIME_OUTPUT_NAME ${SAMPLE_TARGET_NAME})
            set (SAMPLE_RUNTIME_OUTPUT_DIRECTORY ${SAMPLE_OUPUT_PATH}/${ARCH})
        else ()
            string (TOLOWER ${CONFIGURATION_TYPE} CONFIGURATION_TYPE_LOWER)
            set (SAMPLE_RUNTIME_OUTPUT_NAME "${SAMPLE_TARGET_NAME}_${CONFIGURATION_TYPE_LOWER}")
            set (SAMPLE_RUNTIME_OUTPUT_DIRECTORY ${SAMPLE_OUPUT_PATH}/${ARCH}/${CONFIGURATION_TYPE})
        endif ()
        set_target_properties (${SAMPLE_TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_NAME_${CONFIGURATION_TYPE_UPPER} ${SAMPLE_RUNTIME_OUTPUT_NAME}
            RUNTIME_OUTPUT_DIRECTORY_${CONFIGURATION_TYPE_UPPER} ${SAMPLE_RUNTIME_OUTPUT_DIRECTORY}
        )
        message (STATUS "[${PROJECT_NAME}] Cfg ${CONFIGURATION_TYPE} -> Output directory: ${SAMPLE_RUNTIME_OUTPUT_DIRECTORY}, Output name: ${SAMPLE_RUNTIME_OUTPUT_NAME}")
    endforeach ()
endif ()

set_target_properties(${SAMPLE_TARGET_NAME} PROPERTIES
    VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6"  # .NET Framework version 4.6 corresponds to C# 6.0
    VS_DOTNET_REFERENCES
        "Microsoft.CSharp;PresentationCore;System;System.Core;System.Data;System.Drawing;System.Windows.Forms;WindowsBase"
    FOLDER "API/DotNet"
)

GET_PROPERTY(${LIBRARY_NAME_UPPER_VISION_API}_LIBRARIES_COPIED_LOCAL GLOBAL PROPERTY ${LIBRARY_NAME_UPPER_VISION_API}_LIBRARIES_COPIED)
if(NOT ${LIBRARY_NAME_UPPER_VISION_API}_LIBRARIES_COPIED_LOCAL)
    file (GLOB ids_peak_LIBS
        "${${LIBRARY_NAME_UPPER_VISION_API}_LIBRARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    foreach (ids_peak_LIBRARY ${ids_peak_LIBS})
        message (STATUS "[${PROJECT_NAME}] Add PostBuildStep for copy of ${ids_peak_LIBRARY}.")
        add_custom_command (TARGET ${SAMPLE_TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ids_peak_LIBRARY}
            $<TARGET_FILE_DIR:${SAMPLE_TARGET_NAME}>
            COMMENT "Post build copy of ${ids_peak_LIBRARY} to output dir." VERBATIM
        )
    endforeach ()
    SET_PROPERTY(GLOBAL PROPERTY ${LIBRARY_NAME_UPPER_VISION_API}_LIBRARIES_COPIED ON)
endif()
GET_PROPERTY(${LIBRARY_NAME_UPPER_VISION_API}_DOTNET_LIBRARIES_COPIED_LOCAL GLOBAL PROPERTY ${LIBRARY_NAME_UPPER_VISION_API}_DOTNET_LIBRARIES_COPIED)
if(NOT ${LIBRARY_NAME_UPPER_VISION_API}_DOTNET_LIBRARIES_COPIED_LOCAL)
    add_custom_command (TARGET ${SAMPLE_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PROPERTY:ids_peak_dotnet,INTERFACE_LINK_LIBRARY>
        $<TARGET_FILE_DIR:${SAMPLE_TARGET_NAME}>
        COMMENT "Post build copy of ids_peak_dotnet DLLs to output dir." VERBATIM
    )
    SET_PROPERTY(GLOBAL PROPERTY ${LIBRARY_NAME_UPPER_VISION_API}_DOTNET_LIBRARIES_COPIED ON)
endif()

GET_PROPERTY(${LIBRARY_NAME_LIBIMG_UPPER}_LIBRARIES_COPIED_LOCAL GLOBAL PROPERTY ${LIBRARY_NAME_LIBIMG_UPPER}_LIBRARIES_COPIED)
if(NOT ${LIBRARY_NAME_LIBIMG_UPPER}_LIBRARIES_COPIED_LOCAL)
    file (GLOB ids_peak_ipl_LIBS
        "${${LIBRARY_NAME_LIBIMG_UPPER}_LIBRARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    foreach (ids_peak_ipl_LIBRARY ${ids_peak_ipl_LIBS})
        message (STATUS "[${PROJECT_NAME}] Add PostBuildStep for copy of ${ids_peak_ipl_LIBRARY}.")
        add_custom_command (TARGET ${SAMPLE_TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ids_peak_ipl_LIBRARY}
            $<TARGET_FILE_DIR:${SAMPLE_TARGET_NAME}>
            COMMENT "Post build copy of ${ids_peak_ipl_LIBRARY} to output dir." VERBATIM
        )
    endforeach ()
SET_PROPERTY(GLOBAL PROPERTY ${LIBRARY_NAME_LIBIMG_UPPER}_LIBRARIES_COPIED ON)
endif()
GET_PROPERTY(${LIBRARY_NAME_LIBIMG_UPPER}_DOTNET_LIBRARIES_COPIED_LOCAL GLOBAL PROPERTY ${LIBRARY_NAME_LIBIMG_UPPER}_DOTNET_LIBRARIES_COPIED)
if(NOT ${LIBRARY_NAME_LIBIMG_UPPER}_DOTNET_LIBRARIES_COPIED_LOCAL)
    add_custom_command (TARGET ${SAMPLE_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PROPERTY:ids_peak_ipl_dotnet,INTERFACE_LINK_LIBRARY>
        $<TARGET_FILE_DIR:${SAMPLE_TARGET_NAME}>
        COMMENT "Post build copy of ids_peak_ipl_dotnet DLLs to output dir." VERBATIM
    )
    SET_PROPERTY(GLOBAL PROPERTY ${LIBRARY_NAME_LIBIMG_UPPER}_DOTNET_LIBRARIES_COPIED ON)
endif()

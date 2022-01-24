cmake_minimum_required (VERSION 2.8.12)

project (samples_csharp)

if (NOT UNIX)
    add_subdirectory (open_camera)
    add_subdirectory (simple_live_windows_forms)
    add_subdirectory (simple_live_wpf)
endif ()

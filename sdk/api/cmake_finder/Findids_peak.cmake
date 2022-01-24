message (VERBOSE "[${PROJECT_NAME}] Processing ${CMAKE_CURRENT_LIST_FILE}")

# Macro definitions - start
function (LIBRARY_FOUND libname)
    string (TOUPPER ${libname} libname_upper)
    set (${libname_upper}_FOUND on CACHE BOOL "Found ${libname_upper}-${libname}")
endfunction ()

function (LIBRARY_NOT_FOUND libname)
    string (TOUPPER ${libname} libname_upper)
    set (${libname_upper}_FOUND off CACHE BOOL "Found ${libname_upper}-${libname}")
endfunction ()

macro (ADD_LIBRARY_WINDOWS libname)
    file (GLOB LIBRARY_LIB_SHARED_RELEASE
        "${LIBRARY_LIB_DIR}/*${libname}*${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    file (GLOB LIBRARY_LIB_STATIC_RELEASE
        "${LIBRARY_LIB_DIR}/*${libname}*.lib"
    )

    if (LIBRARY_LIB_SHARED_RELEASE)
        message (STATUS "Found ${libname}: Release ${ARCH}")
        LIBRARY_FOUND(${libname})
        if (NOT ${LIBRARY_NAME_UPPER}_DYNAMIC_LOADING)
            add_library (${libname} SHARED IMPORTED GLOBAL)
            set_target_properties (${libname} PROPERTIES
                IMPORTED_IMPLIB               ${LIBRARY_LIB_STATIC_RELEASE}
                IMPORTED_LOCATION             ${LIBRARY_LIB_SHARED_RELEASE}
            )
        else()
            add_library (${libname} INTERFACE IMPORTED GLOBAL)
            target_compile_definitions(${libname}
                INTERFACE "PEAK_DYNAMIC_LOADING"
            )
        endif()
        target_include_directories (${libname} 
            INTERFACE ${LIBRARY_INCLUDE_DIR}
        )
    else ()
        message (STATUS "Not Found ${libname}: Release ${ARCH}")
        LIBRARY_NOT_FOUND (${libname})
    endif ()
endmacro ()

macro (ADD_LIBRARY_WINDOWS_DOTNET libname)
    file (GLOB LIBRARY_LIB_SHARED_RELEASE
        "${LIBRARY_BINDING_DIR}/dotnet/${ARCH}/*${libname}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    file (GLOB LIBRARY_LIB_SHARED_INTERFACE_RELEASE
        "${LIBRARY_BINDING_DIR}/dotnet/${ARCH}/*${libname}_interface${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )

    if (LIBRARY_LIB_SHARED_RELEASE)
        message (STATUS "Found ${libname}: Release ${ARCH}")
        LIBRARY_FOUND(${libname})
        add_library (${libname} UNKNOWN IMPORTED GLOBAL)
        set_target_properties (${libname} PROPERTIES
            IMPORTED_LOCATION ${LIBRARY_LIB_SHARED_RELEASE}
            INTERFACE_LINK_LIBRARY ${LIBRARY_LIB_SHARED_INTERFACE_RELEASE}
        )
    else ()
        message (STATUS "Not Found ${libname}: Release ${ARCH}")
        LIBRARY_NOT_FOUND (${libname})
    endif ()
endmacro ()

macro (ADD_LIBRARY_LINUX libname)
    file (GLOB LIBRARY_LIB_SHARED_RELEASE
        "${LIBRARY_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${libname}*${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )

    if (LIBRARY_LIB_SHARED_RELEASE)
        message (STATUS "Found ${libname}: Release ${ARCH}")
        LIBRARY_FOUND (${libname})
        if (NOT ${LIBRARY_NAME_UPPER}_DYNAMIC_LOADING)
            add_library (${libname} SHARED IMPORTED GLOBAL)
            set_target_properties (${libname} PROPERTIES
                IMPORTED_LOCATION ${LIBRARY_LIB_SHARED_RELEASE}
            )
        else()
            add_library (${libname} INTERFACE IMPORTED GLOBAL)
            target_compile_definitions(${libname}
                INTERFACE "PEAK_DYNAMIC_LOADING"
            )
        endif()
        set_target_properties (${libname} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${LIBRARY_INCLUDE_DIR}
        )

    else ()
        message (WARNING "Not Found ${libname}: Release ${ARCH}")
        LIBRARY_NOT_FOUND (${libname})
    endif ()
endmacro ()

macro (LIBRARY_ADD_LIBRARY libname)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        ADD_LIBRARY_WINDOWS(${libname})
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        ADD_LIBRARY_LINUX(${libname})
    else ()
        message (FATAL_ERROR "Find${LIBRARY_NAME}: Unknown compiler ID")
    endif ()
endmacro ()

macro (LIBRARY_ADD_LIBRARY_DOTNET libname)
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        ADD_LIBRARY_WINDOWS_DOTNET(${libname})
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message (WARNING "Find${LIBRARY_NAME}: ${LIBRARY_NAME}_dotnet for Linux not yet supported ...")
    else ()
        message (FATAL_ERROR "Find${LIBRARY_NAME}: Unknown compiler ID ...")
    endif ()
endmacro ()
# Macro definitions - end

# Finder script - start
set (LIBRARY_NAME "ids_peak")
string (TOUPPER ${LIBRARY_NAME} LIBRARY_NAME_UPPER)

set (LIBRARY_INTERFACE_NAME "peak")
find_path (${LIBRARY_NAME_UPPER}_PROJECT_DIR include/${LIBRARY_INTERFACE_NAME}/${LIBRARY_INTERFACE_NAME}.hpp
    HINTS   "${CMAKE_CURRENT_LIST_DIR}/../"
)

if (${LIBRARY_NAME_UPPER}_PROJECT_DIR)
    message(STATUS "Found ${LIBRARY_NAME}: ${${LIBRARY_NAME_UPPER}_PROJECT_DIR}")

    include (${CMAKE_CURRENT_LIST_DIR}/cmake_detect_architecture.cmake)
    detect_target_architecture (ARCH)
    
    set(${LIBRARY_NAME_UPPER}_DYNAMIC_LOADING FALSE CACHE BOOL "Enable dynamic loading for library")

    set (LIBRARY_PROJECT_DIR                "${${LIBRARY_NAME_UPPER}_PROJECT_DIR}")
    set (${LIBRARY_NAME_UPPER}_INCLUDE_DIR  "${LIBRARY_PROJECT_DIR}/include" CACHE PATH "${LIBRARY_NAME} include directory")
    set (${LIBRARY_NAME_UPPER}_LIBRARY_DIR  "${LIBRARY_PROJECT_DIR}/lib/${ARCH}" CACHE PATH "${LIBRARY_NAME} ${ARCH} library directory")
    set (${LIBRARY_NAME_UPPER}_BINDING_DIR  "${LIBRARY_PROJECT_DIR}/binding" CACHE PATH "${LIBRARY_NAME} binding directory")
    set (LIBRARY_INCLUDE_DIR                "${${LIBRARY_NAME_UPPER}_INCLUDE_DIR}")
    set (LIBRARY_LIB_DIR                    "${${LIBRARY_NAME_UPPER}_LIBRARY_DIR}")
    set (LIBRARY_BINDING_DIR                "${${LIBRARY_NAME_UPPER}_BINDING_DIR}")

    LIBRARY_ADD_LIBRARY (${LIBRARY_NAME})
    LIBRARY_ADD_LIBRARY_DOTNET (${LIBRARY_NAME}_dotnet)
else ()
    message(WARNING "Not Found ${LIBRARY_NAME}!")
endif ()
# Finder script - end

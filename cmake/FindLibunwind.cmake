include(FindPackageHandleStandardArgs)

message("Root  ${Libunwind_ROOT_DIR}")

find_path(Libunwind_INCLUDE_DIR
    NAMES libunwind.h
    HINTS ${Libunwind_ROOT_DIR}/include ${Libunwind_INCLUDEDIR}
)

find_library(Libunwind_LIBRARY NAMES unwind HINTS ${Libunwind_ROOT_DIR}/lib)
find_library(Libunwind_LIBRARY_DEBUG NAMES unwindd HINTS ${Libunwind_ROOT_DIR}/lib)

find_library(LibunwindArm_LIBRARY NAMES unwind-arm HINTS ${Libunwind_ROOT_DIR}/lib)
find_library(LibunwindArm_LIBRARY_DEBUG NAMES unwind-armd HINTS ${Libunwind_ROOT_DIR}/lib)

if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    find_library(LibunwindNative_LIBRARY NAMES unwind-x86_64 HINTS ${Libunwind_ROOT_DIR}/lib)
    find_library(LibunwindNative_DEBUG_LIBRARY NAMES unwind-x86_64d HINTS ${Libunwind_ROOT_DIR}/lib)
ELSE ()
    find_library(LibunwindNative_LIBRARY NAMES unwind-x86 HINTS ${Libunwind_ROOT_DIR}/lib)
    find_library(LibunwindNative_DEBUG_LIBRARY NAMES unwind-x86d HINTS ${Libunwind_ROOT_DIR}/lib)
endif()

message(STATUS "Libunwind library: ${Libunwind_LIBRARY}")

find_package_handle_standard_args(Libunwind
    FOUND_VAR Libunwind_FOUND
    REQUIRED_VARS Libunwind_INCLUDE_DIR Libunwind_LIBRARY
)

mark_as_advanced(
    Libunwind_ROOT_DIR
    Libunwind_INCLUDE_DIR
    Libunwind_LIBRARY
    Libunwind_LIBRARY_DEBUG
    LibunwindArm_LIBRARY
    LibunwindArm_LIBRARY_DEBUG
    LibunwindNative_LIBRARY
    LibunwindNative_LIBRARY_DEBUG
)

if(Libunwind_FOUND AND NOT TARGET Libunwind::Libunwind)
    add_library(Libunwind::Libunwind UNKNOWN IMPORTED)
    set_target_properties(Libunwind::Libunwind PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Libunwind_INCLUDE_DIR}"
        IMPORTED_LOCATION ${Libunwind_LIBRARY}
    )

    if(Libunwind_LIBRARY_DEBUG)
        set_target_properties(Libunwind::Libunwind PROPERTIES
            IMPORTED_LOCATION_DEBUG "${Libunwind_LIBRARY_DEBUG}"
        )
    endif()
endif()

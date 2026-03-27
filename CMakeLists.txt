# CMakeList.txt : CMake project for SimonLepetitEngine, include source and define
# project specific logic here.
#

macro(SetOption var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

cmake_minimum_required (VERSION 3.8)
project(SimonLepetitEngine LANGUAGES CXX)

include(cmake/Config.cmake)
include(cmake/Macros.cmake)

include(GNUInstallDirs)

SetOption(BUILD_WINDOW ON BOOL "ON to build Window_cls module. This setting is ignored, if the graphics module is built.")

if(BUILD_WINDOW)

    SetOption(OPENGL_ES ${OPENGL_ES} BOOL "ON to use an OpenGL ES implementation, OFF to use a desktop OpenGL implementation")

    if(PLATFORM_LINUX)
        SetOption(USE_DRM OFF BOOL "ON to use DRM windowing backend")
    endif()
endif()

if(PLATFORM_WINDOWS)
    set(DEFAULT_INSTALL_MISC_DIR .)
elseif(PLATFORM_LINUX)
    set(DEFAULT_INSTALL_MISC_DIR share/SLE)
endif()


if(PLATFORM_WINDOWS)
    set(USE_SYSTEM_DEPS_DEFAULT OFF)
else()
    set(USE_SYSTEM_DEPS_DEFAULT ON)
endif()

SetOption(USE_SYSTEM_DEPS ${USE_SYSTEM_DEPS_DEFAULT} BOOL "ON to use system dependencies, OFF to use the bundled ones.")

if(COMPILER_MSVC)

    SetOption(GENERATE_PDB ON BOOL "True to generate PDB debug symbols, OFF otherwise.")

    if(CMAKE_CXX_COMPILER_LAUNCHER)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
    endif()
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(INSTALL_PKGCONFIG_DEFAULT OFF)

if(PLATFORM_LINUX)
    set(INSTALL_PKGCONFIG_DEFAULT ON)
endif()

SetOption(INSTALL_PKGCONFIG_FILES ${INSTALL_PKGCONFIG_DEFAULT} BOOL "ON to automatically install pkg-config files so other projects can find SLE")

if(INSTALL_PKGCONFIG_FILES)
    # account for CMAKE_INSTALL_LIBDIR potentially being an absolute path
    file(RELATIVE_PATH RELATIVE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX} ${CMAKE_INSTALL_FULL_LIBDIR})

    # set pkgconfig install directory
    # this could be e.g. macports on mac or msys2 on windows etc.
    set(PKGCONFIG_DIR "${RELATIVE_INSTALL_LIBDIR}/pkgconfig")

    sfml_SetOption(PKGCONFIG_INSTALL_DIR "${PKGCONFIG_DIR}" PATH "Install directory for SLE's pkg-config .pc files")

    if(BUILD_AUDIO AND BUILD_NETWORK AND BUILD_GRAPHICS AND BUILD_WINDOW)
        configure_file(
            "tools/pkg-config/sle-all.pc.in"
            "tools/pkg-config/sle-all.pc"
            @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tools/pkg-config/sle-all.pc"
            DESTINATION "${PKGCONFIG_INSTALL_DIR}")
    endif()
endif()

SetOption(ENABLE_PCH OFF BOOL "ON to enable precompiled headers for SLE builds -- only supported on Windows/Linux and for static library builds")

if(ENABLE_PCH AND BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Precompiled headers are currently not supported for shared library builds")
endif()

add_subdirectory(src/SLE)

file(GLOB HEADERS RELATIVE ${PROJECT_SOURCE_DIR} "include/SLE/*")


add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp VERBATIM)

set(SOURCES ${HEADERS})
list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

# create SLE.framework
add_library(SLE ${SOURCES})

# enable C++17 support
target_compile_features(SLE PUBLIC cxx_std_17)

# set the target flags to use the appropriate C++ standard library
SetSTDLib(SLE)


# add the non-optional SLE headers
#add_custom_command(TARGET SLE POST_BUILD COMMAND cp -r
#                    ${PROJECT_SOURCE_DIR}/include/SLE/Config.hpp
#                    ${PROJECT_SOURCE_DIR}/include/SLE/OpenGL.hpp
#                    ${PROJECT_SOURCE_DIR}/include/SLE/GpuPreference.hpp
#                    ${PROJECT_SOURCE_DIR}/include/SLE/System.hpp
#                    ${PROJECT_SOURCE_DIR}/include/SLE/Main.hpp
#                    ${PROJECT_SOURCE_DIR}/include/SLE/System
#                    $<TARGET_FILE_DIR:SLE>/Headers
#                    VERBATIM)

# add Window_cls module headers if enabled
#if(BUILD_WINDOW)
#    add_custom_command(TARGET SLE POST_BUILD COMMAND cp -r
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Window_cls.hpp
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Window_cls
#                        $<TARGET_FILE_DIR:SLE>/Headers
#                        VERBATIM)
#endif()

# add network module headers if enabled
#if(BUILD_NETWORK)
#    add_custom_command(TARGET SLE POST_BUILD COMMAND cp -r
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Network.hpp
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Network
#                        $<TARGET_FILE_DIR:SLE>/Headers
#                        VERBATIM)
#endif()

# add graphics module headers if enabled
#if(BUILD_GRAPHICS)
#    add_custom_command(TARGET SLE POST_BUILD COMMAND cp -r
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Graphics.hpp
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Graphics
#                        $<TARGET_FILE_DIR:SLE>/Headers
#                        VERBATIM)
#endif()

# add audio module headers if enabled
#if(BUILD_AUDIO)
#    add_custom_command(TARGET SLE POST_BUILD COMMAND cp -r
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Audio.hpp
#                        ${PROJECT_SOURCE_DIR}/include/SLE/Audio
#                        $<TARGET_FILE_DIR:SLE>/Headers
#                        VERBATIM)
#endif()

# adapt install directory to allow distributing dylibs/frameworks in user's frameworks/application bundle
# NOTE: it's not required to link against SLE.framework
set_target_properties(SLE PROPERTIES INSTALL_NAME_DIR "@rpath")
if(NOT CMAKE_SKIP_BUILD_RPATH)
    set_target_properties(SLE PROPERTIES BUILD_WITH_INSTALL_NAME_DIR ON)
endif()

# install rule
install(TARGETS SLE FRAMEWORK DESTINATION "." COMPONENT devel)

ExportTargets()

SetOption(CONFIGURE_EXTRAS ${PROJECT_IS_TOP_LEVEL} BOOL "ON to configure extras, OFF to ignore them")

if(NOT CONFIGURE_EXTRAS)
    return()
endif()

include(CPack)

SetOption(CLANG_FORMAT_EXECUTABLE clang-format STRING "Override clang-format executable, requires version 17")
add_custom_target(format
    COMMAND ${CMAKE_COMMAND} -DCLANG_FORMAT_EXECUTABLE=${CLANG_FORMAT_EXECUTABLE} -P ./cmake/Format.cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM)

SetOption(CLANG_TIDY_EXECUTABLE clang-tidy STRING "Override clang-tidy executable, requires minimum version 14")
add_custom_target(tidy
    COMMAND ${CMAKE_COMMAND} -DCLANG_TIDY_EXECUTABLE=${CLANG_TIDY_EXECUTABLE} -DPROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} -P ./cmake/Tidy.cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM)
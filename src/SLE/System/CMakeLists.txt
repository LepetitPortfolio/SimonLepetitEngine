message(STATUS "System - CMkeList")

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SLE/System)

file(GLOB_RECURSE PROJECT_SOURCES
  "${SRCROOT}/*.cpp"
  "${SRCROOT}/*.cxx"
  "${SRCROOT}/*.cc"
)

# Also add headers to the IDE view (optional)
file(GLOB_RECURSE PROJECT_HEADERS
  "${SRCROOT}/*.h"
  "${SRCROOT}/*.hpp"
)

AddLibrary(System SOURCES ${PROJECT_SOURCES} DEPENDENCIES "Dependencies.cmake")

if(PLATFORM_LINUX)
    if(USE_DRM)
        find_package(DRM REQUIRED)
        find_package(GBM REQUIRED)
        target_link_libraries(sle-system  PRIVATE DRM::DRM GBM::GBM)
    else()
        find_package(X11 REQUIRED COMPONENTS Xrandr Xcursor Xi)
        target_link_libraries(sle-system  PRIVATE X11::X11 X11::Xrandr X11::Xcursor X11::Xi)
    endif()
endif()

target_link_libraries(sle-system PUBLIC sle-common)

target_include_directories(sle-system SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/glad/include")

target_include_directories(sle-system SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/vulkan")

if(NOT OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE "LEGACY")
endif()

if(PLATFORM_ANDROID)
    find_package(EGL REQUIRED)
    find_package(GLES REQUIRED)
    target_link_libraries(sle-system PRIVATE EGL::EGL GLES::GLES)
else()
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    target_link_libraries(sle-system PRIVATE OpenGL::GL)
endif()

if(PLATFORM_WINDOWS AND NOT COMPILER_MSVC)
    include(CheckIncludeFileCXX)
    check_include_file_cxx(dinput.h DINPUT_H_FOUND)
    if(NOT DINPUT_H_FOUND)
        target_include_directories(System SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/mingw")
    endif()
endif()

if(PLATFORM_WINDOWS)
    find_package(UDev REQUIRED)
    target_link_libraries(System PRIVATE UDev::UDev dl)
elseif(PLATFORM_WINDOWS)
    target_link_libraries(System PRIVATE winmm gdi32)
elseif(PLATFORM_ANDROID)
    target_link_libraries(System PRIVATE android)
endif()

if(COMPILER_GCC)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <atomic>
        int main(){std::atomic_ullong x(1); return x.fetch_add(1);}" ATOMIC_TEST)
    if(NOT ATOMIC_TEST)
        target_link_libraries(sle-system PRIVATE atomic)
    endif()
endif()
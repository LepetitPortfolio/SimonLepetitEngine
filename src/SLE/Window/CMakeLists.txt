message(STATUS "Window - CMkeList")

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SLE/Window)

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

if(PLATFORM_WINDOWS)
    set(PLATFORM_SRC
        #${SRCROOT}/Win32/CursorImpl.hpp
        #${SRCROOT}/Win32/CursorImpl.cpp
        #${SRCROOT}/Win32/ClipboardImpl.hpp
        #${SRCROOT}/Win32/ClipboardImpl.cpp
        #${SRCROOT}/Win32/InputImpl.cpp
        #${SRCROOT}/Win32/JoystickImpl.cpp
        #${SRCROOT}/Win32/JoystickImpl.hpp
        #${SRCROOT}/Win32/SensorImpl.hpp
        #${SRCROOT}/Win32/SensorImpl.cpp
        #${SRCROOT}/Win32/Utils.hpp
        #${SRCROOT}/Win32/VideoModeImpl.cpp
        #${SRCROOT}/Win32/VulkanImplWin32.cpp
        #${SRCROOT}/Win32/WindowImplWin32.cpp
        #${SRCROOT}/Win32/WindowImplWin32.hpp
    )
    if(OPENGL_ES)
        list(APPEND PLATFORM_SRC
            #${SRCROOT}/EGLCheck.cpp
            #${SRCROOT}/EGLCheck.hpp
            #${SRCROOT}/EglContext.cpp
            #${SRCROOT}/EglContext.hpp
        )
    else()
        list(APPEND PLATFORM_SRC
            #${SRCROOT}/Win32/WglContext.cpp
            #${SRCROOT}/Win32/WglContext.hpp
        )
    endif()
    source_group("windows" FILES ${PLATFORM_SRC})
elseif(PLATFORM_LINUX)
    set(PLATFORM_SRC
        #${SRCROOT}/Unix/CursorImpl.hpp
        #${SRCROOT}/Unix/CursorImpl.cpp
        #${SRCROOT}/Unix/ClipboardImpl.hpp
        #${SRCROOT}/Unix/ClipboardImpl.cpp
        #${SRCROOT}/Unix/InputImpl.cpp
        #${SRCROOT}/Unix/KeyboardImpl.hpp
        #${SRCROOT}/Unix/KeyboardImpl.cpp
        #${SRCROOT}/Unix/KeySymToKeyMapping.hpp
        #${SRCROOT}/Unix/KeySymToKeyMapping.cpp
        #${SRCROOT}/Unix/KeySymToUnicodeMapping.hpp
        #${SRCROOT}/Unix/KeySymToUnicodeMapping.cpp
        #${SRCROOT}/Unix/SensorImpl.cpp
        #${SRCROOT}/Unix/SensorImpl.hpp
        #${SRCROOT}/Unix/Display.cpp
        #${SRCROOT}/Unix/Display.hpp
        #${SRCROOT}/Unix/Utils.hpp
        #${SRCROOT}/Unix/VideoModeImpl.cpp
        #${SRCROOT}/Unix/VulkanImplX11.cpp
        #${SRCROOT}/Unix/WindowImplX11.cpp
        #${SRCROOT}/Unix/WindowImplX11.hpp
    )
    if(OPENGL_ES)
        list(APPEND PLATFORM_SRC
            #${SRCROOT}/EGLCheck.cpp
            #${SRCROOT}/EGLCheck.hpp
            #${SRCROOT}/EglContext.cpp
            #${SRCROOT}/EglContext.hpp
        )
    else()
            list(APPEND PLATFORM_SRC
            #${SRCROOT}/Unix/GlxContext.cpp
            #${SRCROOT}/Unix/GlxContext.hpp
        )
    endif()
    if(PLATFORM_LINUX)
        list(APPEND PLATFORM_SRC
            #${SRCROOT}/Unix/JoystickImpl.cpp
            #${SRCROOT}/Unix/JoystickImpl.hpp
        )
    endif()
    source_group("unix" FILES ${PLATFORM_SRC})
endif()

AddLibrary(Window SOURCES ${PROJECT_SOURCES} ${PLATFORM_SRC} DEPENDENCIES "Dependencies.cmake")

if(PLATFORM_LINUX)
    if(USE_DRM)
        find_package(DRM REQUIRED)
        find_package(GBM REQUIRED)
        target_link_libraries(Window PRIVATE DRM::DRM GBM::GBM)
    else()
        find_package(X11 REQUIRED COMPONENTS Xrandr Xcursor Xi)
        target_link_libraries(Window PRIVATE X11::X11 X11::Xrandr X11::Xcursor X11::Xi)
    endif()
endif()

target_link_libraries(sle-window PUBLIC sle-common)

target_include_directories(sle-window SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/glad/include")

target_include_directories(sle-window SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/extlibs/headers/vulkan")

if(NOT OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE "LEGACY")
endif()

if(PLATFORM_ANDROID)
    find_package(EGL REQUIRED)
    find_package(GLES REQUIRED)
    target_link_libraries(sle-window PRIVATE EGL::EGL GLES::GLES)
else()
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    target_link_libraries(sle-window PRIVATE OpenGL::GL)
endif()

if(PLATFORM_WINDOWS AND NOT COMPILER_MSVC)
    include(CheckIncludeFileCXX)
    check_include_file_cxx(dinput.h DINPUT_H_FOUND)
    if(NOT DINPUT_H_FOUND)
        target_include_directories(Window SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/mingw")
    endif()
endif()

if(PLATFORM_WINDOWS)
    find_package(UDev REQUIRED)
    target_link_libraries(Window PRIVATE UDev::UDev dl)
elseif(PLATFORM_WINDOWS)
    target_link_libraries(Window PRIVATE winmm gdi32)
elseif(PLATFORM_ANDROID)
    target_link_libraries(Window PRIVATE android)
endif()

if(COMPILER_GCC)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <atomic>
        int main(){std::atomic_ullong x(1); return x.fetch_add(1);}" ATOMIC_TEST)
    if(NOT ATOMIC_TEST)
        target_link_libraries(sle-window PRIVATE atomic)
    endif()
endif()
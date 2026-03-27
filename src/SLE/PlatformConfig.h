#pragma once
#if _WIN32 || _WIN64
    #define PLATFORM_WINDOWS 1

    #include <windows.h>
    #include <winuser.h>

    #include <dinput.h>
    #include <mmsystem.h>

    struct HWND__;
using WindowHandle = HWND__*;

#elif __linux__

    #define PLATFORM_LINUX 1

    #include <X11/Xatom.h>
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysym.h>

    #include <X11/Xlibint.h>

    #include <X11/Xcursor/Xcursor.h>
    #include <X11/cursorfont.h>

    #include <X11/extensions/XInput2.h>
    #include <X11/extensions/Xrandr.h>

    #include<glad/glx.h>

    #include <linux/input.h>

    using WindowHandle = unsigned long;

    template <typename T>
    struct XDeleter
    {
        void operator()(T* _Data) const
        {
            XFree(_Data);
        }
    };

    template <typename T>
    using X11Ptr = std::unique_ptr<T, XDeleter<std::remove_all_extents_t<T>>>;

#else 

    #error "Unsupported platform"

#endif
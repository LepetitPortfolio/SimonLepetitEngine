#include "../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "WindowUnix_cls.h"

#include "../../../Common/Error.h"
#include "../../Inputs/Unix/KeyboardUnix_cls.h"

#include <algorithm>
#include <array>
#include <fcntl.h>
#include <libgen.h>
#include <ostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <cassert>
#include <cstring>

template <>
struct XDeleter<XImage>
{
    void operator()(XImage* _Image) const
    {
        XDestroyImage(_Image);
    }
};


template <>
struct XDeleter<XRRScreenResources>
{
    void operator()(XRRScreenResources* _Resources) const
    {
        XRRFreeScreenResources(_Resources);
    }
};


template <>
struct XDeleter<XRROutputInfo>
{
    void operator()(XRROutputInfo* _OutputInfo) const
    {
        XRRFreeOutputInfo(_OutputInfo);
    }
};


template <>
struct XDeleter<XRRCrtcInfo>
{
    void operator()(XRRCrtcInfo* _CrtcInfo) const
    {
        XRRFreeCrtcInfo(_CrtcInfo);
    }
};


WindowUnix_cls::WindowUnix_cls(WindowHandle _Handle) : m_IsExternal(true)
{
    m_Display = OpenDisplay();
    EwmhSupported();

    m_Screen = DefaultScreen(m_Display.get());

    m_Window = _Handle;

    if (m_Window)
    {
        XSetWindowAttributes attributes;
        attributes.event_mask = m_EventMask;

        XChangeWindowAttributes(m_Display.get(), m_Window, CWEventMask, &attributes);

        SetProtocols();

        Initialize();
    }    
}

WindowUnix_cls::WindowUnix_cls(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState_e _State, const WindowSettings_str& _WindowSettings) :
            m_Fullscreen(_State == WindowState_e::Fullscreen), m_Cursor_clsGrabbed(m_Fullscreen)
{
    

    m_Display = OpenDisplay();
    EwmhSupported();

    m_Screen = DefaultScreen(m_Display.get());

    Vector2i windowPosition;
    if (m_Fullscreen)
    {
        windowPosition = GetPrimaryMonitorPosition();
    }
    else
    {
        Vector2i displaySize(DisplayWidth(m_Display.get(), m_Screen), DisplayHeight(m_Display.get(), m_Screen));
        windowPosition = displaySize - Vector2i(_WindowConfig.m_Size) / 2;
    }

    const unsigned int width = _WindowConfig.m_Size.X;
    const unsigned int height = _WindowConfig.m_Size.Y;

    Visual* visual = nullptr;
    int depth = 0;

    if (_WindowSettings.AttributeFlags == 0xFFFFFFFF)
    {
        // Choose default visual since the user is going to use their own rendering API
        visual = DefaultVisual(m_Display.get(), m_Screen);
        depth = DefaultDepth(m_Display.get(), m_Screen);
    }
    else
    {
        const XVisualInfo visualInfo = ContextType::selectBestVisual(m_Display.get(), _WindowConfig.m_BitsPerPixel, _WindowSettings);// /!\ openGL !!!!

        visual = visualInfo.visual;
        depth = visualInfo.depth;
    }

    XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap(m_Display.get(), DefaultRootWindow(m_Display.get()), visual, AllocNone);
    attributes.event_mask = m_EventMask;
    attributes.override_redirect = (m_Fullscreen && EwmhSupported()) ? True : False;

    m_Window = XCreateWindow(m_Display.get(), DefaultRootWindow(m_Display.get()), windowPosition.X, windowPosition.Y, width, height, 0, depth, InputOutput,
                             visual, CWEventMask | CWOverrideRedirect | CWColormap, &attributes);

    if (!m_Window)
    {
        Err() << "Failed to create Window_cls" << std::endl;
        return;
    }

    SetProtocols();

    XWMHints xHints{};
    xHints.flags = StateHint;
    xHints.initial_state = NormalState;
    XSetWMHints(m_Display.get(), m_Window, &xHints);

    if (m_Fullscreen)
    {
        if (const Atom wmHintsAtom = GetAtom("_MOTIF_WM_HINTS", false))
        {
            unsigned long MWM_HINTS_FUNCTIONS = 1 << 0;
            unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

            unsigned long MWM_DECOR_BORDER = 1 << 1;
            unsigned long MWM_DECOR_RESIZEH = 1 << 2;
            unsigned long MWM_DECOR_TITLE = 1 << 3;
            unsigned long MWM_DECOR_MENU = 1 << 4;
            unsigned long MWM_DECOR_MINIMIZE = 1 << 5;
            unsigned long MWM_DECOR_MAXIMIZE = 1 << 6;

            unsigned long MWM_FUNC_RESIZE = 1 << 1;
            unsigned long MWM_FUNC_MOVE = 1 << 2;
            unsigned long MWM_FUNC_MINIMIZE = 1 << 3;
            unsigned long MWM_FUNC_MAXIMIZE = 1 << 4;
            unsigned long MWM_FUNC_CLOSE = 1 << 5;
            
            struct WMHints
            {
                unsigned long Flags{ MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS };
                unsigned long Functions{};
                unsigned long Decorations{};
                long InputMode{};
                unsigned long State{};
            } hints;

            if (_Style & WindowStyle_e::Titlebar)
            {
                hints.Decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
                hints.Functions |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE;
            }

            if (_Style & WindowStyle_e::Resize)
            {
                hints.Decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
                hints.Functions |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
            }

            if (_Style & WindowStyle_e::Close)
            {
                hints.Decorations |= 0;
                hints.Functions |= MWM_FUNC_CLOSE;
            }

            XChangeProperty(m_Display.get(), m_Window, wmHintsAtom, wmHintsAtom, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&hints), 5);
        }
    }

    if (_Style & WindowStyle_e::Resize)
    {
        m_UseSizeHints = true;
        XSizeHints sizeHints{};
        sizeHints.flags = PMinSize | PMaxSize | USPosition;
        sizeHints.min_width = sizeHints.max_width = static_cast<int>(width);
        sizeHints.min_height = sizeHints.max_height = static_cast<int>(height);
        sizeHints.x = windowPosition.X;
        sizeHints.y = windowPosition.Y;
        XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);
    }

    XClassHint hints{};

    std::string executableName = FindExecutableName().string();
    std::vector<char> windowInstance(executableName.size() + 1, 0);
    std::copy(executableName.begin(), executableName.end(), windowInstance.begin());
    hints.res_name = windowInstance.data();

    std::string ansiTitle = _Title.ToANSIString();
    std::vector<char> windowClass(ansiTitle.size() + 1, 0);
    std::copy(ansiTitle.begin(), ansiTitle.end(), windowClass.begin());
    hints.res_class = windowClass.data();

    XSetClassHint(m_Display.get(), m_Window, &hints);

    SetTitle(_Title);

    Initialize();

    if (m_Fullscreen)
    {
        XSizeHints sizeHints{};
        long flags = 0;
        XGetWMNormalHints(m_Display.get(), m_Window, &sizeHints, &flags);
        sizeHints.flags &= ~(PMinSize | PMaxSize);
        XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);

        SetWindowConfig(_WindowConfig);
        SwitchToFullscreen();
    }
}

WindowUnix_cls::~WindowUnix_cls()
{ 
    Cleanup();

    if (m_IconPixmap)
    {
        XFreePixmap(m_Display.get(), m_IconPixmap);
    }

    if (m_IconMaskPixmap)
    {
        XFreePixmap(m_Display.get(), m_IconMaskPixmap);
    }

    if (m_HiddenCursor)
    {
        XFreeCursor(m_Display.get(), m_HiddenCursor);
    }

    if (m_InputContext)
    {
        XDestroyIC(m_InputContext);
    }

    // Destroy the Window_cls
    if (m_Window && !m_IsExternal)
    {
        XDestroyWindow(m_Display.get(), m_Window);
        XFlush(m_Display.get());
    }

    const std::lock_guard lock(m_AllWindowsMutex);
    m_AllWindows.erase(std::find(m_AllWindows.begin(), m_AllWindows.end(), this));
}


WindowHandle WindowUnix_cls::GetNativeHandle() const
{
    return m_Window;
}

Vector2u WindowUnix_cls::GetSize() const
{
    XWindowAttributes attributes;
    XGetWindowAttributes(m_Display.get(), m_Window, &attributes);
    return Vector2u(Vector2i(attributes.width, attributes.height));
}

Vector2i WindowUnix_cls::GetPosition() const
{
    Window_cls child = 0;
    int xAbsRelToRoot = 0;
    int yAbsRelToRoot = 0;

    XTranslateCoordinates(m_Display.get(), m_Window, DefaultRootWindow(m_Display.get()), 0, 0, &xAbsRelToRoot, &yAbsRelToRoot, &child);

    if(IsWMAbsolutePositionGood())
    {
        return {xAbsRelToRoot, yAbsRelToRoot};
    }

    long xFrameExtent = 0;
    long yFrameExtent = 0;

    if(GetEWMHFrameExtents(m_Display.get(), m_Window, xFrameExtent, yFrameExtent))
    {
        return {(xAbsRelToRoot - static_cast<int>(xFrameExtent)), (yAbsRelToRoot - static_cast<int>(yFrameExtent))};
    }

    Window_cls ancestor = m_Window;
    Window_cls root = DefaultRootWindow(m_Display.get());

    while(GetParentWindow(m_Display.get(), ancestor)!= root)
    {
        ancestor = GetParentWindow(m_Display.get(), ancestor);
    }

    int xRelToRoot = 0;
    int yRelToRoot = 0;
    unsigned int  width = 0;
    unsigned int  height = 0;
    unsigned int  borderWidth = 0;
    unsigned int  depth = 0;

    XGetGeometry(m_Display.get(), ancestor, &root, &xRelToRoot, &yRelToRoot, &width, &height, &borderWidth, &depth);

    return {xRelToRoot, yRelToRoot};
}

void WindowUnix_cls::SetPosition(Vector2i _Position)
{
    XMoveWindow(m_Display.get(), m_Window, _Position.X, _Position.Y);
    XFlush(m_Display.get());
}

void WindowUnix_cls::SetSize(Vector2u _Size)
{
    if(m_UseSizeHints)
    {
        XSizeHints sizeHints{};
        sizeHints.flags = PMinSize | PMaxSize;
        sizeHints.min_width = sizeHints.max_width = static_cast<int>(_Size.X);
        sizeHints.min_height = sizeHints.max_height = static_cast<int>(_Size.Y);
        XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);
    }

    XResizeWindow(m_Display.get(), m_Window, _Size.X, _Size.Y);
    XFlush(m_Display.get());
}

void WindowUnix_cls::SetMinimumSize(std::optional<Vector2u> _MinimumSize)
{
    WindowPlatform_cls::SetMinimumSize(_MinimumSize);
    SetWindowSizeContraints();
}

void WindowUnix_cls::SetMaximumSize(std::optional<Vector2u> _MaximumSize)
{
    WindowPlatform_cls::SetMaximumSize(_MaximumSize);
    SetWindowSizeContraints();
}

void WindowUnix_cls::SetTitle(const String& _Title)
{
    const auto UTF8Title = _Title.ToUTF8();

    const Atom useUTF8 = GetAtom("UTF8_STRING", false);

    const Atom wmName = GetAtom("_NET_WM_NAME", false);

    XChangeProperty(m_Display.get(), m_Window, wmName, useUTF8, 8, PropModeReplace, UTF8Title.c_str(), static_cast<int>(UTF8Title.size()));

    const Atom wmIconName = GetAtom("_NET_WM_ICON_NAME", false);

    XChangeProperty(m_Display.get(), m_Window, wmIconName, useUTF8, 8, PropModeReplace, UTF8Title.c_str(), static_cast<int>(UTF8Title.size()));

    Xutf8SetWMProperties(m_Display.get(), m_Window, _Title.ToANSIString().c_str(), _Title.ToANSIString().c_str(), nullptr, 0, nullptr, nullptr, nullptr);
}

void WindowUnix_cls::SetIcon(Vector2u _Size, const std::uint8_t* _Pixels)
{
    auto* iconPixels = static_cast<std::uint8_t*>(std::malloc(std::size_t{_Size.X} * std::size_t{_Size.Y} * 4));

    for(std::size_t index = 0; index < std::size_t{_Size.X} * std::size_t{_Size.Y}; index++)
    {
        iconPixels[index * 4 + 0] = _Pixels[index * 4 + 2];
        iconPixels[index * 4 + 1] = _Pixels[index * 4 + 1];
        iconPixels[index * 4 + 2] = _Pixels[index * 4 + 0];
        iconPixels[index * 4 + 3] = _Pixels[index * 4 + 3];
    }

    Visual* defVisual = DefaultVisual(m_Display.get(), m_Screen);
    const auto defDepth = static_cast<unsigned int>(DefaultDepth(m_Display.get(), m_Screen));
    const auto iconImage = X11Ptr<XImage>(XCreateImage(m_Display.get(), defVisual, defDepth, ZPixmap, 0, reinterpret_cast<char*>(iconPixels), _Size.X, _Size.Y, 32, 0));

    if(!iconImage)
    {
        Err() << "Failed to set the Window_cls's icon" << std::endl;
        return;
    }

    if(m_IconPixmap)
    {
        XFreePixmap(m_Display.get(), m_IconPixmap);
    }

    if(m_IconMaskPixmap)
    {
        XFreePixmap(m_Display.get(), m_IconMaskPixmap);
    }

    m_IconPixmap = XCreatePixmap(m_Display.get(), RootWindow(m_Display.get(), m_Screen), _Size.X, _Size.Y, defDepth);
    XGCValues values;
    GC iconGC = XCreateGC(m_Display.get(), m_IconPixmap, 0, &values);
    XPutImage(m_Display.get(), m_IconPixmap, iconGC, iconImage.get(), 0, 0, 0, 0, _Size.X, _Size.Y);
    XFreeGC(m_Display.get(), iconGC);

    const std::size_t pitch = (_Size.X + 7) / 8;
    std::vector<std::uint8_t> maskPixels(pitch * _Size.Y, 0);

    for(std::size_t ySize = 0; ySize < _Size.Y; ySize++)
    {
        for(std::size_t pitchIndex = 0; pitchIndex < pitch; pitchIndex++)
        {
            for(std::size_t index = 0; index < 8; index++)
            {
                const std::uint8_t opacity = _Pixels[(pitchIndex * 8 + index + ySize * _Size.X) * 4 + 3] > 0;
                maskPixels[pitchIndex + ySize * pitch] |= static_cast<std::uint8_t>(opacity << index);
            }
        }
    }

    m_IconMaskPixmap = XCreatePixmapFromBitmapData(m_Display.get(), m_Window, reinterpret_cast<char*>(maskPixels.data()), _Size.X, _Size.Y, 1, 0, 1);

    XWMHints hints{};
    hints.flags = IconPixmapHint | IconMaskHint;
    hints.icon_pixmap = m_IconPixmap;
    hints.icon_mask = m_IconMaskPixmap;
    XSetWMHints(m_Display.get(), m_Window, &hints);

    std::vector<unsigned long> icccmIconPixels(2 + _Size.X * _Size.Y, 0);
    unsigned long* ptr = icccmIconPixels.data();

#pragma GCC dianostic push
#pragma GCC dianostic ignored "-Wnull-dereference"
    *ptr++ = _Size.X;
    *ptr++ = _Size.Y;
#pragma GCC dianostic pop

    for(std::size_t index = 0; index < std::size_t{_Size.X}; index++)
    {
        *ptr++ = static_cast<unsigned long>((_Pixels[index * 4 + 2] << 0) | (_Pixels[index * 4 + 1] << 8)
                                            | (_Pixels[index * 4 + 0] << 16) | (_Pixels[index * 4 + 3] << 24));
    }

    const Atom netWmIcon = GetAtom("8NET_WM_ICON");

    XChangeProperty(m_Display.get(), m_Window, netWmIcon, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(icccmIconPixels.data()), static_cast<int>(2 + _Size.X * _Size.Y));
    XFlush(m_Display.get());
}

void WindowUnix_cls::SetVisible(bool _Visible)
{
    if(_Visible)
    {
        XMapWindow(m_Display.get(), m_Window);

        if(m_Fullscreen)
        {
            SwitchToFullscreen();
        }

        while((!m_WindowMapped) && (!m_IsExternal))
        {
            ProcessEvents();
        }
    }
    else
    {
        XUnmapWindow(m_Display.get(), m_Window);
        XFlush(m_Display.get());

        while((m_WindowMapped) && (!m_IsExternal))
        {
            ProcessEvents();
        }
    }
}

void WindowUnix_cls::SetCursor_clsVisible(bool _Visible)
{
    XDefineCursor_cls(m_Display.get(), m_Window, _Visible ? m_LastCursor_cls : m_HiddenCursor);
    XFlush(m_Display.get());
}

void WindowUnix_cls::SetCursor_clsGrabbed(bool _Grabbed)
{
    if((m_Fullscreen) || (m_Cursor_clsGrabbed == _Grabbed))
    {
        return;
    }

    if(_Grabbed)
    {
        unsigned int trial = 0;
        bool breakLoop = false;

        while((!breakLoop) && (trial < m_MaxTrialsCount))
        {
            const int result = XGrabPointer(m_Display.get(), m_Window, True, None, GrabModeSync, GrabModeSync, m_Window, None, CurrentTime);

            if(result == GrabSuccess)
            {
                m_Cursor_clsGrabbed = true;
                breakLoop = true;
            }
            else
            {
                trial++;
                Sleep(Milliseconds(50));
            }
        }

        if(!m_Cursor_clsGrabbed)
        {
            Err() << "Failed to grab mouse Cursor_cls" << std::endl;
        }
        
    }
    else
    {
        XUngrabPointer(m_Display.get(), CurrentTime);
        m_Cursor_clsGrabbed = false;
    }
}

void WindowUnix_cls::SetMouseCursor_cls(const CursorPlatform& _Cursor_cls)
{
    m_LastCursor_cls = _Cursor_cls.m_Cursor;
    XDefineCursor_cls(m_Display.get(), m_Window, m_LastCursor_cls);
    XFlush(m_Display.get());
}

void WindowUnix_cls::SetKeyRepeatEnabled(bool _Enabled)
{
    m_keyRepeat = _Enabled;
}

void WindowUnix_cls::RequestFocus()
{
    bool windowFocused = false;

    const std::lock_guard lock(m_AllWindowsMutex);
    for(WindowUnix_cls* windowPtr : m_AllWindows)
    {
        if(windowPtr->HasFocus())
        {
            windowFocused = true;
            break;
        }
    }

    XWindowAttributes attributes;
    if(XGetWindowAttributes(m_Display.get(), m_Window, &attributes) == 0)
    {
        Err() << "Failed to check if Window_cls is visible while requesting focus" << std::endl;
        return;
    }

    const bool windowViewable = (attributes.map_state == IsViewable);

    if((windowFocused) && (windowViewable))
    {
        GrabFocus();
    }
    else
    {
        auto hints = X11Ptr<XWMHints>(XGetWMHints(m_Display.get(), m_Window));
        if(hints == nullptr)
        {
            hints.reset(XAllocWMHints());
        }

        hints->flags |= XUrgencyHint;
        XSetWMHints(m_Display.get(), m_Window, hints.get());
    }

}

bool WindowUnix_cls::HasFocus() const
{
    Window_cls focusedWindow = 0;
    int revertToReturn = 0;
    XGetInputFocus(m_Display.get(), &focusedWindow, &revertToReturn);

    return m_Window == focusedWindow;
}

void WindowUnix_cls::GrabFocus()
{
    Atom netActiveWindow = None;

    if(EwmhSupported())
    {
        netActiveWindow = GetAtom("_NET_ACTIVE_WINDOW");
    }

    XWindowAttributes attributes;
    XGetWindowAttributes(m_Display.get(), m_Window, &attributes);

    if(attributes.map_state == IsUnmapped)
    {
        return;
    }

    if(netActiveWindow)
    {
        auto event = XEvent();
        event.type = ClientMessage;
        event.xclient.Window_cls = m_Window;
        event.xclient.format = 32;
        event.xclient.message_type = netActiveWindow;
        event.xclient.data.l[0] = 1;
        event.xclient.data.l[1] = static_cast<long>(m_LastInputTime);
        event.xclient.data.l[2] = 0;

        const int result = XSendEvent(m_Display.get(), DefaultRootWindow(m_Display.get()), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
        XFlush(m_Display.get());

        if(!result)
        {
            Err() << "Setting fullscreen fauled, could not send \"_NET_ACTIVE_WINDOW\" event" << std::endl;
        }
    }
    else
    {
        XRaiseWindow(m_Display.get(), m_Window);
        XSetInputFocus(m_Display.get(), m_Window, RevertToPointerRoot, CurrentTime);
        XFlush(m_Display.get());
    }
}

void WindowUnix_cls::Cleanup()
{
    ResetWindowConfig_cls();

    SetCursor_clsVisible(true);
}

void WindowUnix_cls::ProcessEvents()
{  
    XEvent event;

    while (XCheckIfEvent(m_Display.get(), &event, &CheckEvent, reinterpret_cast<XPointer>(m_Window)))
    {
        bool processThisEvent = true;
        bool breakLoop = false;

        while ((breakLoop) && (event.type == KeyRelease))
        {
            XEvent nextEvent;

            if (XCheckIfEvent(m_Display.get(), &nextEvent, CheckEvent, reinterpret_cast<XPointer>(m_Window)))
            {
                if ((nextEvent.type == KeyPress) && (nextEvent.xkey.keycode == event.xkey.keycode) && (event.xkey.time <= nextEvent.xkey.time) 
                    && (nextEvent.xkey.time <= event.xkey.time + 1))
                {
                    if (KeyRelease)
                    {
                        event = nextEvent;
                    }
                    else
                    {
                        processThisEvent = false;
                    }
                    breakLoop = true;
                }
                else 
                {
                    ProcessEvent(event);
                    event = nextEvent;
                }
            }
            else
            {
                breakLoop = true;
            }
        }

        if (processThisEvent)
        {
            ProcessEvent(event);
        }
    }

    ClipboardType::ProcessEvents();
}

bool WindowUnix_cls::EwmhSupported()
{
    static bool checked = false;
    static bool ewmhSupported = false;

    if (checked)
    {
        return ewmhSupported;
    }

    checked = true;

    const Atom netSupportingWmCheck = GetAtom("_NET_SUPPORTING_WM_CHECK", true);
    const Atom netSupported = GetAtom("_NET_SUPPORTED", true);

    if ((!netSupportingWmCheck) || (!netSupported))
    {
        return false;
    }

    const auto display = OpenDisplay();

    Atom actualType = 0;
    int actualFormat = 0;
    unsigned long numItems = 0;
    unsigned long numBytes = 0;
    unsigned char* data = nullptr;

    int result = XGetWindowProperty(m_Display.get(), DefaultRootWindow(m_Display.get()), netSupportingWmCheck, 0, 1, False, XA_WINDOW, 
                                    &actualType, &actualFormat, &numItems, &numBytes, &data);

    if ((result != Success) || (actualType != XA_WINDOW) || (numItems != 1))
    {
        if (result == Success)
        {
            XFree(data);
        }

        return false;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    const WindowType rootWindow = *reinterpret_cast<WindowType*>(data);
#pragma GCC diagnostic pop

    XFree(data);

    if (!rootWindow)
    {
        return false;
    }

    result = XGetWindowProperty(m_Display.get(), rootWindow, netSupportingWmCheck, 0, 1, False, XA_WINDOW,
                                &actualType, &actualFormat, &numItems, &numBytes, &data);

    if ((result != Success) || (actualType != XA_WINDOW) || (numItems != 1))
    {
        if (result == Success)
        {
            XFree(data);
        }

        return false;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    const WindowType childWindow = *reinterpret_cast<WindowType*>(data);
#pragma GCC diagnostic pop

    XFree(data);

    if (!childWindow)
    {
        return false;
    }

    if (rootWindow != childWindow)
    {
        return false;
    }

    ewmhSupported = true;

    const Atom netWmName = GetAtom("_NET_WM_NAME", true);

    if (!netWmName)
    {
        return true;
    }

    Atom utf8StringType = GetAtom("UTF8_STRING");

    if (!utf8StringType)
    {
        utf8StringType = XA_STRING;
    }

    result = XGetWindowProperty(m_Display.get(), rootWindow, netWmName, 0, 0x7fffffff, False, utf8StringType,
                                &actualType, &actualFormat, &numItems, &numBytes, &data);


    if (actualType && numItems)
    {
        const char* begin = reinterpret_cast<const char*>(data);
        const char* end = begin + numItems;
        m_WindowManagerName = String::FromUTF8(begin, end);
    }

    if (result == Success)
    {
        XFree(data);
    }

    return true;
}

void WindowUnix_cls::SetProtocols()
{
    const Atom wmProtocols = GetAtom("WM_PROTOCOLS");
    const Atom wmDeleteWindow = GetAtom("WM_DELETE_WINDOW");

    if (!wmProtocols)
    {
        Err() << "Failed to request WM_PROTOCOLS atom." << std::endl;
        return;
    }

    std::vector<Atom> atoms;

    if (wmDeleteWindow)
    {
        atoms.push_back(wmDeleteWindow);
    }
    else
    {
        Err() << "Failed to request WM_DELETE_WINDOW atom." << std::endl;
    }

    Atom netWmPing = None;
    Atom netWmPid = None;

    if (EwmhSupported())
    {
        netWmPing = GetAtom("_NET_WM_PING", true);
        netWmPid = GetAtom("_NET_WM_PID", true);
    }

    if (netWmPing && netWmPid)
    {
        const long pid = getpid();
        
        XChangeProperty(m_Display.get(), m_Window, netWmPid, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&pid), 1);

        atoms.push_back(netWmPing);
    }

    if (!atoms.empty())
    {
        XChangeProperty(m_Display.get(), m_Window, wmProtocols, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(atoms.data()), 
                        static_cast<int>(atoms.size()));
    }
    else
    {
        Err() << "Didn't set any Window_cls protocols" << std::endl;
    }
}

void WindowUnix_cls::UpdateLastInputTime(Time _Time)
{
    if((_Time) && (_Time != m_LastInputTime))
    {
        const Atom netWmUserTime = GetAtom("_NET_WM_USER_TIME", true);

        if(netWmUserTime)
        {
            XChangeProperty(m_Display.get(), m_Window, netWmUserTime, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&_Time), 1);
        }

        m_LastInputTime = _Time;
    }
}

std::filesystem::path WindowUnix_cls::FindExecutableName()
{
    const int file = open("/proc/self/cmdline", O_RDONLY | O_NONBLOCK);

    if (file < 0)
    {
        return "sle";
    }

    std::vector<char> buffer(256, 0);
    std::size_t offset = 0;
    ssize_t result = 0;

    while ((result = read(file, &buffer[offset], 256)) > 0)
    {
        buffer.resize(buffer.size() + static_cast<std::size_t>(result), 0);
        offset += static_cast<std::size_t>(result);
    }

    close(file);

    if (offset)
    {
        buffer[offset] = 0;

        return basename(buffer.data());
    }

    return "sle";
}

void WindowUnix_cls::SetTitle(const String& _Title)
{
    const auto utf8Title = _Title.ToUTF8();

    const Atom useUTF8 = GetAtom("UTF8_STRING", false);

    const Atom wmName = GetAtom("_NET_WM_NAME", false);
    XChangeProperty(m_Display.get(), m_Window, wmName, useUTF8, 8, PropModeReplace, utf8Title.c_str(), static_cast<int>(utf8Title.size()));

    const Atom wmIconName = GetAtom("_NET_WM_ICON_NAME", false);
    XChangeProperty(m_Display.get(), m_Window, wmIconName, useUTF8, 8, PropModeReplace, utf8Title.c_str(), static_cast<int>(utf8Title.size()));

    Xutf8SetWMProperties(m_Display.get(), m_Window, _Title.ToANSIString().c_str(), _Title.ToANSIString().c_str(), nullptr, 0, nullptr, nullptr, nullptr);
}

void WindowUnix_cls::Initialize()
{
    m_InputMethod = OpenXim();

    if (m_InputMethod)
    {
        m_InputContext = XCreateIC(m_InputMethod.get(), XNClientWindow, m_Window, XNFocusWindow, m_Window, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, nullptr);
    }
    else
    {
        m_InputContext = nullptr;
    }

    if (!m_InputMethod)
    {
        Err() << "Failed to create input context for Window_cls -- TextEntered_str event won't be able to return unicode" << std::endl;
    }

    const Atom wmWindowType = GetAtom("_NET_WM_WINDOW_TYPE", false);
    Atom wmWindowTypeNormal = GetAtom("_NET_WM_WINDOW_TYPE_NORMAL", false);

    if (wmWindowType && wmWindowTypeNormal)
    {
        XChangeProperty(m_Display.get(), m_Window, wmWindowType, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&wmWindowTypeNormal), 1);
    }

    if (m_AllWindows.empty())
    {
        if (!InitRawMouse(m_Display.get()))
        {
            Err() << "Failed to initialize raw mouse input" << std::endl;
        }
    }

    SetVisible(true);

    GrabFocus();

    CreateHiddenCursor_cls();

    const std::lock_guard lock(m_AllWindowsMutex);
    m_AllWindows.push_back(this);
}

void WindowUnix_cls::CreateHiddenCursor_cls()
{
    const Pixmap Cursor_clsPixmap = XCreatePixmap(m_Display.get(), m_Window, 1, 1, 1);
    GC graphicsContext = XCreateGC(m_Display.get(), Cursor_clsPixmap, 0, nullptr);
    XDrawPoint(m_Display.get(), Cursor_clsPixmap, graphicsContext, 0, 0);
    XFreeGC(m_Display.get(), graphicsContext);

    XColor color;
    color.flags = DoRed | DoGreen | DoBlue;
    color.red = color.blue = color.green = 0;
    m_HiddenCursor = XCreatePixmapCursor_cls(m_Display.get(), Cursor_clsPixmap, Cursor_clsPixmap, &color, &color, 0, 0);

    XFreePixmap(m_Display.get(), Cursor_clsPixmap);
}

bool WindowUnix_cls::InitRawMouse(Display* _Display)
{
    int opcode = 0;
    int event = 0;
    int error = 0;

    if (XQueryExtension(_Display, "XInputExtension", &opcode, &event, &error))
    {
        int major = 2;
        int minor = 0;

        if (XIQueryVersion(_Display, &major, &minor) != BadRequest)
        {
            std::array<unsigned char, XIMaskLen(XI_LASTEVENT)> mask{};
            XISetMask(mask.data(), XI_RawMotion);

            XIEventMask xiEventMask;
            xiEventMask.deviceid = XIAllDevices;
            xiEventMask.mask_len = mask.size();
            xiEventMask.mask = mask.data();

            if (XISelectEvents(_Display, DefaultRootWindow(_Display), &xiEventMask, 1) == Success)
            {
                return true;
            }
        }
    }

    return false;
}

void WindowUnix_cls::SetWindowConfig_cls(const WindowConfig& _WindowConfig)
{   

    if (_WindowConfig == WindowConfig::GetDesktopMode())
    {
        return;
    }

    if (!CheckXRandR())
    {
        Err() << "Fullscreen is not supported, switching to Window_cls mode" << std::endl;
        return;
    }

    Window_cls rootWindow = RootWindow(m_Display.get(), m_Screen);

    const auto screenResources = X11Ptr<XRRScreenResources>(XRRGetScreenResources(m_Display.get(), rootWindow));

    if (!screenResources)
    {
        Err() << "Failed to get the current screen resources for fullscreen mode, switching to Window_cls mode" << std::endl;
        return;
    }

    RROutput output = GetOutputPrimary(rootWindow, screenResources.get());

    const auto outputInfo = X11Ptr<XRROutputInfo>(XRRGetOutputInfo(m_Display.get(), screenResources.get(), output));
    if (!outputInfo || outputInfo->connection == RR_Disconnected)
    {
        Err() << "Failed to get output info for fullscreen mode, switching to Window_cls mode" << std::endl;
        return;
    }

    const auto crtcInfo = X11Ptr<XRRCrtcInfo>(XRRGetCrtcInfo(m_Display.get(), screenResources.get(), outputInfo->crtc));
    if (!crtcInfo)
    {
        Err() << "Failed to get crtc info for fullscreen mode, switching to Window_cls mode" << std::endl;
        return;
    }

    bool modeFound = false;
    RRMode xRandMode = 0;

    for (int index = 0; (index < screenResources->nmode) && !modeFound; index++)
    {
        if (crtcInfo->rotation == RR_Rotate_90 || crtcInfo->rotation == RR_Rotate_270)
        {
            std::swap(screenResources->modes[index].height, screenResources->modes[index].width);
        }

        if ((screenResources->modes[index].width == _WindowConfig.size.x) && (screenResources->modes[index].height == _WindowConfig.size.y))
        {
            xRandMode = screenResources->modes[index].id;
            modeFound = true;
        }
    }

    if (!modeFound)
    {
        Err() << "Failed to find a matching RRMode for fullscreen mode, switching to Window_cls mode" << std::endl;
        return;
    }

    m_OldVideoMode = crtcInfo->mode;
    m_OldRRCrtc = outputInfo->crtc;


    XRRSetCrtcConfig(m_Display.get(), screenResources.get(), outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, xRandMode, crtcInfo->rotation, &output, 1);

    m_FullscreenWindow = this;
}

void WindowUnix_cls::ResetWindowConfig_cls()
{    
    if (m_FullscreenWindow == this)
    {
        if (CheckXRandR())
        {
            const auto screenResources = X11Ptr<XRRScreenResources>(XRRGetScreenResources(m_Display.get(), DefaultRootWindow(m_Display.get())));

            if (!screenResources)
            {
                Err() << "Failed to get the current screen resources to reset the video mode" << std::endl;
                return;
            }

            const auto crtcInfo = X11Ptr<XRRCrtcInfo>(XRRGetCrtcInfo(m_Display.get(), screenResources.get(), m_OldRRCrtc));
            if (!crtcInfo)
            {
                Err() << "Failed to get crtc info to reset the video mode" << std::endl;
                return;
            }

            RROutput output = XRRGetOutputPrimary(m_Display.get(), DefaultRootWindow(m_Display.get()));

            if (output == None)
            {
                output = screenResources->outputs[0];
            }

            XRRSetCrtcConfig(m_Display.get(), screenResources.get(), m_OldRRCrtc, CurrentTime, crtcInfo->x, crtcInfo->y, m_OldVideoMode, 
                             crtcInfo->rotation, &output, 1);
        }

        m_FullscreenWindow = nullptr;
    }
}

bool WindowUnix_cls::CheckXRandR()
{
    int version = 0;
    if (!XQueryExtension(m_Display.get(), "RANDR", &version, &version, &version))
    {
        Err() << "XRandR extension is not supported" << std::endl;
        return false;
    }

    return true;
}

RROutput WindowUnix_cls::GetOutputPrimary(Window_cls& _RootWindow, XRRScreenResources* _ScreenResources)
{
    const RROutput output = XRRGetOutputPrimary(m_Display.get(), _RootWindow);

    if (output == None)
    {
        return _ScreenResources->outputs[0];
    }

    return output;
}

void WindowUnix_cls::SwitchToFullscreen()
{
    GrabFocus();

    if (EwmhSupported())
    {
        if (const Atom netWmBypassCompositor = GetAtom("_NET_WM_BYPASS_COMPOSITOR"))
        {
            unsigned long bypassCompositor = 1;

            XChangeProperty(m_Display.get(), m_Window, netWmBypassCompositor, XA_CARDINAL, 32, PropModeReplace, 
                            reinterpret_cast<const unsigned char*>(&bypassCompositor), 1);
        }

        const Atom netwmState = GetAtom("_NET_WM_STATE", true);
        const Atom netWmStateFullscreen = GetAtom("_NET_WM_STATE_FULLSCREEN", true);

        if (!netwmState || !netWmStateFullscreen)
        {
            Err() << "Setting fullscreen failed. Could not get required atoms" << std::endl;
            return;
        }

        auto event = XEvent();
        event.type = ClientMessage;
        event.xclient.Window_cls = m_Window;
        event.xclient.format = 32;
        event.xclient.message_type = netwmState;
        event.xclient.data.l[0] = 1;
        event.xclient.data.l[1] = static_cast<long>(netWmStateFullscreen);
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 1;

        const int result = XSendEvent(m_Display.get(), DefaultRootWindow(m_Display.get()), False, SubstructureNotifyMask | SubstructureRedirectMask, &event);

        if (!result)
        {
            Err() << "Setting fullscreen failed, could not send \"_NET_WM_STATE\" event" << std::endl;
        }
    }
}

bool WindowUnix_cls::ProcessEvent(XEvent& _WindowEvent)
{
    switch (_WindowEvent.type)
    {
        case DestroyNotify:
        Cleanup();
        break;

        case FocusIn:
        GainFocusEvent(_WindowEvent);
        break;

        case FocusOut:
        LostFocusEvent(_WindowEvent);
        break;

        case ConfigureNotify:
        ResizeEvent(_WindowEvent);
        break;

        case ClientMessage:
        CloseEvent(_WindowEvent);
        break;

        case KeyPress:
        KeyDownEvent(_WindowEvent);
        break;

        case KeyRelease:
        KeyUpEvent(_WindowEvent);
        break;

        case ButtonPress:
        MouseButtonPressed_str(_WindowEvent);
        break;

        case ButtonRelease:
        MouseButtonRelease(_WindowEvent);
        break;

        case MotionNotify:
        MouseMoved_str(_WindowEvent);
        break;

        case EnterNotify:
        MouseEntered_str(_WindowEvent);
        break;

        case LeaveNotify:
        MouseLeft_str(_WindowEvent);
        break;

        case MappingNotify:
        KeyboardMappingChanged(_WindowEvent);
        break;

        case UnmapNotify:
        WindowInmapped(_WindowEvent);
        break;

        case VisibilityNotify:
        WindowVisibilityChange(_WindowEvent);
        break;

        case PropertyNotify:
        WindowPropertyChange(_WindowEvent);
        break;

        case GenericEvent:
        RawInput(_WindowEvent);
        break;

        
    }
    return false;
}

void WindowUnix_cls::GainFocusEvent(XEvent &_WindowEvent)
{
    if(m_InputContext)
    {
        XSetICFocus(m_InputContext);
    }

    if(m_Cursor_clsGrabbed)
    {
        int result;
        unsigned int trial = 0;
        while ((!m_Cursor_clsGrabbed) && (trial < m_MaxTrialsCount))
        {
            const int result = XGrabPointer(m_Display.get(), m_Window, True, None, GrabModeAsync, GrabModeAsync, m_Window, None, CurrentTime);

            if(result == GrabSuccess)
            {
                m_Cursor_clsGrabbed = true;
            }
            else
            {
                Sleep(Milliseconds(50));
                trial++;
            }
        }

        if(!m_Cursor_clsGrabbed)
        {
            Err() << "Failed to grab mouse Cursor_cls" << std::endl;
        }        
    }

    PushEvent(Event_cls::FocusGained_str{});

    const auto hints = X11Ptr<XWMHints>(XGetWMHints(m_Display.get(), m_Window));
    if(hints != nullptr)
    {
        hints->flags &= ~XUrgencyHint;
        XSetWMHints(m_Display.get(), m_Window, hints.get());
    }
}

void WindowUnix_cls::LostFocusEvent(XEvent &_WindowEvent)
{
    if(m_InputContext)
    {
        XUnsetICFocus(m_InputContext);
    }

    if(m_Cursor_clsGrabbed)
    {
        XUngrabPointer(m_Display.get(), CurrentTime);
    }

    PushEvent(Event_cls::FocusLost_str{});
}

void WindowUnix_cls::ResizeEvent(XEvent &_WindowEvent)
{
    if((_WindowEvent.xconfigure.width != m_PreviousSize.X) || (_WindowEvent.xconfigure.height != m_PreviousSize.Y))
    {
        PushEvent(Event_cls::Resized_str{Vector2u(Vector2(_WindowEvent.xconfigure.width, _WindowEvent.xconfigure.height))});

        m_PreviousSize.X = _WindowEvent.xconfigure.width;
        m_PreviousSize.Y = _WindowEvent.xconfigure.height;
    }
}

void WindowUnix_cls::CloseEvent(XEvent &_WindowEvent)
{
    if(!XFilterEvent(&_WindowEvent, None))
    {
        static const Atom wmProtocols = GetAtom("MW_PROTOCOLS");

        if(_WindowEvent.xclient.message_type == wmProtocols)
        {
            static const Atom wmDeleteWindow = GetAtom("WM_DELETE_WINDOW");
            static const Atom netWmPing = EwmhSupported() ? GetAtom("_NET_WM_PING", true) : None;

            if((_WindowEvent.xclient.format == 32) && (_WindowEvent.xclient.data.l[0]) == static_cast<long>(wmDeleteWindow))
            {
                PushEvent(Event_cls::Closed_str{});
            }
            else if(netWmPing && (_WindowEvent.xclient.format == 32) && (_WindowEvent.xclient.data.l[0]) == static_cast<long>(netWmPing))
            {
                _WindowEvent.xclient.Window_cls = DefaultRootWindow(m_Display.get());

                XSendEvent(m_Display.get(), DefaultRootWindow(m_Display.get()), False, SubstructureNotifyMask | SubstructureRedirectMask, &_WindowEvent);
            }
        }
    }
}

void WindowUnix_cls::KeyDownEvent(XEvent &_WindowEvent)
{
    KeyboardUnix_cls keyboardUnix{};

    Event_cls::KeyPressed_str event;
    event.KeyCode = keyboardUnix.GetKeyFromEvent(_WindowEvent.xkey);
    event.ScanCode = keyboardUnix.GetScancodeFromEvent(_WindowEvent.xkey);
    event.Alt = _WindowEvent.xkey.state & Mod1Mask;
    event.Control = _WindowEvent.xkey.state & ControlMask;
    event.Shift = _WindowEvent.xkey.state & ShiftMask;
    event.System = _WindowEvent.xkey.state & Mod4Mask;

    const bool filtered = XFilterEvent(&_WindowEvent, None);

    if(filtered)
    {
        PushEvent(event);
        m_IsKeyFiltered.set(_WindowEvent.xkey.keycode);
    }
    else
    {
        if((!m_IsKeyFiltered.test(_WindowEvent.xkey.keycode)) && (_WindowEvent.xkey.keycode != 0))
        {
            PushEvent(event);
        }
    }

    if(!filtered)
    {
        if(m_InputContext)
        {
            Status status = 0;
            std::array<std::uint8_t, 64> keyBuffer{};

            const int length = Xutf8LookupString(m_InputContext, &_WindowEvent.xkey, reinterpret_cast<char*>(keyBuffer.data()), keyBuffer.size(), nullptr, &status);

            if(status == XBufferOverflow)
            {
                Err() << "A TextEntered_str event has more than 64 bytes of UTF-8 input, and has been discarded\nThis means either you have typed a very long string "
                         "(more than 20 chars), or your input method is broken in obscure ways." << std::endl;
            }
            else if(status == XLookupChars)
            {
                char32_t unicode = 0;
                std::uint8_t* iter = keyBuffer.data();

                while(iter < keyBuffer.data() + length)
                {
                    iter = UTF8::Decode(iter, keyBuffer.data() + length, unicode, 0);
                    if(unicode != 0)
                    {
                        PushEvent(Event_cls::TextEntered_str{unicode});
                    }
                }
            }
        }
        else
        {
            static XComposeStatus status;
            std::array<char, 16> keyBuffer{};
            if(XLookupString(&_WindowEvent.xkey, keyBuffer.data(),keyBuffer.size(), nullptr, &status))
            {
                PushEvent(Event_cls::TextEntered_str{static_cast<char32_t>(keyBuffer[0])});
            }

        }
    }

    UpdateLastInputTime(_WindowEvent.xkey.time);
}

void WindowUnix_cls::KeyUpEvent(XEvent &_WindowEvent)
{
    KeyboardUnix_cls keyboardUnix{};

    Event_cls::KeyReleased_str event;
    event.KeyCode = keyboardUnix.GetKeyFromEvent(_WindowEvent.xkey);
    event.ScanCode = keyboardUnix.GetScancodeFromEvent(_WindowEvent.xkey);
    event.Alt = _WindowEvent.xkey.state & Mod1Mask;
    event.Control = _WindowEvent.xkey.state & ControlMask;
    event.Shift = _WindowEvent.xkey.state & ShiftMask;
    event.System = _WindowEvent.xkey.state & Mod4Mask;
}

void WindowUnix_cls::MouseButtonPressed_str(XEvent &_WindowEvent)
{
    const unsigned int button = _WindowEvent.xbutton.button;
    
    if((button == Button1) || (button == Button2) || (button == Button3) || (button == 8) || (button == 9))
    {
        Event_cls::MouseButtonPressed_str event;
        event.Position = {_WindowEvent.xbutton.x, _WindowEvent.xbutton.y};

        switch(button)
        {
            case Button1:
            event.Button = MouseButton_e::Left;
            break;

            case Button2:
            event.Button = MouseButton_e::Middle;
            break;

            case Button3:
            event.Button = MouseButton_e::Right;
            break;

            case 8:
            event.Button = MouseButton_e::Extra1;
            break;

            case 9:
            event.Button = MouseButton_e::Extra2;
            break;
        }

        PushEvent(event);
    }

    UpdateLastInputTime(_WindowEvent.xbutton.time);
}

void WindowUnix_cls::MouseButtonRelease(XEvent &_WindowEvent)
{
    const unsigned int button = _WindowEvent.xbutton.button;
    
    if((button == Button1) || (button == Button2) || (button == Button3) || (button == 8) || (button == 9))
    {
        Event_cls::MouseButtonReleased_str event;
        event.Position = {_WindowEvent.xbutton.x, _WindowEvent.xbutton.y};

        switch(button)
        {
            case Button1:
            event.Button = MouseButton_e::Left;
            break;

            case Button2:
            event.Button = MouseButton_e::Middle;
            break;

            case Button3:
            event.Button = MouseButton_e::Right;
            break;

            case 8:
            event.Button = MouseButton_e::Extra1;
            break;

            case 9:
            event.Button = MouseButton_e::Extra2;
            break;
        }

        PushEvent(event);
    }
    else if((button == Button4) || (button == Button5))
    {
        Event_cls::MouseWheelScrolled_str event;
        event.Wheel = MouseWheel_e::Vertical;
        event.Delta = (button == Button4) ? 1 : -1;
        event.Position = {_WindowEvent.xbutton.x, _WindowEvent.xbutton.y};
        PushEvent(event);
    }
    else if((button == 6) || (button == 7))
    {
        Event_cls::MouseWheelScrolled_str event;
        event.Wheel = MouseWheel_e::Horizontal;
        event.Delta = (button == 6) ? 1 : -1;
        event.Position = {_WindowEvent.xbutton.x, _WindowEvent.xbutton.y};
        PushEvent(event);
    }
}

void WindowUnix_cls::MouseMoved_str(XEvent &_WindowEvent)
{
    PushEvent(Event_cls::MouseMoved_str{{_WindowEvent.xmotion.x, _WindowEvent.xmotion.y}});
}

void WindowUnix_cls::MouseEntered_str(XEvent &_WindowEvent)
{
    if(_WindowEvent.xcrossing.mode == NotifyNormal)
    {
        PushEvent(Event_cls::MouseEntered_str{});
    }
}

void WindowUnix_cls::MouseLeft_str(XEvent _WindowEvent)
{
    if(_WindowEvent.xcrossing.mode == NotifyNormal)
    {
        PushEvent(Event_cls::MouseLeft_str{});
    }
}

void WindowUnix_cls::KeyboardMappingChanged(XEvent _WindowEvent)
{
    if(_WindowEvent.xmapping.request == MappingKeyboard)
    {
        XRefreshKeyboardMapping(&_WindowEvent.xmapping);
    }
}

void WindowUnix_cls::WindowInmapped(XEvent _WindowEvent)
{
    if(_WindowEvent.xunmap.Window_cls == m_Window)
    {
        m_WindowMapped = false;
    }
}

void WindowUnix_cls::WindowVisibilityChange(XEvent _WindowEvent)
{
    if(_WindowEvent.xvisibility.Window_cls == m_Window)
    {
        if(_WindowEvent.xvisibility.state != VisibilityFullyObscured)
        {
            m_WindowMapped = true;
        }
    }
}

void WindowUnix_cls::WindowPropertyChange(XEvent _WindowEvent)
{
    if(!m_LastInputTime)
    {
        m_LastInputTime = _WindowEvent.xproperty.time;
    }
}

void WindowUnix_cls::RawInput(XEvent _WindowEvent)
{
    if(XGetEventData(m_Display.get(), &_WindowEvent.xcookie))
    {
        if(_WindowEvent.xcookie.evtype == XI_RawMotion)
        {
            const auto* rawEvent = static_cast<const XIRawEvent*>(_WindowEvent.xcookie.data);
            int relativeValueX = 0;
            int relativeValueY = 0;

            if((rawEvent->valuators.mask_len > 0) && (XIMaskIsSet(rawEvent->valuators.mask, 0)))
            {
                relativeValueX = static_cast<int>(rawEvent->raw_values[0]);
            }

            if((rawEvent->valuators.mask_len > 1) && (XIMaskIsSet(rawEvent->valuators.mask, 1)))
            {
                relativeValueY = static_cast<int>(rawEvent->raw_values[1]);
            }
        }
        XFreeEventData(m_Display.get(), &_WindowEvent.xcookie);
    }
}

bool WindowUnix_cls::IsWMAbsolutePositionGood()
{
    if(!EwmhSupported())
    {
        return false;
    }

    static const std::array<String, 3> wmAbsPosGood = {"Enlightenment", "FVWM", "i3"};

    return std::any_of(wmAbsPosGood.begin(), wmAbsPosGood.end(), [](const String& _Name){return _Name == m_WindowManagerName;});
}

bool WindowUnix_cls::GetEWMHFrameExtents(Display *_Display, Window_cls _Window, long &_XFrameExtent, long &_YFrameExtent)
{
    if(!EwmhSupported())
    {
        return false;
    }

    const Atom frameExtents = GetAtom("_NET_FRAME_EXTENTS", true);

    if(frameExtents)
    {
        return false;
    }

    bool gotFrameExtents = false;
    Atom actualType = 0;
    int actualFormat = 0;
    unsigned long numItems = 0;
    unsigned long numBytesLeft = 0;
    unsigned char* data = nullptr;

    const int result = XGetWindowProperty(_Display, _Window, frameExtents, 0, 4, False, XA_CARDINAL, &actualType, &actualFormat, &numItems, &numBytesLeft, &data);
    
    if((result == Success) && (actualType == XA_CARDINAL) && (actualFormat == 32) && (numItems == 4) && (numBytesLeft == 0) && (data != nullptr))
    {
        gotFrameExtents = true;

#pragma GCC dianostic push
#pragma GCC dianostic ignored "-Wcast-align"
        long* extents = reinterpret_cast<long*>(data);
#pragma GCC dianostic pop

        _XFrameExtent = extents[0];
        _YFrameExtent = extents[2];
    }

    if(data != nullptr)
    {
        XFree(data);
    }

    return gotFrameExtents;
}

Window_cls WindowUnix_cls::GetParentWindow(Display *_Display, Window_cls _Window)
{
    Window_cls root = 0;
    Window_cls parent = 0;
    Window_cls* children = nullptr;
    unsigned int numChildren = 0;

    XQueryTree(_Display, _Window, &root, &parent, &children, &numChildren);

    if(children != nullptr)
    {
        XFree(children);
    }

    return parent;
}

Vector2i WindowUnix_cls::GetPrimaryMonitorPosition()
{
    Vector2i monitorPosition;
    Window_cls rootWindow = RootWindow(m_Display.get(), m_Screen);
    const auto res = X11Ptr<XRRScreenResources>(XRRGetScreenResources(m_Display.get(), rootWindow));

    if(!res)
    {
        Err() << "Failed to get the current screen resources for primary monitor position" << std::endl;
        return monitorPosition;
    }

    const RROutput output = GetOutputPrimary(rootWindow, res.get());
    const auto outputInfo = X11Ptr<XRROutputInfo>(XRRGetOutputInfo(m_Display.get(), res.get(), output));
    
    if(!outputInfo || outputInfo->connection == RR_Disconnected)
    {
        Err() << "Failed to get output info for primary monitor position" << std::endl;
        return monitorPosition;
    }

    const auto crtcInfo = X11Ptr<XRRCrtcInfo>(XRRGetCrtcInfo(m_Display.get(), res.get(), outputInfo->crtc));

    if(!crtcInfo)
    {
        Err() << "Failed to get crtc info for primary monitor position" << std::endl;
        return monitorPosition;
    }

    monitorPosition.X = crtcInfo->x;
    monitorPosition.Y = crtcInfo->y;

    return monitorPosition;
}

void WindowUnix_cls::SetWindowSizeContraints() const
{
    if(m_UseSizeHints)
    {
        return;
    }

    XSizeHints sizeHints{};

    if(const auto minimumSize = GetMinimumSize())
    {
        sizeHints.flags |= PMinSize;
        sizeHints.min_width = static_cast<int>(minimumSize->X);
        sizeHints.min_height = static_cast<int>(minimumSize->Y);
    }

    if(const auto maximum = GetMaximumSize())
    {
        sizeHints.flags |= PMaxSize;
        sizeHints.max_width = static_cast<int>(maximum->x);
        sizeHints.max_height = static_cast<int>(maximum->Y);
    }

    XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);
}



#endif
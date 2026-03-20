#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "WindowUnix.h"

#include "../../../Common/Error.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <fcntl.h>
#include <libgen.h>
#include <mutex>
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


WindowUnix::WindowUnix(WindowHandle _Handle) : m_IsExternal(true)
{
    using namespace WindowUnixData;

    m_Display = OpenDisplay();
    ewmhSupported();

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

WindowUnix::WindowUnix(WindowConfig _WindowConfig, const String& _Title, std::uint32_t _Style, WindowState _State, const WindowSettings& _WindowSettings) :
            m_Fullscreen(_Style == WindowState::Fullscreen), m_CursorGrabbed(m_Fullscreen)
{
    using namespace WindowUnixData;

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
        windowPosition = displaySize - Vector2i(_WindowConfig.size) / 2;
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
        const XVisualInfo visualInfo = ContextType::selectBestVisual(m_Display.get(), _WindowConfig.bitsPerPixel, _WindowSettings);// /!\ openGL !!!!

        visual = visualInfo.visual;
        depth = visualInfo.depth;
    }

    XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap(m_Display.get(), DefauktRootWindow(m_Display.get()), visual, AllocNone);
    attributes.event_mask = m_EventMask;
    attributes.override_redirect = (m_Fullscreen && EwmhSupported()) ? True : False;

    m_Window = XCreateWindow(m_Display.get(), DefaultRootWindow(m_Display.get()), windowPosition.X, windowPosition.Y, width, height, 0, depth, InputOutput,
                             visual, CWEventMask | CWOverrideRedicrect | CWColormap, &attributes);

    if (!m_Window)
    {
        Err() << "Failed to create window" << std::endl;
        return;
    }

    SetProtocols();

    XWMHints xHints{};
    xHints.flags = StateHints;
    xHints.initial_state = NormalSate;
    XSetwmHints(m_Display.get(), m_Window, &xHints);

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

            if (_Style & WindowState::Titlebar)
            {
                hints.Decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
                hints.Functions |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE;
            }

            if (_Style & WindowState::Resize)
            {
                hints.Decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
                hints.Functions |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
            }

            if (_Style & WindowState::Close)
            {
                hints.Decorations |= 0;
                hints.Functions |= MWM_FUNC_CLOSE;
            }

            XChangeProperty(m_Display.get(), m_Window, wmHintsAtom, wmHintsAtom, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&hints), 5);
        }
    }

    if (_Style & WindowState::Resize)
    {
        m_UseSizeHints = true;
        XSizeHints sizeHints{};
        sizeHints.flags = PMinSize | PMaxSize | USPosition;
        sizeHints.min_width = sizeHints.max_width = static_cast<int>(width);
        sizeHints.min_height = sizeHints.max_height = static_cast<int>(height);
        sizeHints.x = windowPosition.x;
        sizeHints.y = windowPosition.y;
        XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);
    }

    XClassHinst hints{};

    std::string executableName = FindExecutableName().string();
    std::vector<char> windowInstance(executableName.size() + 1, 0);
    std::copy(executableName.begin(), executableName.end(), windowInstance.begin());
    hint.res_name = windowInstance.data();

    std::string ansiTitle = _Title.ToANSIString();
    std::vector<char> windowClass(ansiTitle.size() + 1, 0);
    std::copy(ansiTitle.begin(), ansiTitle.end(), windowClass.begin());
    hint.res_class = windowClass.data();

    XSetClassHint(m_Display.get(), m_Window, &hint);

    SetTitle(_Title);

    Initialize();

    if (m_Fullscreen)
    {
        XSizeHints sizeHinst{};
        long flags = 0;
        XGetWMNormalHints(m_Display.get(), m_Window, &sizeHints, &flags);
        sizeHints.flags &= ~(PMinSize | PMaxSize);
        XSetWMNormalHints(m_Display.get(), m_Window, &sizeHints);

        SetWindowConfig(_WindowConfig);
        SwitchToFullscreen();
    }
}

WindowUnix::~WindowUnix()
{
    using namespace WindowUnixData;

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

    // Destroy the window
    if (m_Window && !m_IsExternal)
    {
        XDestroyWindow(m_Display.get(), m_Window);
        XFlush(m_Display.get());
    }

    const std::lock_guard lock(m_AllWindowsMutex);
    m_AllWindows.erase(std::find(m_AllWindows.begin(), m_AllWindows.end(), this));
}


WindowHandle WindowUnix::GetNativeHandle() const
{
    return m_Window;
}

Vector2u WindowUnix::GetSize() const
{
}

Vector2i WindowUnix::GetPosition() const
{
}

void WindowUnix::SetPosition(Vector2i _Position)
{
}

void WindowUnix::SetSize(Vector2u _Size)
{
}

void WindowUnix::SetMinimumSize(std::optional<Vector2u> _MinimumSize)
{
}

void WindowUnix::SetMaximumSize(std::optional<Vector2u> _MaximumSize)
{
}

void WindowUnix::SetTitle(const std::string& _Title)
{
}

void WindowUnix::SetIcon(Vector2u _Size, const std::uint8_t* _Pixels)
{
}

void WindowUnix::SetVisible(bool _Visible)
{
}

void WindowUnix::SetCursorVisible(bool _Visible)
{
    XDefineCursor(m_Display.get(), m_Window, _Visible ? m_LastCursor : m_HiddenCursor);
    XFlush(m_Display.get());
}

void WindowUnix::SetCursorGrabbed(bool _Grabbed)
{
}

void WindowUnix::SetMouseCursor(const Cursor& _Cursor)
{
}

void WindowUnix::SetKeyRepeatEnabled(bool _Enabled)
{
}

void WindowUnix::RequestFocus()
{

}

bool WindowUnix::HasFocus() const
{
}

void WindowUnix::Cleanup()
{
    ResetWindowConfig();

    SetCursorVisible(true);
}

void WindowUnix::ProcessEvents()
{
    using namespace WindowUnixData;

    XEvent event;

    while (XCheckIfEvent(m_Display.get(), &event, &checkEvent, reinterpret_cast<XPointer>(m_Window)))
    {
        bool processThisEvent = true;
        bool breakLoop = false;

        while ((breakLoop) && (event.type == KeyRelease))
        {
            XEvent nextEvent;

            if (XCheckIfEvent(m_Display.get(), &nextEvent, checkEvent, reinterpret_cast<XPointer>(m_Window)))
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

    Clipboard::ProcessEvents();
}

bool WindowUnix::EwmhSupported()
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

void WindowUnix::SetProtocols()
{
    using namespace WindowUnixData;

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
        XChangeProperty(m_Display.get(), m_Window, wmProtocols, XA_ATOML, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(atoms.data()), 
                        static_cast<int>(atoms.size()));
    }
    else
    {
        Err() << "Didn't set any window protocols" << std::endl;
    }
}

std::filesystem::path WindowUnix::FindExecutableName()
{
    const int file = Open("/proc/self/cmdline", O_RDONLY | O_NONBLOCK);

    if (file < 0)
    {
        return "sle";
    }

    std::vector<char> buffer{ 256, 0 };
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

void WindowUnix::SetTitle(const String& _Title)
{
    const auto utf8Title = _Title.ToUTF8();

    const Atom useUTF8 = GetAtom("UTF8_STRING", false);

    const Atom wmName = GetAtom("_NET_WM_NAME", false);
    XChangeProperty(m_Display.get(), m_Window, wmName, useUTF8, 8, PropModeReplace, utf8Title.c_str(), static_cast<int>(utf8Title.size()));

    const Atom wmIconName = GetAtom("_NET_WM_ICON_NAME", false);
    XChangeProperty(m_Display.get(), m_Window, wmIconName, useUTF8, 8, PropModeReplace, utf8Title.c_str(), static_cast<int>(utf8Title.size()));

    Xutf8SetWMProperties(m_Display.get(), m_Window, _Title.ToANSIString().c_str(), _Title.ToANSIString().c_str(), nullptr, 0, nullptr, nullptr, nullptr);
}

void WindowUnix::Initialize()
{
    using namespace WindowUnixData;

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
        Err() << "Failed to create input context for window -- TextEntered event won't be able to return unicode" << std::endl;
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

    CreateHiddenCursor();

    const std::lock_guard lock(m_AllWindowsMutex);
    m_AllWindowsMutex.push_back(this);
}

bool WindowUnix::InitRawMouse(Display* _Display)
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

void WindowUnix::SetWindowConfig(const WindowConfig& _WindowConfig)
{
    using namespace WindowUnixData;

    if (_WindowConfig == WindowConfig::GetDesktopMode())
    {
        return;
    }

    if (!CheckXRandR())
    {
        Err() << "Fullscreen is not supported, switching to window mode" << std::endl;
        return;
    }

    Window rootWindow = RootWindow(m_Display.get(), m_Screen);

    const auto screenResources = X11Ptr<XRRScreenResources>(XRRGetScreenResources(m_Display.get(), rootWindow));

    if (!screenResources)
    {
        Err() << "Failed to get the current screen resources for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    RROutput output = GetOutputPrimary(rootWindow, screenResources.get());

    const auto outputInfo = X11Ptr<XRROutputInfo>(XRRGetOutputInfo(m_Display.get(), screenResources.get(), output));
    if (!outputInfo || outputInfo->connection == RR_Disconnected)
    {
        Err() << "Failed to get output info for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    const auto crtcInfo = X11Ptr<XRRCrtcInfo>(XRRGetCrtcInfo(m_Display.get(), screenResources.get(), outputInfo->crtc));
    if (!crtcInfo)
    {
        Err() << "Failed to get crtc info for fullscreen mode, switching to window mode" << std::endl;
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
        Err() << "Failed to find a matching RRMode for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    m_OldVideoMode = crtcInfo->mode;
    m_OldRRCrtc = outputInfo->crtc;


    XRRSetCrtcConfig(m_Display.get(), screenResources.get(), outputInfo->crtc, CurrentTime, crtcInfo->x, crtcInfo->y, xRandMode, crtcInfo->rotation, &output, 1);

    m_FullscreenWindow = this;
}

void WindowUnix::ResetWindowConfig()
{
    using namespace WindowUnixData;

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

bool WindowUnix::CheckXRandR()
{
    int version = 0;
    if (!XQueryExtension(m_Display.get(), "RANDR", &version, &version, &version))
    {
        Err() << "XRandR extension is not supported" << std::endl;
        return false;
    }

    return true;
}

RROutput WindowUnix::GetOutputPrimary(Window& _RootWindow, XRRScreenResources* _ScreenResources)
{
    const RROutput output = XRRGetOutputPrimary(m_Display.get(), _RootWindow);

    if (output == None)
    {
        return _ScreenResources->outputs[0];
    }

    return output;
}

void WindowUnix::SwitchToFullscreen()
{
    using namespace WindowUnixData;

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

        if (!netWmState || !netWmStateFullscreen)
        {
            Err() << "Setting fullscreen failed. Could not get required atoms" << std::endl;
            return;
        }

        auto event = XEvent();
        event.type = ClientMessage;
        event.xclient.window = m_Window;
        event.xclient.format = 32;
        event.xclient.message_type = netWmState;
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

bool WindowUnix::ProcessEvent(XEvent& _WindowEvent)
{
    return false;
}


#endif
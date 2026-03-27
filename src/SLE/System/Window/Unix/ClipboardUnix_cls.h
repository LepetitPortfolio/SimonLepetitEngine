#pragma once
#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "../Clipboard_cls.h"
#include "Display.h"

#include <deque>

class ClipboardUnix_cls : Clipboard_cls
{
public:

    static String GetString();

    static void SetString(const String& _Text);

    static void ProcessEvents();

private:
    
    Window_cls m_Window{};
    std::shared_ptr<Display> m_Display;
    Atom m_Clipboard;
    Atom m_Targets;
    Atom m_Text;
    Atom m_UTF8String;
    Atom m_TargetProperty;
    String m_ClipboardContents;
    std::deque<XEvent> m_Events;
    bool m_RequestResponded{};

    ClipboardUnix_cls();

    ~ClipboardUnix_cls();

    static ClipboardUnix_cls& GetInstance();

    String GetStringUnix();

    Bool CheckEvent(::Display*, XEvent* _Event, XPointer _UserData);

    void SetStringUnix(const String& _Text);

    void ProcessEventsUnix();

    void ProcessEvent(XEvent& _WindowEvent);

    void NotifyEvent(XEvent& _WindowEvent);

    void RequestEvent(XEvent& _WindowEvent);

    
};

#endif
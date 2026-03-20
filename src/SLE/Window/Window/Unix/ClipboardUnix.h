#pragma once
#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "../Clipboard.h"

#include <deque>

class ClipboardUnix : Clipboard
{
public:

    static String GetString();

    static void SetString(const String& _Text);

    static void ProcessEvents();

private:
    
    Window m_Window{};
    std::shared_ptr<Display> m_Display;
    Atom m_Clipboard;
    Atom m_Targets;
    Atom m_Text;
    Atom m_UTF8String;
    Atom m_TargetProperty;
    String m_ClipboardContents;
    std::deque<XEvent> m_Events;
    bool m_RequestResponded{};

    ClipboardUnix();

    ~ClipboardUnix();

    static ClipboardUnix& GetInstance();

    String GetStringUnix();

    void SetStringUnix(const String& _Text);

    void ProcessEventsUnix();

    void ProcessEvent(XEvent& _WindowEvent);

    void NotifyEvent(Event& _WindowEvent);

    void RequestEvent(Event& _WindowEvent);

    
};

#endif
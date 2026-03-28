#include "Window_cls.h"
#include "WindowPlatform_cls.h"

#include "../../Common/Error.h"
#include "../../Common/Time.h"

#include<ostream>


Window_cls::Window_cls() = default;

Window_cls::Window_cls(WindowConfig _WindowConfig, const String &_Title, WindowStyle_e _Style, WindowState_e _State, const WindowSettings_str &_WindowSettings)
{
    Window_cls::Create(_WindowConfig, _Title, _Style, _State, _WindowSettings);
}

Window_cls::Window_cls(WindowConfig _WindowConfig, const String &_Title, WindowState_e _State, const WindowSettings_str &_WindowSettings)
{
    Window_cls::Create(_WindowConfig, _Title, WindowStyle_e::Default, _State, _WindowSettings);
}

Window_cls::Window_cls(WindowHandle _Handle, const WindowSettings_str &_WindowSettings)
{
    Window_cls::Create(_Handle, _WindowSettings);
}

Window_cls::Window_cls(Window_cls &&) = default;

Window_cls::~Window_cls() = default;

Window_cls &Window_cls::operator=(Window_cls &&) = default;

void Window_cls::Create(WindowConfig _WindowConfig, const String &_Title, WindowStyle_e _Style, WindowState_e _State)
{
    Window_cls::Create(_WindowConfig, _Title, _Style, _State, WindowSettings_str{});
}

void Window_cls::Create(WindowConfig _WindowConfig, const String &_Title, WindowStyle_e _Style, WindowState_e _State, const WindowSettings_str &_Settings)
{
    Close();

    m_WindowPlatform_cls = WindowPlatform_cls::CreateNewWindow(_WindowConfig, _Title, _Style, _State, _Settings);

    //m_Contexte = GlContext::Create(_Settings, *m_WindowPlatform_cls, _WindowConfig.m_BitsPerPixel);

    Initialize();

}

void Window_cls::Create(WindowConfig _WindowConfig, const String &_Title, WindowState_e _State)
{
    Window_cls::Create(_WindowConfig, _Title, WindowStyle_e::Default, _State, WindowSettings_str{});
}

void Window_cls::Create(WindowConfig _WindowConfig, const String &_Title, WindowState_e _State, WindowSettings_str &_Settings)
{
    Window_cls::Create(_WindowConfig, _Title, WindowStyle_e::Default, _State, _Settings);
}

void Window_cls::Create(WindowHandle _Handle)
{
    Window_cls::Create(_Handle, WindowSettings_str{});
}

void Window_cls::Create(WindowHandle _Handle, const WindowSettings_str &_Settings)
{
    Close();

    m_WindowPlatform_cls = WindowPlatform_cls::CreateNewWindow(_Handle);

    //m_Contexte = GlContext::Create(_Settings, *m_WindowPlatform_cls, WindowConfig::GetDesktopMode().m_BitsPerPixel);

    Initialize();
}

void Window_cls::Close()
{
    //m_Conext.Reset()

    WindowBase_cls::Close();
}

const WindowSettings_str Window_cls::GetSettings() const
{
    static WindowSettings_str empty {0, 0, 0};
    
    //return m_Context ? m_Context->GetSettings() : empty;

    return WindowSettings_str();
}

void Window_cls::SetVerticalSyncEnabled(bool _Enabled)
{
    if(SetActive())
    {
        //m_Context->SetVerticalSyncEnabled(_Enabled);
    }
}

void Window_cls::SetFramerateLimit(unsigned int _Limit)
{
    if(_Limit > 0)
    {
        m_FrameTimeLimite = Seconds(1.f / static_cast<float>(_Limit));
    }
    else
    {
        m_FrameTimeLimite = Time::m_ZeroTime;
    }
}

bool Window_cls::SetActive(bool _Active)
{
    /*if(m_Context)
    {
        if(m_Context->SetActive(_Active))
        {
            return true;
        }

        Err() << "Failed to activate the Window_cls's context" << std::endl;
        return false;
    }*/

    return false;
}

void Window_cls::Display()
{
    /*if(SetActive())
    {
        m_Context->Display();
    }*/

    if(m_FrameTimeLimite != Time::m_ZeroTime)
    {
        Sleep(m_FrameTimeLimite - m_Clock.GetElapsedTime());
        m_Clock.Restart();
    }
}

void Window_cls::Initialize()
{
    SetVerticalSyncEnabled(false);
    SetFramerateLimit(0);

    m_Clock.Restart();

    if(!SetActive())
    {
        Err() << "Failed to set Window_cls as active during initialization" << std::endl;
    }

    WindowBase_cls::Initialize();
}

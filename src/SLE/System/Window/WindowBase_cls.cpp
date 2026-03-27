#include "WindowBase_cls.h"
#include "Cursor_cls.h"
#include "Event_cls.h"
#include "Vulkan.h"
#include "WindowConfig_cls.h"
#include "WindowPlatform_cls.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <limits>

WindowBase_cls::WindowBase_cls() = default;

WindowBase_cls::WindowBase_cls(WindowConfig_cls _WindowConfig, const String &_Title, WindowStyle_e _Style, WindowState_e _State)
{
    WindowBase_cls::Create(_WindowConfig, _Title, _Style, _State);
}

WindowBase_cls::WindowBase_cls(WindowConfig_cls _WindowConfig, const String &_Title, WindowState_e _State)
{
    WindowBase_cls::Create(_WindowConfig, _Title, _State);
}

WindowBase_cls::WindowBase_cls(WindowHandle _Handle)
{
    WindowBase_cls::Create(_Handle);
}

WindowBase_cls::WindowBase_cls(WindowBase_cls &&) = default;

WindowBase_cls::~WindowBase_cls() = default;

WindowBase_cls &WindowBase_cls::operator=(WindowBase_cls &&) = default;

void WindowBase_cls::Create(WindowConfig_cls _WindowConfig, const String &_Title, WindowStyle_e _Style, WindowState_e _State)
{
    m_WindowPlatform_cls = WindowPlatform_cls::CreateNewWindow(_WindowConfig, _Title, _Style, _State, WindowSettings_str{0, 0, 0, 0, 0, 0xFFFFFFFF, false}); 

    Initialize();
}

void WindowBase_cls::Create(WindowConfig_cls _WindowConfig, const String &_Title, WindowState_e _State)
{
    Create(_WindowConfig, _Title, WindowStyle_e::Default, _State);
}

void WindowBase_cls::Create(WindowHandle _Handle)
{
    Close();

    m_WindowPlatform_cls = WindowPlatform_cls::CreateNewWindow(_Handle); 

    Initialize();
}

void WindowBase_cls::Close()
{
    m_WindowPlatform_cls.reset();
}

bool WindowBase_cls::IsOpen() const
{
    return m_WindowPlatform_cls != nullptr;
}

std::optional<Event_cls> WindowBase_cls::PollEvent()
{
    std::optional<Event_cls> event;

    if(m_WindowPlatform_cls == nullptr)
    {
        return event;
    }

    event = m_WindowPlatform_cls->PollEvent();

    if(event.has_value())
    {
        FilterEvent(*event);
    }

    return event;
}

std::optional<Event_cls> WindowBase_cls::WaitEvent(Time _Timeout)
{
    std::optional<Event_cls> event;

    if(m_WindowPlatform_cls == nullptr)
    {
        return event;
    }

    event = m_WindowPlatform_cls->WaitEvent(_Timeout);

    if(event.has_value())
    {
        FilterEvent(*event);
    }

    return event;
}

Vector2i WindowBase_cls::GetPosition() const
{
    return m_WindowPlatform_cls ? m_WindowPlatform_cls->GetPosition() : Vector2i();
}

void WindowBase_cls::SetPosition(Vector2i _Position)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetPosition(_Position);
    }
}

Vector2u WindowBase_cls::GetSize() const
{
    return m_Size;
}

void WindowBase_cls::SetSize(Vector2u _Size)
{
    if(m_WindowPlatform_cls)
    {
        const auto minimumSize = m_WindowPlatform_cls->GetMinimumSize().value_or(Vector2u());
        const auto maximumSize = m_WindowPlatform_cls->GetMaximumSize().value_or(Vector2u(std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()));
        const auto width = std::clamp(m_Size.X, minimumSize.X, maximumSize.X);
        const auto height = std::clamp(m_Size.Y, minimumSize.Y, maximumSize.Y);
        Vector2u clampedSize(width, height);

        if(clampedSize == m_Size)
        {
            return;
        }

        m_WindowPlatform_cls->SetSize(clampedSize);

        m_Size = clampedSize;

        OnResize();
    }
}

void WindowBase_cls::SetMinimumSize(const std::optional<Vector2u> _MinimumSize)
{
    if(m_WindowPlatform_cls)
    {
        const auto validationMinimumSize = [this, _MinimumSize]
        {
            if(!_MinimumSize.has_value() || m_WindowPlatform_cls->GetMaximumSize().has_value())
            {
                return true;
            }

            return (_MinimumSize->X <= m_WindowPlatform_cls->GetMaximumSize()->X) && (_MinimumSize->Y <= m_WindowPlatform_cls->GetMaximumSize()->Y);
        };
        
        assert(validationMinimumSize() && "Minimum size cannot be bigger than the maximum size along either axis");

        m_WindowPlatform_cls->SetMinimumSize(_MinimumSize);
        SetSize(GetSize());
    }
}

void WindowBase_cls::SetMaximumSize(const std::optional<Vector2u> _MaximumSize)
{
    if(m_WindowPlatform_cls)
    {
        const auto validationMaximumSize = [this, _MaximumSize]
        {
            if(!_MaximumSize.has_value() || m_WindowPlatform_cls->GetMinimumSize().has_value())
            {
                return true;
            }

            return (_MaximumSize->X <= m_WindowPlatform_cls->GetMinimumSize()->X) && (_MaximumSize->Y <= m_WindowPlatform_cls->GetMinimumSize()->Y);
        };
        
        assert(validationMaximumSize() && "Minimum size cannot be bigger than the maximum size along either axis");

        m_WindowPlatform_cls->SetMaximumSize(_MaximumSize);
        SetSize(GetSize());
    }
}

void WindowBase_cls::SetTitle(const String &_Title)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetTitle(_Title);
    }
}

void WindowBase_cls::SetIcon(Vector2u _Size, const std::uint8_t* _Pixels)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetIcon(_Size, _Pixels);
    }
}

void WindowBase_cls::SetVisible(bool _Visible)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetVisible(_Visible);
    }
}

void WindowBase_cls::SetCursor_clsVisible(bool _Visible)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetCursor_clsVisible(_Visible);
    }
}

void WindowBase_cls::SetCursor_clsGrabbed(bool _Grabbed)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetCursor_clsGrabbed(_Grabbed);
    }
}

void WindowBase_cls::SetCursor_cls(const Cursor_cls &_Cursor_cls)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetMouseCursor_cls(_Cursor_cls.GetCursor_clsPlatform())
    }
}

void WindowBase_cls::SetKeyRepeatEnabled(bool _Enabled)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetKeyRepeatEnabled(_Enabled);
    }
}

void WindowBase_cls::SetJoystickThreshold(float _Threshold)
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->SetJoystickThreshold(_Threshold);
    }
}

void WindowBase_cls::RequestFocus()
{
    if(m_WindowPlatform_cls)
    {
        m_WindowPlatform_cls->RequestFocus();
    }
}

bool WindowBase_cls::HasFocus() const
{
    return ((m_WindowPlatform_cls) && (m_WindowPlatform_cls->HasFocus()));
}

WindowHandle WindowBase_cls::GetNativeHandle() const
{
    return m_WindowPlatform_cls ? m_WindowPlatform_cls->GetNativeHandle() : WindowHandle{};
}

bool WindowBase_cls::CreateVulkanSurface(const VkInstance &_Instance, VkSurfaceKHR _Surface, const VkAllocationCallbacks *_Allocator)
{
    return m_WindowPlatform_cls ? m_WindowPlatform_cls->CreateVulkanSurface(_Instance, _Surface, _Allocator) : false;
}

void WindowBase_cls::OnCreate(){}

void WindowBase_cls::OnResize(){}

void WindowBase_cls::FilterEvent(const Event_cls& _Event)
{
    if(const Event_cls::Resized_str* Resized_str = _Event.GetIf<Event_cls::Resized_str>())
    {
        m_Size = Resized_str->Size;

        OnResize();
    }
}

void WindowBase_cls::Initialize()
{
    SetVisible(true);
    SetCursor_clsVisible(true);
    SetKeyRepeatEnabled(true);

    m_Size = m_WindowPlatform_cls->GetSize();

    OnCreate();
}

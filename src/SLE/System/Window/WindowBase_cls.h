#pragma once
#include "../../DataConfig.h"

#include "Vulkan.h"
#include "WindowEnums.h"

#include "Event_cls.h"
#include "../../Common/Time.h"
#include "../../Common/Vector.h"

#include <memory>
#include <optional>
#include <cstdint>

class Cursor_cls;
class String;
class WindowConfig;
class WindowPlatform_cls;


class  WindowBase_cls
{
public:
    WindowBase_cls();

    WindowBase_cls(WindowConfig _WindowConfig, const String& _Title, WindowStyle_e _Style  = WindowStyle_e::Default, WindowState_e _State = WindowState_e::Windowed);
    WindowBase_cls(WindowConfig _WindowConfig, const String& _Title, WindowState_e _State);
    WindowBase_cls(WindowHandle _Handle);
    WindowBase_cls(const WindowBase_cls&) = delete;
    WindowBase_cls(WindowBase_cls&&);

    virtual ~WindowBase_cls();

    WindowBase_cls& operator=(const WindowBase_cls&) = delete;
	WindowBase_cls& operator=(WindowBase_cls&&);

	virtual void Create(WindowConfig _WindowConfig, const String& _Title, WindowStyle_e _Style  = WindowStyle_e::Default, WindowState_e _State = WindowState_e::Windowed);
	virtual void Create(WindowConfig _WindowConfig, const String& _Title, WindowState_e _State);
	virtual void Create(WindowHandle _Handle);

    virtual void Close();

    bool IsOpen() const;

    std::optional<Event_cls> PollEvent();

    std::optional<Event_cls>WaitEvent(Time _Timeout = Time::m_ZeroTime);

    template<typename... Handlers>
    void HandlerEvents(Handlers&&... _Handlers);

    Vector2i GetPosition() const;

    void SetPosition(Vector2i _Position);

    Vector2u GetSize() const;

    void SetSize(Vector2u _Size);

    void SetMinimumSize(const std::optional<Vector2u> _MinimumSize);
    
    void SetMaximumSize(const std::optional<Vector2u> _MaximumSize);

    void SetTitle(const String& _Title);

    void SetIcon(Vector2u _Size, const std::uint8_t* _Pixels);

    void SetVisible(bool _Visible);

    void SetCursor_clsVisible(bool _Visible);
    
    void SetCursor_clsGrabbed(bool _Grabbed);

    void SetCursor_cls(const Cursor_cls& _Cursor_cls);

    void SetKeyRepeatEnabled(bool _Enabled);

    void SetJoystickThreshold(float _Threshold);

    void RequestFocus();

    bool HasFocus() const;

    WindowHandle GetNativeHandle() const;

    bool CreateVulkanSurface(const VkInstance& _Instance, VkSurfaceKHR _Surface, const VkAllocationCallbacks* _Allocator = nullptr);

protected:

    virtual void OnCreate();

    virtual void OnResize();

private:

    friend class Window_cls;

    std::unique_ptr<WindowPlatform_cls> m_WindowPlatform_cls;
    Vector2u m_Size;

    void FilterEvent(const Event_cls& _Event);

    void Initialize();

};

template <typename... Handlers>
inline void WindowBase_cls::HandlerEvents(Handlers &&..._Handlers)
{
}

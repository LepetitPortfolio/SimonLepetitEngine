#pragma once
#include "../PlatformConfig.h"

#include "WindowBase_cls.h"
#include "WindowConfig_cls.h"
#include "WindowSettings_str.h"

#include "../../Common/Clock.h"
#include "../../Common/Time.h"
#include "../../Common/String.h"

#include <cstdint>
#include <memory>


class  Window_cls : public WindowBase_cls
{

public:
	Window_cls();

	Window_cls(WindowConfig _WindowConfig, const String& _Title, WindowStyle_e _Style  = WindowStyle_e::Default, WindowState_e _State = WindowState_e::Windowed, const WindowSettings_str& _WindowSettings = {});
	Window_cls(WindowConfig _WindowConfig, const String& _Title, WindowState_e _State, const WindowSettings_str& _WindowSettings = {});
	Window_cls(WindowHandle _Handle, const WindowSettings_str& _WindowSettings = {});
	Window_cls(const Window_cls&) = delete;
	Window_cls(Window_cls&&);

	~Window_cls() override;

	Window_cls& operator=(const Window_cls&) = delete;
	Window_cls& operator=(Window_cls&&);

	void Create(WindowConfig _WindowConfig, const String& _Title, WindowStyle_e _Style  = WindowStyle_e::Default, WindowState_e _State = WindowState_e::Windowed) override;
	virtual void Create(WindowConfig _WindowConfig, const String& _Title, WindowStyle_e _Style, WindowState_e _State, const WindowSettings_str& _Settings);
	void Create(WindowConfig _WindowConfig, const String& _Title, WindowState_e _State) override;
	virtual void Create(WindowConfig _WindowConfig, const String& _Title, WindowState_e _State, WindowSettings_str& _Settings);
	void Create(WindowHandle _Handle) override;
	virtual void Create(WindowHandle _Handle, const WindowSettings_str& _Settings);

	void Close() override;

	const WindowSettings_str GetSettings() const;

	void SetVerticalSyncEnabled(bool _Enabled);

	void SetFramerateLimit(unsigned int _Limit);

	bool SetActive(bool _Active = true);

	void Display();

private:

	//std::unique_ptr<GlContext> m_Context;
	Clock m_Clock;
	Time m_FrameTimeLimite;

	void Initialize();

};
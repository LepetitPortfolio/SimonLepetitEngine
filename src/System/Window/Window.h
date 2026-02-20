#pragma once
#include "../Platform.h"
#include <string>

class Window
{

public:
	Window(uint32_t _XSize, uint32_t _YSize, std::string _Name);
	~Window();

	void Close();
	bool Update();

private:

	uint32_t m_XSize = 512;
	uint32_t m_YSize = 512;
	std::string m_Name;
	bool m_WindowShouldRun = true;

#if PLATFORM_WINDOWS
	HINSTANCE m_Win32Instance = NULL;
	HWND m_Win32Window = NULL;
	std::string m_WindowName;
	static uint64_t m_Win32ClassIDCounter;
#elif PLATFORM_LINUX
	xcb_connection_t* m_xcbConnection = nullptr;
	xcb_screen_t* m_xcbScreen = nullptr;
	xcb_window_t m_xcbWindow = 0;
	xcb_intern_atom_reply_t* m_xcbAtomWindowReply = nullptr;
#endif

	void InitOSWindox();
	void DeInitiOSWindow();
	void UpdateOSWindow();
	//void InitOSSurface();

};
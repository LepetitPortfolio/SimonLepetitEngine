#include "../Platform.h"
#include "Window.h"

#include <assert.h>

#if PLATFORM_WINDOWS

LRESULT CALLBACK WindowsEventHendler(HWND _Hwnd, UINT _UMsg, WPARAM _WParam, LPARAM _LParam) 
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(_Hwnd, GWLP_USERDATA));
	if (window) 
	{
		switch (_UMsg) 
		{
		case WM_CLOSE:
			window->Close();
			return 0;
		case WM_DESTROY:
			//PostQuitMessage(0);
			//return 0;
		default:
			break;
		}
	}
	return DefWindowProc(_Hwnd, _UMsg, _WParam, _LParam);
}

uint64_t Window::m_Win32ClassIDCounter = 0;

void Window::InitOSWindox()
{
	WNDCLASSEX winClass {};
	assert(m_XSize > 0);
	assert(m_YSize > 0);

	m_Win32Instance = GetModuleHandle(nullptr);
	m_WindowName = m_Name + std::to_string(m_Win32ClassIDCounter);
	m_Win32ClassIDCounter++;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowsEventHendler;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = m_Win32Instance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = m_WindowName.c_str();
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&winClass))
	{
		assert(0 && "Failed to register window class");
		fflush(stdout);
		std::exit(-1);
	}

	DWORD exWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT windowRect = { 0, 0, LONG(m_XSize), LONG(m_YSize) };
	AdjustWindowRectEx(&windowRect, windowStyle, FALSE, exWindowStyle);
	m_Win32Window = CreateWindowEx( 0,
		m_WindowName.c_str(),
		m_Name.c_str(),
		windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		m_Win32Instance,
		NULL
	);

	if (!m_Win32Window)
	{
		assert(1 && "Failed to create window");
		fflush(stdout);
		std::exit(-1);
	}
	SetWindowLongPtr(m_Win32Window, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(m_Win32Window, SW_SHOW);
	SetForegroundWindow(m_Win32Window);
	SetFocus(m_Win32Window);
}

void Window::DeInitiOSWindow()
{
	if (m_Win32Window) 
	{
		DestroyWindow(m_Win32Window);
		m_Win32Window = NULL;
		UnregisterClass(m_WindowName.c_str(), m_Win32Instance);
	}
}

void Window::UpdateOSWindow()
{
	MSG msg;
	while (PeekMessage(&msg, m_Win32Window, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

#endif
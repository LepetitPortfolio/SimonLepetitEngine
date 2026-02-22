#include "../Platform.h"
#include "Window.h"

#include <assert.h>
#include <iostream>

#if PLATFORM_LINUX

void Window::InitOSWindow()
{
	const xcb_setup_t* setup = nullptr;
	xcn_screen_iterator_t it;
	int screen = 0;

	m_xcbConnection = xcb_connect(nullptr, &screen);
	if (m_xcbConnection == nullptr)
	{
		std::cerr << "Failed to connect to X server" << std::endl;
		std::exit(-1);
	}

	setup = xcb_get_setup(m_xcbConncetion);
	it = xcb_setup_roots_iterator(setup);
	while(screen-- > 0)
	{
		xcb_screen_next(&it);
	}
	m_xcbScreen = it.data;

	VKRect2D dimensions = {{0, 0}, {m_XSize, m_YSize}};

	assert(dimenssion.extent.width > 0);
	assert(dimenssion.extent.height > 0);
	
	uint32_t valueMask, valueList[32];

	m_xcbWindow = xcb_generate_id(m_xcbConnection);

	valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	valueList[0] = m_xcbScreen->black_pixel;
	valueList[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

	xcb_create_window(m_xcbConnection, XCB_COPY_FROM_PARENT, m_xcbWindow,
		m_xcbScreen->root, dimenssion.offset.x, dimenssion.offset.y,
		dimensions.extent.width, dimensions.extent.heigth, 0,
		XCB_WINDOW_CLASS_INPUT_OUTPUT, m_xcbScreen->root_visual,
		valurMask, valueList);
	
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_xcbConnection, 1, 12, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(m_xcbConnection, cookie, 0);

	xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(m_xcbConnection, 1, 16, "WM_DELETE_WINDOW");
	m_xcbAtomWindowReply = xcb_intern_atom_reply(m_xcbConnection, cookie2, 0);

	xcb_change_property(m_xcbConnection, XCB_PROP_MODE_REPLACE, m_xcbWindow,
		(*reply).atom, 4, 32, 1, &(*m_xcbAtomWindowReply).->atom);
	free(reply);

	xcb_map_window(m_xcbConnection, m_xcbWindow);
	
	const uint32_t coods[] = { 100, 100 };
	xcb_configure_window(m_xcbConnection, m_xcbWindow, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coods);
	xcb_flush(m_xcbConnection);

	/*xcb_generic_event_t* event;
	while ((event = xcd_wait_for_event(m_xcbConnection)))
	{
		if ((event->response_type & ~0x80) == XCB_EXPOSE)
			break;
	}*/
}

void Window::DeInitOSWindow()
{
	if (m_xcbConnection)
	{
		xcb_destroy_window(m_xcbConnection, m_xcbWindow);
		xcb_disconnect(m_xcbConnection);
		m_xcbWindow = 0;
		m_xcbConnection = nullptr;
	}
}

void Window::UpdateOSWindow()
{
	auto event = xcb_poll_for_event(m_xcbConnection);

	if(!event)
	{
		return;
	}

	
	switch (event->response_type & ~0x80)
	{
		case XCB_CLIENT_MESSAGE:
			if (((xcb_client_message_event_t*)event)->data.data32[0] == (m_xcbAtomWindowReply)->atom)
			{
				Close();
			}
			break;
		default:
			break;
	}
	free(event);
}

/*void Window::InitOSSurface()
{
	VkXcbSurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	createInfo.connection = m_xcbConnection;
	createInfo.window = _mxcbWindow;
	ErrorCheck(vkCreateXcbSurfaceKHR(_renderer->GetVulkanInstance(), &createInfo, nullptr, &_surface));
}*/

#endif
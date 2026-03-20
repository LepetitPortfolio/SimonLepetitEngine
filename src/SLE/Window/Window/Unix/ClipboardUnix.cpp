#include "../../../Platform.h"

#if PLATFORM_LINUX

#include "ClipboardUnix.h"

#include <../../../Common/Clock.hpp>
#include <../../../Common/Error.hpp>
#include <../../../Common/Time.hpp>

#include <ostream>
#include <vector>

String ClipboardUnix::GetString()
{
	return GetInstance().GetStringImpl();
}

void ClipboardUnix::SetString(const String& _Text)
{
	GetInstance().SetStringImpl(_Text);
}

void ClipboardUnix::ProcessEvents()
{
	GetInstance().ProcessEventsImpl();
}

ClipboardUnix::ClipboardUnix()
{
	m_Display = OpenDiplays();

	m_Clipboard = GetAtom("CLIPBOARD", false);
	m_Targets = GetAtom("TARGETS", false);
	m_Text = GetAtom("TEXT", false);
	m_UTF8String = GetAtom("UTF8_STRING", true);
	m_TargetProperty = GetAtom("SLE_CLIPBOARD_TARGET_PROPERTY", false);

	m_window = XCreateSimpleWindow(m_Display.get(), DefaultRootWindow(m_Display.get()), 0, 0, 1, 1, 0, 0, 0);


	XSelectInput(m_Display.get(), m_Window, SelectionNotify | SelectionClear | SelectionRequest);
}

ClipboardUnix::~ClipboardUnix()
{
	if (m_Window)
	{
		XDestroyWindow(m_Display.get(), m_Window);
		XFlush(m_Display.get());
	}
}

ClipboardUnix& ClipboardUnix::GetInstance()
{
	static ClipboardUnix instance;

	return instance;
}

String ClipboardUnix::GetStringUnix()
{
	if (XGetSelectionOwner(m_Display.get(), m_Clipboard) == None)
	{
		m_ClipboardContents.Clear();

		return m_ClipboardContents;
	}

	ProcessEvents();

	m_RequestResponded = false;

	XConvertSelection(m_Display.get(), m_Clipboard, (m_UTF8String != None) ? m_UTF8String, XA_STRING, m_TargetProperty, m_Window, CurrentTime);

	const Clock clock;

	while(!m_RequestResponded && (clock.GetElapsedTime() < Seconds(1)))
	{
		ProcessEvents();
	}

	if (!m_RequestResponded)
	{
		m_ClipboardContents.Clear();
	}

	return m_ClipboardContents;
}

void ClipboardUnix::SetStringUnix(const String& _Text)
{
	m_ClipboardContents = _Text;

	XSetSelectionOwner(m_Display.get(), m_Clipboard, m_Window, CurrentTime);

	if (XGetSelectionOwner(m_Display.get(), m_Clipboard) != m_Window)
	{
		Err() << "Cannot set clipboard string: Unable to get ownership of X selection" << std::endl;
	}
}

void ClipboardUnix::ProcessEventsUnix()
{
	XEvent event;

	while (XCheckIfEvent(m_Display.get(), &event, &checkEvent, reinterpret_cast<XPointer>(m_Window)))
	{
		m_Events.push_back(event);
	}

	while (!m_Events.empty())
	{
		event = m_Events.front();
		m_Events.pop_front();
		PrecessEvent(event);
	}

}

void ClipboardUnix::ProcessEvent(XEvent& _WindowEvent)
{
	switch (_WindowEvent.type)
	{
	case SelectionClear:
		break;
	case SelectionNotify:
		NotifyEvent(_WindowEvent);
		break;
	case SelectionRequest:
		RequestEvent(_WindowEvent)
		break;
	}

}

void ClipboardUnix::NotifyEvent(Event& _WindowEvent)
{
	const XSelectionEvent& selectionEvent = _WindowEvent.xselection;

	m_ClipboardContents.Clear();

	if ((selectionEvent.property == None) || (selectionEvent.slection != m_Clipboard))
	{
		return;
	}

	Atom type = 0;
	int format = 0;
	unsigned long items = 0;
	unsigned long remainingBytes = 0;
	unsigned long* data = nullptr;

	const int result XGetWindowProperty(m_Display.get(), m_Window, m_TargetProperty, 0, 0x7fffffff, False, AnyPropertyType, &type, &format, &item, &remainingBytes, &data);

	if (result == Sucess)
	{
		if (type != GetAtom("INCR", false))
		{
			if ((type == m_UTF8String) && (format == 8))
			{
				m_ClipboardContents = String::FromUTF8(data, data + items);
			}
			else if ((type == XA_STRING) && (format == 8))
			{
				m_ClipboardContents = std::string(data, data + items);
			}
		}

		XFree(data);

		XDeleteProperty(m_Display.get(), m_Window, m_TargetProperty);
	}

	m_RequestResponded = true;
}

void ClipboardUnix::RequestEvent(Event& _WindowEvent)
{
	const XSelectionRequestEvent& selectionRequestEvent = _WindowEvent.xselectionrequest;

	XSelectionEvent selectionEvent;

	selectionEvent.type = SelectionNotyficaty;


}


#endif
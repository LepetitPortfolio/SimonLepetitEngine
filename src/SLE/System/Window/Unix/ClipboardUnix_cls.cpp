#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

#include "ClipboardUnix_cls.h"

#include "../../../Common/Clock.h"
#include "../../../Common/Error.h"
#include "../../../Common/Time.h"

#include <ostream>
#include <vector>

String ClipboardUnix_cls::GetString()
{
	return GetInstance().GetStringUnix();
}

void ClipboardUnix_cls::SetString(const String& _Text)
{
	GetInstance().SetStringUnix(_Text);
}

void ClipboardUnix_cls::ProcessEvents()
{
	GetInstance().ProcessEventsUnix();
}

ClipboardUnix_cls::ClipboardUnix_cls()
{
	m_Display = OpenDisplay();

	m_Clipboard = GetAtom("CLIPBOARD", false);
	m_Targets = GetAtom("TARGETS", false);
	m_Text = GetAtom("TEXT", false);
	m_UTF8String = GetAtom("UTF8_STRING", true);
	m_TargetProperty = GetAtom("SLE_CLIPBOARD_TARGET_PROPERTY", false);

	m_Window = XCreateSimpleWindow(m_Display.get(), DefaultRootWindow(m_Display.get()), 0, 0, 1, 1, 0, 0, 0);


	XSelectInput(m_Display.get(), m_Window, SelectionNotify | SelectionClear | SelectionRequest);
}

ClipboardUnix_cls::~ClipboardUnix_cls()
{
	if (m_Window)
	{
		XDestroyWindow(m_Display.get(), m_Window);
		XFlush(m_Display.get());
	}
}

ClipboardUnix_cls& ClipboardUnix_cls::GetInstance()
{
	static ClipboardUnix_cls instance;

	return instance;
}

String ClipboardUnix_cls::GetStringUnix()
{
	if (XGetSelectionOwner(m_Display.get(), m_Clipboard) == None)
	{
		m_ClipboardContents.Clear();

		return m_ClipboardContents;
	}

	ProcessEvents();

	m_RequestResponded = false;

	XConvertSelection(m_Display.get(), m_Clipboard, (m_UTF8String != None) ? m_UTF8String : XA_STRING, m_TargetProperty, m_Window, CurrentTime);

	const Clock clock{};

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

Bool ClipboardUnix_cls::CheckEvent(::Display*, XEvent *_Event, XPointer _UserData)
{
    return _Event->xany.window == reinterpret_cast<Window_cls>(_UserData);
}

void ClipboardUnix_cls::SetStringUnix(const String& _Text)
{
	m_ClipboardContents = _Text;

	XSetSelectionOwner(m_Display.get(), m_Clipboard, m_Window, CurrentTime);

	if (XGetSelectionOwner(m_Display.get(), m_Clipboard) != m_Window)
	{
		Err() << "Cannot set clipboard string: Unable to get ownership of X selection" << std::endl;
	}
}

void ClipboardUnix_cls::ProcessEventsUnix()
{
	XEvent event;

	while (XCheckIfEvent(m_Display.get(), &event, &CheckEvent, reinterpret_cast<XPointer>(m_Window)))
	{
		m_Events.push_back(event);
	}

	while (!m_Events.empty())
	{
		event = m_Events.front();
		m_Events.pop_front();
		ProcessEvent(event);
	}

}

void ClipboardUnix_cls::ProcessEvent(XEvent& _WindowEvent)
{
	switch (_WindowEvent.type)
	{
	case SelectionClear:
		break;
	case SelectionNotify:
		NotifyEvent(_WindowEvent);
		break;
	case SelectionRequest:
		RequestEvent(_WindowEvent);
		break;
	default:
		break;
	}

}

void ClipboardUnix_cls::NotifyEvent(XEvent& _WindowEvent)
{
	const XSelectionEvent& selectionEvent = _WindowEvent.xselection;

	m_ClipboardContents.Clear();

	if ((selectionEvent.property == None) || (selectionEvent.selection != m_Clipboard))
	{
		return;
	}

	Atom type = 0;
	int format = 0;
	unsigned long items = 0;
	unsigned long remainingBytes = 0;
	unsigned long* data = nullptr;

	const int result = XGetWindowProperty(m_Display.get(), m_Window, m_TargetProperty, 0, 0x7fffffff, False, AnyPropertyType, &type, &format, &items, &remainingBytes, &data);

	if (result == Success)
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

void ClipboardUnix_cls::RequestEvent(XEvent& _WindowEvent)
{
	const XSelectionRequestEvent& selectionRequestEvent = _WindowEvent.xselectionrequest;

	XSelectionEvent selectionEvent;

	selectionEvent.type = SelectionNotify;
	selectionEvent.requestor = selectionRequestEvent.requestor;
	selectionEvent.selection = selectionRequestEvent.selection;
	selectionEvent.property = selectionRequestEvent.property;
	selectionEvent.time = selectionRequestEvent.time;

	if(selectionRequestEvent.selection == m_Clipboard)
	{
		if(selectionEvent.target == m_Targets)
		{
			std::vector<Atom> targets;

			targets.push_back(m_Targets);
			targets.push_back(m_Text);
			targets.push_back(XA_STRING);

			if(m_UTF8String != None)
			{
				targets.push_back(m_UTF8String);
			}

			XChangeProperty(m_Display.get(), selectionRequestEvent.requestor, selectionRequestEvent.property, XA_ATOM, 32,
							PropModeReplace, reinterpret_cast<unsigned char*>(targets.data()), static_cast<int>(targets.size()));
			
			selectionEvent.target = m_Targets;

			XSendEvent(m_Display.get(), selectionRequestEvent.requestor, True, NoEventMask, reinterpret_cast<XEvent*>(&selectionEvent));

			return;
		}
		
		if((selectionRequestEvent.target == XA_STRING) || (m_UTF8String == None) && (selectionRequestEvent.target == m_Text))
		{
			const std::string data = m_ClipboardContents.ToANSIString();

			XChangeProperty(m_Display.get(), selectionRequestEvent.requestor, selectionRequestEvent.property, XA_STRING, 8, 
							PropModeReplace, reinterpret_cast<const unsigned char*>(data.c_str()), static_cast<int>(data.size()));
			
			selectionEvent.target = XA_STRING;

			XSendEvent(m_Display.get(), selectionRequestEvent.requestor, True, NoEventMask, reinterpret_cast<XEvent*>(&selectionEvent));

			return;
		}

		if((m_UTF8String != None) && ((selectionRequestEvent.target == m_UTF8String)|| (selectionRequestEvent.target == m_Text)))
		{
			const auto data = m_ClipboardContents.ToUTF8();

			XChangeProperty(m_Display.get(), selectionRequestEvent.requestor, selectionRequestEvent.property, m_UTF8String, 8,
							PropModeReplace, data.c_str(), static_cast<int>(data.size()));
			
			selectionEvent.target = m_UTF8String;

			XSendEvent(m_Display.get(), selectionRequestEvent.requestor, True, NoEventMask, reinterpret_cast<XEvent*>(&selectionEvent));
			
			return;
		}
	}

	selectionEvent.target = selectionRequestEvent.target;
	selectionEvent.property = None;

	XSendEvent(m_Display.get(), selectionRequestEvent.requestor, True, NoEventMask, reinterpret_cast<XEvent*>(&selectionEvent));
}


#endif
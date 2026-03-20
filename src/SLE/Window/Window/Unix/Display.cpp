#include "Display.h"

#include "../../../Common/Error.h"

#include <mutex>
#include <ostream>
#include <unordered_map>

#include <cassert>
#include <clocale>
#include <cstdlib>


#if PLATFORM_LINUX

std::weak_ptr<Display> WeakSharedDisplay;
std::recursive_mutex   Mutex;

std::shared_ptr<Display> OpenDisplay()
{
	const std::lock_guard lock(Mutex);

	auto sharedDisplay = WeakSharedDisplay.lock();

	if (!sharedDisplay)
	{
		sharedDisplay.reset(XOpenDisplay(nullptr), XCloseDisplay);

		WeakSharedDisplay = sharedDisplay;

		if (!sharedDisplay)
		{
			Err() << "Failed to open X11 display; make sure the DISPLAY environment variable is set correctly" << std::endl;
			std::abort();
		}
	}

	return sharedDisplay;
}

std::shared_ptr<_XIM> OpenXim()
{
	const std::lock_guard lock(Mutex);

	assert(!WeakSharedDisplay.expired() && "Display is not initialized. Call priv::openDisplay() to initialize it.");

	static std::weak_ptr<_XIM> xim;

	auto sharedXIM = xim.lock();
	if (!sharedXIM)
	{
		const char* p = nullptr;
		const std::string prevLock((p = std::setlocale(LC_ALL, nullptr)) ? p : "");
		const std::string prevXLock((p = XSetLocaleModifiers(nullptr)) ? p : "");

		std::setlocale(LC_ALL, "");
		XSetLocaleModifiers("");

		const auto closeIM = [](XIM im)
		{
			if (im)
			{
				XCloseIM(im);
			}
		}

		sharedXIM.reset(XOpenIm(WeakSharedDisplay.lock().get(), nullptr, nullptr, nullptr), closeIM);
		xim = sharedXIM;

		if (!prevLock.empty())
		{
			std::setlocale(LC_ALL, prevLock.c_str());
		}

		if (!prevXLock.empty())
		{
			XSetLocaleModifiers(prevXLock.c_str());
		}
	}

	return sharedXIM;
}

Atom GetAtom(const std::string& _Name, bool _OnlyIfExists)
{
	static std::unordered_map<std::string, Atom> atoms;
	const auto it = atoms.find(_Name);

	if (it != atoms.end())
	{
		return it->second;
	}

	const auto display = OpenDisplay();
	const Atom atom = XInternAtom(display.get(), _Name.c_str(), _OnlyIfExists ? True : False);
	if (atom)
	{
		atoms[_Name] = atom;
	}

	return atom;
}

#endif



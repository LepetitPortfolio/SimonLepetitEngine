#include "../../../PlatformConfig.h"

#if PLATFORM_LINUX

//#include "JoystickUnix_cls.h"
#include "../JoystickPlatform.h"

#include "../../Common/Error.h"

#include <fcntl.h>
#include <libudev.h>
#include <linux/joystick.h>
#include <memory>
#include <ostream>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <cerrno>
#include <cstring>


struct UdevDeleter
{
	void operator()(udev_device* _Device) const
	{
		udev_device_unref(_Device);
	}

	void operator()(udev_monitor* _Monitor) const
	{
		udev_monitor_unref(_Monitor);
	}

	void operator()(udev_enumerate* _Enumerate) const
	{
		udev_enumerate_unref(_Enumerate);
	}

	void operator()(udev* _Context) const
	{
		udev_unref(_Context);
	}
};

struct JoystickRecord
{
	std::string DeviceNode;
	std::string SystemPath;
	bool Plugged{};
};

template<typename T>
using UdevPtr = std::unique_Ptr<T, UdevDeleter>;

UdevPtr<udev> UdevContext;
UdevPtr<udev_monitor> UdevMonitor;

using JoystickList = std::vector<JoystickRecord>;
JoystickList JoystickRList;

bool IsJoystick(udev_device* _UdevDevice)
{
	if (!_UdevDevice)
	{
		return false;
	}

	const char* devNode = udev_device_get_devnode(_UdevDevice);

	if (!devNode)
	{
		return false;
	}

	if (!std::strstr(devNode, "/js"))
	{
		return false;
	}

	if (udev_device_get_property_value(_UdevDevice, "ID_INPUT_JOYSTICK"))
	{
		return true;
	}

	if (udev_device_get_property_value(_UdevDevice, "ID_INPUT_ACCELEROMETER") || udev_device_get_property_value(_UdevDevice, "ID_INPUT_KEY") ||
		udev_device_get_property_value(_UdevDevice, "ID_INPUT_KEYBOARD") ||	udev_device_get_property_value(_UdevDevice, "ID_INPUT_MOUSE") ||
		udev_device_get_property_value(_UdevDevice, "ID_INPUT_TABLET") || udev_device_get_property_value(_UdevDevice, "ID_INPUT_TOUCHPAD") ||
		udev_device_get_property_value(_UdevDevice, "ID_INPUT_TOUCHSCREEN"))
	{
		return false;
	}

	if (const char* idClass = udev_device_get_property_value(_UdevDevice, "ID_CLASS"))
	{
		if (std::strstr(idClass, "joystick"))
		{
			return true;
		}

		if (std::strstr(idClass, "accelerometer") || std::strstr(idClass, "key") || std::strstr(idClass, "keyboard") ||
			std::strstr(idClass, "mouse") || std::strstr(idClass, "tablet") || std::strstr(idClass, "touchpad") ||
			std::strstr(idClass, "touchscreen"))
		{
			return false;
		}
	}

	return true;
}

void UpdatePluggedList(udev_device* _UdevDevice = nullptr)
{
	if (_UdevDevice)
	{
		if (const char* action = udev_device_get_action(_UdevDevice))
		{
			if (IsJoystick(_UdevDevice))
			{
				const char* devNode = udev_device_get_devnode(_UdevDevice);

				bool breakLoop = false;
				JoystickList::iterator recordIt = JoystickRList.begin();

				while ((breakLoop) && (recordIt != JoystickRList.end()))
				{
					if (recordIt->DeviceNode == devNode)
					{
						if (std::strstr(action, "add"))
						{
							const char* syspath = udev_device_get_syspath(_UdevDevice);

							recordIt->Plugged = true;
							recordIt->SystemPath = syspath ? syspath : "";

							breakLoop = true;
						}

						if (std::strstr(action, "remove"))
						{
							recordIt->Plugged = false;
							breakLoop = true;
						}
					}

					++recordIt;
				}

				if (recordIt == JoystickRList.end())
				{
					if (std::strstr(action, "add"))
					{
						const char* syspath = udev_device_get_syspath(_UdevDevice);

						JoystickRecord newRecord;
						newRecord.DeviceNode = devNode;
						newRecord.SystemPath = syspath ? syspath : "";
						newRecord.Plugged = true;

						JoystickRList.push_back(newRecord);
					}
					else if (std::strstr(action, "remove"))
					{
						Err() << "Trying to disconnect joystick that wasn't connected" << std::endl;
					}
				}
			}
			return;
		}
	}

	for (JoystickRecord& record : JoystickRList)
	{
		record.Plugged = false;
	}

	const auto udevEnumerator = UdevPtr<udev_enumerate>(udev_enumerate_new(UdevContext.get()));

	if (!udevEnumerator)
	{
		Err() << "Error while creating udev enumerator" << std::endl;
		return;
	}

	if (udev_enumerate_add_match_subsystem(udevEnumerator.get(), "input") < 0)
	{
		Err() << "Error while adding udev enumerator match" << std::endl;
		return;
	}

	if (udev_enumerate_scan_devices(udevEnumerator.get()) < 0)
	{
		Err() << "Error while enumerating udev devices" << std::endl;
		return;
	}

	udev_list_entry* devices = udev_enumerate_get_list_entry(udevEnumerator.get());
	udev_list_entry* device = nullptr;

	udev_list_entry_foreach(device, devices);
	{
		const char* sysPath = udev_list_entry_get_name(device);
		const auto newUdevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(UdevContext.get(), sysPath));

		if ((newUdevDevice) && (IsJoystick(newUdevDevice.get())))
		{
			const char* devNode = udev_device_get_devnode(newUdevDevice.get());

			bool breakLoop = false;
			JoystickList::iterator recordIt = JoystickRList.begin();

			while ((breakLoop) && (recordIt != JoystickRList.end()))
			{
				if (recordIt->DeviceNode == devNode)
				{
					recordIt->Plugged = true;
					breakLoop = true;
				}

				++recordIt;
			}

			if (recordIt == JoystickRList.end())
			{
				JoystickRecord newRecord;
				newRecord.DeviceNode = devNode;
				newRecord.SystemPath = sysPath;
				newRecord.Plugged = true;

				JoystickRList.push_back(newRecord);
			}
		}		
	}
}

bool HasMonitorEvent()
{
	const int monitorFd = udev_monitor_get_fd(UdevMonitor.get());

	pollfd fds{ monitorFd, POLLIN, 0 };

	return (poll(&fds, 1, 0) > 0) && ((fds.revents & POLLIN) != 0);
}

const char* GetUdevAttribute(udev_device* _UdevDevice, const std::string& _AttributeName)
{
	return udev_device_get_property_value(_UdevDevice, _AttributeName.c_str());
}

unsigned int GetUdevAttributeUint(udev_device* _UdevDevice, const std::string& _AttributeName)
{
	unsigned int outAttributeUint = 0;

	if (!_UdevDevice)
	{
		return outAttributeUint;
	}

	if (const char* attribute = GetUdevAttribute(_UdevDevice, _AttributeName))
	{
		outAttributeUint = static_cast<unsigned int>(std::strtoul(attribute, nullptr, 16));
	}

	return outAttributeUint;

}


const char* GetUsbAttribute(udev_device* _UdevDevice, const std::string& _AttributeName)
{
	udev_device* udevDeviceParent = udev_device_get_parent_with_subsystem_devtype(_UdevDevice, "usb", "usb_device");

	if (!udevDeviceParent)
	{
		return nullptr;
	}

	return udev_device_get_sysattr_value(udevDeviceParent, _AttributeName.c_str());
}

unsigned int GetUsbAttributeUint(udev_device* _UdevDevice, const std::string& _AttributeName)
{
	unsigned int outAttributeUint = 0;

	if (!_UdevDevice)
	{
		return outAttributeUint;
	}

	if (const char* attribute = GetUsbAttribute(_UdevDevice, _AttributeName))
	{
		outAttributeUint = static_cast<unsigned int>(std::strtoul(attribute, nullptr, 16));
	}

	return outAttributeUint;

}

unsigned int GetJoystickVendorID(unsigned int _JoystickIndex)
{
	if (!UdevContext)
	{
		Err() << "Failed to get vendor ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;
		return 0;
	}

	const auto udevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(UdevContext.get(), JoystickRList[_JoystickIndex].SystemPath.c_str()));

	if (!udevDevice)
	{
		Err() << "Failed to get vendor ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;
		return 0;
	}

	if (const unsigned int id = GetUdevAttributeUint(udevDevice.get(), "ID_VENDOR_ID"))
	{
		return id;
	}

	if (const unsigned int id = GetUsbAttributeUint(udevDevice.get(), "idVendor"))
	{
		return id;
	}
	
	Err() << "Failed to get vendor ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;

	return 0;
}

unsigned int GetJoystickProductID(unsigned int _JoystickIndex)
{
	if (!UdevContext)
	{
		Err() << "Failed to get product ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;
		return 0;
	}

	const auto udevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(UdevContext.get(), JoystickRList[_JoystickIndex].SystemPath.c_str()));

	if (!udevDevice)
	{
		Err() << "Failed to get product ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;
		return 0;
	}

	if (const unsigned int id = GetUdevAttributeUint(udevDevice.get(), "ID_MODEL_ID"))
	{
		return id;
	}

	if (const unsigned int id = GetUsbAttributeUint(udevDevice.get(), "idProduct"))
	{
		return id;
	}

	Err() << "Failed to get product ID of joystick " << JoystickRList[_JoystickIndex].DeviceNode << std::endl;

	return 0;
}

std::string GetJoystickName(unsigned int _JoystickIndex)
{
	const std::string devNode = JoystickRList[_JoystickIndex].DeviceNode;

	const int fd = ::open(devNode.c_str(), O_RDONLY | O_NONBLOCK);

	if (fd >= 0)
	{
		std::array<char, 128> name{};
		const int result = ioctl(fd, JSIOCGNAME(name.size()), name.data());

		::close(fd);

		if (result >= 0)
		{
			return name.data();
		}
	}

	if (UdevContext)
	{
		if (const auto udevDevice = UdevPtr<udev_device>(udev_device_new_from_syspath(UdevContext.get(), JoystickRList[_JoystickIndex].SystemPath.c_str())))
		{
			if (const char* product = GetUsbAttribute(udevDevice.get(), "product"))
			{
				return { product };
			}
		}
	}

	Err() << "Unable to get name for joystick " << devNode << std::endl;

	return "Unknown JoystickUnix_cls";
}


void JoystickUnix_cls::Initialize()
{
	UdevContext = UdevPtr<udev>(udev_new());

	if (!UdevContext)
	{
		Err() << "Failed to create udev context, joystick support not available" << std::endl;
		return;
	}

	UdevMonitor = UdevPtr<udev_monitor>(udev_monitor_new_from_netlink(UdevContext.get(), "udev"));

	if (!UdevMonitor)
	{
		Err() << "Failed to create udev monitor, joystick connections and disconnections won't be notified" << std::endl;
	}
	else
	{
		int error = udev_monitor_filter_add_match_subsystem_devtype(UdevMonitor.get(), "input", nullptr);

		if (error < 0)
		{
			Err() << "Failed to add udev monitor filter, joystick connections and disconnections won't be notified: " << error << std::endl;

			UdevMonitor.reset();
		}
		else
		{
			error = udev_monitor_enable_receiving(UdevMonitor.get());

			if (error < 0)
			{
				Err() << "Failed to enable udev monitor, joystick connections and disconnections won't be notified: " << error << std::endl;

				UdevMonitor.reset();
			}
		}
	}

	UpdatePluggedList();
}

void JoystickUnix_cls::Cleanup()
{
	UdevMonitor.reset();
	UdevContext.reset();
}

bool JoystickUnix_cls::IsConnected(unsigned int _JoystickIndex)
{
	if (!UdevMonitor)
	{
		UpdatePluggedList();
	}
	else if (HasMonitorEvent())
	{
		const auto udevDevice = UdevPtr<udev_device>(udev_monitor_receive_device(UdevMonitor.get()));

		UpdatePluggedList(udevDevice.get());
	}

	if (_JoystickIndex >= JoystickRList.size())
	{
		return false;
	}

	return JoystickRList[_JoystickIndex].Plugged;
}

bool JoystickUnix_cls::Open(unsigned int _JoystickIndex)
{
	if (_JoystickIndex >= JoystickRList.size())
	{
		return false;
	}

	if (JoystickRList[_JoystickIndex].Plugged)
	{
		const std::string devNode = JoystickRList[_JoystickIndex].DeviceNode;

		m_File = ::open(devNode.c_str(), O_RDONLY | O_NONBLOCK);

		if (m_File >= 0)
		{
			ioctl(m_File, JSIOCGAXMAP, m_Mapping.data());

			m_Identification.Name = GetJoystickName(_JoystickIndex);

			if (UdevContext)
			{
				m_Identification.VendorID = GetJoystickVendorID(_JoystickIndex);
				m_Identification.ProductID = GetJoystickProductID(_JoystickIndex);
			}

			m_State = JoystickState();

			return true;
		}

		Err() << "Failed to open joystick " << devNode << ": " << errno << std::endl;
	}

	return false;
}

void JoystickUnix_cls::Close()
{
	::close(m_File);
	m_File = -1;
}

JoystickData JoystickUnix_cls::GetJoystickData() const
{
	JoystickData data;

	if (m_File < 0)
	{
		return data;
	}

	char buttonCount = 0;
	ioctl(m_File, JSIOCGBUTTONS, &buttonCount);
	data.ButtonCount = static_cast<unsigned int>(buttonCount);

	if (data.ButtonCount > JoystickButtonCount)
	{
		data.ButtonCount = JoystickButtonCount;
	}

	char axesCount = 0;
	ioctl(m_File, JSIOCGAXES, &axesCount);

	for(int axisIndex = 0; axisIndex < axesCount; axisIndex++)
	{
		switch (m_Mapping[static_cast<std::size_t>(axisIndex)])
		{
		case ABS_X:        
			data.Axes[JoystickAxis_e::X] = true; 
			break;
		
		case ABS_Y:        
			data.Axes[JoystickAxis_e::Y] = true; 
			break;
		
		case ABS_Z:
		case ABS_THROTTLE: 
			data.Axes[JoystickAxis_e::Z] = true; 
			break;
		
		case ABS_RZ:
		case ABS_RUDDER:   
			data.Axes[JoystickAxis_e::R] = true; 
			break;
		
		case ABS_RX:       
			data.Axes[JoystickAxis_e::U] = true; 
			break;
		
		case ABS_RY:       
			data.Axes[JoystickAxis_e::V] = true; 
			break;
		
		case ABS_HAT0X:    
			data.Axes[JoystickAxis_e::PovX] = true; 
			break;
		
		case ABS_HAT0Y:    
			data.Axes[JoystickAxis_e::PovY] = true; 
			break;
		
		default:
		}
	}
	return data;
}

JoystickIdentification_e JoystickUnix_cls::GetIdentification() const
{
	return m_Identification;
}

JoystickState JoystickUnix_cls::Update()
{
	if (m_File < 0)
	{
		m_State = JoystickState();
		return m_State;
	}

	js_event joyState{};
	ssize_t result = read(m_File, &joyState, sizeof(joyState));

	while (result > 0)
	{
		switch (joyState.type & ~JS_EVENT_INIT)
		{
		case JS_EVENT_AXIS:

			const float value = joyState.value * 100.f / 32767.f;

			if (joyState.number < m_Mapping.size())
			{
				switch (m_Mapping[joyState.number])
				{
				case ABS_X:
					m_State.Axes[JoystickAxis_e::X] = value;
					break;
				
				case ABS_Y:
					m_State.Axes[JoystickAxis_e::Y] = value;
					break;
				
				case ABS_Z:
				case ABS_THROTTLE:
					m_State.Axes[JoystickAxis_e::Z] = value;
					break;
				
				case ABS_RZ:
				case ABS_RUDDER:
					m_State.Axes[JoystickAxis_e::R] = value;
					break;
				
				case ABS_RX:
					m_State.Axes[JoystickAxis_e::U] = value;
					break;
				
				case ABS_RY:
					m_State.Axes[JoystickAxis_e::V] = value;
					break;
				
				case ABS_HAT0X:
					m_State.Axes[JoystickAxis_e::PovX] = value;
					break;
				
				case ABS_HAT0Y:
					m_State.Axes[JoystickAxis_e::PovY] = value;
					break;
				
				default:
					break;
				}
			}
			break;

		case JS_EVENT_BUTTON:
			if (joyState.number < JoystickButtonCount)
			{
				m_State.Buttons[joyState.number] = (joyState.value != 0);
			}
			break;
		}
		result = read(m_File, &joyState, sizeof(joyState));
	}

	m_State.Connected = (!result || (errno == EAGAIN));

	return m_State;
}

#endif
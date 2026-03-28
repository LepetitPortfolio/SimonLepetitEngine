#pragma once
#include "../PlatformConfig.h"
#include "InputsGlobal.h"
#include "../../Common/Vector.h"

enum class SensorType_e : InputID
{
	Unknown = -1,
	Accelerometer = 0,
	Gyroscope,
	Magnetometer,
	Gravity,
	UserAcceleration,
	Orientation
};

static constexpr unsigned int SensorTypeCount = static_cast<unsigned int>(SensorType_e::Orientation) + 1;

#if PLATFORM_WINDOWS
#include "win/SensorWin32_cls.h"
using Sensor_cls = SensorWin32_cls; 
#elif PLATFORM_LINUX
#include "Unix/SensorUnix_cls.h"
using Sensor_cls = SensorUnix_cls; 
#endif
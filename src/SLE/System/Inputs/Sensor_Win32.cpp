#include "Sensor_cls.h"
#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS

void Sensor_cls::Initialize(){}

void Sensor_cls::Cleanup(){}

bool Sensor_cls::IsAvailable(SensorType_e _SensorType)
{
	return false;
}

bool Sensor_cls::Open(SensorType_e _SensorType)
{
	return false;
}

void Sensor_cls::Close(){}

Vector3f Sensor_cls::Update()
{
	return {};
}

void Sensor_cls::SetEnabled(bool _Enabled){}

#endif
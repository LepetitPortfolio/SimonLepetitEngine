#include "Sensor.h"
#include "../Platform.h"

#if PLATFORM_LINUX

void Sensor::Initialize() {}

void Sensor::Cleanup() {}

bool Sensor::IsAvailable(SensorType _SensorType)
{
	return false;
}

bool Sensor::Open(SensorType _SensorType)
{
	return false;
}

void Sensor::Close() {}

Vector3f Sensor::Update()
{
	return {};
}

void Sensor::SetEnabled(bool _Enabled) {}

#endif
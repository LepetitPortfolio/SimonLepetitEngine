#include "../../PlatformConfig.h"

#if PLATFORM_LINUX
#include "SensorUnix_cls.h"

void SensorUnix_cls::Initialize() {}

void SensorUnix_cls::Cleanup() {}

bool SensorUnix_cls::IsAvailable(SensorType_e _SensorType)
{
	return false;
}

bool SensorUnix_cls::Open(SensorType_e _SensorType)
{
	return false;
}

void SensorUnix_cls::Close() {}

Vector3f SensorUnix_cls::Update()
{
	return {};
}

void SensorUnix_cls::SetEnabled(bool _Enabled) {}

#endif
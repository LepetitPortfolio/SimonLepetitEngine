#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS
#include "SensorWin32_cls.h"

void SensorWin32_cls::Initialize(){}

void SensorWin32_cls::Cleanup(){}

bool SensorWin32_cls::IsAvailable(SensorType_e _SensorType)
{
	return false;
}

bool SensorWin32_cls::Open(SensorType_e _SensorType)
{
	return false;
}

void SensorWin32_cls::Close(){}

Vector3f SensorWin32_cls::Update()
{
	return {};
}

void SensorWin32_cls::SetEnabled(bool _Enabled){}

#endif
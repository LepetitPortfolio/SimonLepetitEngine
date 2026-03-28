#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS
#include "../Sensor_cls.h"

class SensorWin32_cls
{
public:
	
	static void Initialize();

	static void Cleanup();

	static bool IsAvailable(SensorType_e _SensorType);

	bool Open(SensorType_e _SensorType);

	void Close();

	Vector3f Update();

	void SetEnabled(bool _Enabled);

};

#endif
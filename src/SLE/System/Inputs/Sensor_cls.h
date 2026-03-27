#pragma once
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

class Sensor_cls
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
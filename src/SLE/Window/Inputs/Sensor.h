#pragma once
#include "InputsGlobal.h"
#include "../Utils/Vector.h"

enum class SensorType : InputID
{
	Unknown = -1,
	Accelerometer = 0,
	Gyroscope,
	Magnetometer,
	Gravity,
	UserAcceleration,
	Orientation
};

static constexpr unsigned int SensorTypeCount = static_cast<unsigned int>(SensorType::Orientation) + 1;

class Sensor
{
public:
	
	static void Initialize();

	static void Cleanup();

	static bool IsAvailable(SensorType _SensorType);

	bool Open(SensorType _SensorType);

	void Close();

	Vector3f Update();

	void SetEnabled(bool _Enabled);

};
#pragma once
#include "Sensor_cls.h"

#include "../../Common/EnumArray.h"

class SensorManager_cls
{
public:

	SensorManager_cls(const SensorManager_cls& _SensorManager) = delete;

	SensorManager_cls& operator=(const SensorManager_cls& _SensorManager) = delete;

	static SensorManager_cls& GetInstance();

	bool IsAvaileble(SensorType_e _SensorType);

	void SetEnabled(SensorType_e _SensorType, bool _Enabled);

	bool IsEnabled(SensorType_e _SensorType);

	Vector3f GetValue(SensorType_e _SensorType) const;

	void Updatde();

private:

	SensorManager_cls();

	~SensorManager_cls();

	struct Item
	{
		bool Available{};
		bool Enabled{};
		Sensor_cls Sensor_cls{};
		Vector3f Value;
	};

	EnumArray<SensorType_e, Item, SensorTypeCount> m_Sensors;

};
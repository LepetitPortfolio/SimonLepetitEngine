#pragma once
#include "Sensor.h"

#include "../Utils/EnumArray.h"

class SensorManager
{
public:

	SensorManager(const SensorManager& _SensorManager) = delete;

	SensorManager& operator=(const SensorManager& _SensorManager) = delete;

	static SensorManager& GetInstance();

	bool IsAvaileble(SensorType _SensorType);

	void SetEnabled(SensorType _SensorType, bool _Enabled);

	bool IsEnabled(SensorType _SensorType);

	Vector3f GetValue(SensorType _SensorType) const;

	void Updatde();

private:

	SensorManager();

	~SensorManager();

	struct Item
	{
		bool Available{};
		bool Enabled{};
		Sensor Sensor{};
		Vector3f Value;
	};

	EnumArray<SensorType, Item, SensorTypeCount> m_Sensors;

};
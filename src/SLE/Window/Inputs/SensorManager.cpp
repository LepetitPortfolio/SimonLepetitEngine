#include "SensorManager.h"

#include "../Utils/Error.h"

SensorManager& SensorManager::GetInstance()
{
    static SensorManager instance;
    return instance;
}

bool SensorManager::IsAvaileble(SensorType _SensorType)
{
    return m_Sensors[_SensorType].Available;
}

void SensorManager::SetEnabled(SensorType _SensorType, bool _Enabled)
{
    if (m_Sensors[_SensorType].Available)
    {
        m_Sensors[_SensorType].Enabled = _Enabled;
        m_Sensors[_SensorType].Sensor.SetEnabled(_Enabled);
    }
    else
    {
        Err() << "Warning: trying to enable a sensor that is not available (call Sensor::isAvailable to check it)" << std::endl;
    }
}

bool SensorManager::IsEnabled(SensorType _SensorType)
{
    return m_Sensors[_SensorType].Enabled;
}

Vector3f SensorManager::GetValue(SensorType _SensorType) const
{
    return m_Sensors[_SensorType].Value;
}

void SensorManager::Updatde()
{
    for (Item& item = m_Sensors)
    {
        if (item.Available)
        {
            item.Value = item.Sensor.Update();
        }
    }
}

SensorManager::SensorManager()
{
    Sensor::Initialize();

    for (unsigned int  sensorIndex = 0; sensorIndex < SensorTypeCount; sensorIndex++)
    {
        const auto sensorType = static_cast<SensorType>(sensorIndex);
        Item& item = m_Sensors[sensorType];

        item.Available = Sensor::IsAvailable(sensorType);

        if (item.Available)
        {
            if (item.Sensor.Open(sensorType))
            {
                item.Sensor.SetEnabled(false);
            }
            else
            {
                item.Available = false;
                Err() << "Warning: sensor " << sensorIndex << " failed to open, will not be available" << std::endl;
            }
        }
    }
}

SensorManager::~SensorManager()
{
    for (Item& item : m_Sensors)
    {
        if (item.Available)
        {
            item.Sensor.Close();
        }
    }

    Sensor::Cleanup();
}

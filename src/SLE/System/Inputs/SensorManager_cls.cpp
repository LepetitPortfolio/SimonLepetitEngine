#include "SensorManager_cls.h"

#include "../../Common/Error.h"

SensorManager_cls& SensorManager_cls::GetInstance()
{
    static SensorManager_cls instance;
    return instance;
}

bool SensorManager_cls::IsAvaileble(SensorType_e _SensorType)
{
    return m_Sensors[_SensorType].Available;
}

void SensorManager_cls::SetEnabled(SensorType_e _SensorType, bool _Enabled)
{
    if (m_Sensors[_SensorType].Available)
    {
        m_Sensors[_SensorType].Enabled = _Enabled;
        m_Sensors[_SensorType].Sensor_cls.SetEnabled(_Enabled);
    }
    else
    {
        Err() << "Warning: trying to enable a sensor that is not available (call Sensor_cls::isAvailable to check it)" << std::endl;
    }
}

bool SensorManager_cls::IsEnabled(SensorType_e _SensorType)
{
    return m_Sensors[_SensorType].Enabled;
}

Vector3f SensorManager_cls::GetValue(SensorType_e _SensorType) const
{
    return m_Sensors[_SensorType].Value;
}

void SensorManager_cls::Updatde()
{
    for (Item& item : m_Sensors)
    {
        if (item.Available)
        {
            item.Value = item.Sensor_cls.Update();
        }
    }
}

SensorManager_cls::SensorManager_cls()
{
    Sensor_cls::Initialize();

    for (unsigned int  sensorIndex = 0; sensorIndex < SensorTypeCount; sensorIndex++)
    {
        const auto sensorType = static_cast<SensorType_e>(sensorIndex);
        Item& item = m_Sensors[sensorType];

        item.Available = Sensor_cls::IsAvailable(sensorType);

        if (item.Available)
        {
            if (item.Sensor_cls.Open(sensorType))
            {
                item.Sensor_cls.SetEnabled(false);
            }
            else
            {
                item.Available = false;
                Err() << "Warning: sensor " << sensorIndex << " failed to open, will not be available" << std::endl;
            }
        }
    }
}

SensorManager_cls::~SensorManager_cls()
{
    for (Item& item : m_Sensors)
    {
        if (item.Available)
        {
            item.Sensor_cls.Close();
        }
    }

    Sensor_cls::Cleanup();
}

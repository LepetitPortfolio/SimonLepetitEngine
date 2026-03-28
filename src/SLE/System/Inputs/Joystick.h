
#include "InputsGlobal.h"
#include "../../Common/String.h"

enum class JoystickAxis_e : InputID
{
	Unknown = -1,
	X = 0,
	Y,
	Z,
	R,
	U,
	V,
	PovX,
	PovY
};

static constexpr unsigned int JoystickCount = 8;
static constexpr unsigned int JoystickAxisCount = static_cast<unsigned int>(JoystickAxis_e::PovY) + 1;
static constexpr unsigned int JoystickButtonCount = 32;

struct JoystickIdentification_e
{
	String Name;
	unsigned int VendorID;
	unsigned int ProductID;
};

struct Joystick
{

public:

    bool IsConnected(unsigned int _JoystickIndex);
    
    unsigned int GetButtonCount(unsigned int _JoystickIndex) const;

    bool HasAxis(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const;

    bool IsButtonPressed(unsigned int _JoystickIndex, unsigned int _Button) const;

    float GetAxisPosition(unsigned int _JoystickIndex, JoystickAxis_e _Axis) const;

    void Update();

};
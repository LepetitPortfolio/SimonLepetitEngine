#include "../Platform.h"
#include "Inputs.h"

#if PLATFORM_LINUX

bool Inputs::KeyPressed(Keyboard _Key)
{
	const KeyCode keycode = KeyToKeyCode(_Key);
	return isKeyPressedImpl(keycode);
}

bool Inputs::KeyPressed(ScanCode _ScaneCode)
{

}

KeyCode Inputs::KeyToKeyCode(Keyboard _Key)
{
    const KeySym keySym = KeyToKeySym(_Key);

    if (keySym != NoSymbol)
    {
        const auto    display = OpenDisplay();
        const KeyCode keyCode = XKeysymToKeycode(display.get(), keySym);

        if (keyCode != nullKeyCode)
        {
            return keyCode;
        }
    }

    if (key == Keyboard::RAlt)
    {
        return scancodeToKeycode[ScanCode::RAlt];
    }

    return nullKeyCode;
}

#endif
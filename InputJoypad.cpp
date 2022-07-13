#include "InputJoypad.h"

void InputJoypad::Update()
{
    XInputGetState(0, &state);
}

bool InputJoypad::GetButton(WORD button)
{
    return (state.Gamepad.wButtons & button);
}
#include "InputJoypad.h"

void InputJoypad::SetDeadZone(short& sThumb, const short& deaadzone)
{
    if ((sThumb < deaadzone) && sThumb > -deaadzone) {
        sThumb = 0;
    }
}

InputJoypad::~InputJoypad()
{
    //XInputEnable(false);
}

void InputJoypad::Update()
{
	prevState = state;

    XInputGetState(0, &state);

    SetDeadZone(state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    SetDeadZone(state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
    SetDeadZone(state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    SetDeadZone(state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

bool InputJoypad::GetButton(WORD button)
{
    return (state.Gamepad.wButtons & button);
}

bool InputJoypad::GetLTrigger()
{
    return state.Gamepad.bLeftTrigger != 0;
}

bool InputJoypad::GetRTrigger()
{
    return state.Gamepad.bRightTrigger != 0;
}

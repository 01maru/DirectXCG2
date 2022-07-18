#pragma once
#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")

class InputJoypad
{
private:
    XINPUT_STATE state;
    XINPUT_STATE prevState;
private:
    void SetDeadZone(short& sThumb, const short& deaadzone);
public:
    ~InputJoypad();
    void Update();
    bool GetButton(WORD button);
    bool GetLTrigger();
    bool GetRTrigger();
};

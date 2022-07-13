#pragma once
#include <Windows.h>
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")

class InputJoypad
{
private:
    XINPUT_STATE state;
public:
    void Update();
    bool GetButton(WORD button);
};

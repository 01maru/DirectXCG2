#pragma once
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
class Input
{
private:
	BYTE key[256] = {};
	BYTE prev[256] = {};
	IDirectInputDevice8* keyboard = nullptr;
public:
	Input(const HWND& hwnd, const WNDCLASSEX& w);
	void Update();

	bool GetKey(int _key);
	bool GetTrigger(int _key);
	bool ReleaseKey(int _key);
};


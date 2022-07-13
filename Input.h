#pragma once
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

static enum MouseButton {
	LeftClick = 0,
	RightClick,
	WheelClick,
};

class Input
{
private:
	HWND inputHwnd;

	BYTE key[256] = {};
	BYTE prev[256] = {};
	IDirectInputDevice8* keyboard = nullptr;
	IDirectInput8* directInput = nullptr;

	DIMOUSESTATE click = {};
	DIMOUSESTATE prevclick = {};
	IDirectInputDevice8* mouse = nullptr;
	POINT cursor;
public:
	Input(const HWND& hwnd, const WNDCLASSEX& w);
	~Input();
	void Update();

	bool GetKey(int _key);
	bool GetTrigger(int _key);
	bool ReleaseKey(int _key);

	bool Click(int type);
	bool ClickTrriger(int type);

	POINT CursorPos();
};


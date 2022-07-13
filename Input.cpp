#include "Input.h"

Input::Input(const HWND& hwnd, const WNDCLASSEX& w)
{
	//	DirectInput初期化
	HRESULT result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//	デバイス生成(キーボード以外も可能)
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//	入力形成のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//	排他制御のレベルセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//	デバイス生成(キーボード以外も可能)
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	//	入力形成のセット
	result = mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	//	排他制御のレベルセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

Input::~Input()
{
	keyboard->Unacquire();
	keyboard->Release();
	mouse->Unacquire();
	mouse->Release();
	directInput->Release();
}

void Input::Update()
{
	//	前フレームの情報取得
	for (size_t i = 0; i < sizeof(key); i++)
	{
		prev[i] = key[i];
	}

	prevclick = click;

	//	キー情報取得
	keyboard->Acquire();
	//	全キーの入力情報取得
	keyboard->GetDeviceState(sizeof(key), key);

	mouse->Acquire();
	mouse->GetDeviceState(sizeof(DIMOUSESTATE), &click);
	GetCursorPos(&cursor);
}

bool Input::GetKey(int _key)
{
	return key[_key];
}

bool Input::GetTrigger(int _key)
{
	return key[_key] && !prev[_key];
}

bool Input::ReleaseKey(int _key)
{
	return prev[_key] && !key[_key];
}

bool Input::Click(int type)
{
	return (click.rgbButtons[type] & (0x80));
}

bool Input::ClickTrriger(int type)
{
	return (click.rgbButtons[type] & (0x80)) && !(prevclick.rgbButtons[type] & (0x80));
}

POINT Input::CursorPos(HWND hwnd)
{
	ScreenToClient(hwnd, &cursor);
	return cursor;
}

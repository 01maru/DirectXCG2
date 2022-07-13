#include "Input.h"

Input::Input(const HWND& hwnd, const WNDCLASSEX& w)
{
	//	DirectInput������
	HRESULT result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//	�f�o�C�X����(�L�[�{�[�h�ȊO���\)
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//	���͌`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//	�r������̃��x���Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//	�f�o�C�X����(�L�[�{�[�h�ȊO���\)
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
	//	���͌`���̃Z�b�g
	result = mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	//	�r������̃��x���Z�b�g
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
	//	�O�t���[���̏��擾
	for (size_t i = 0; i < sizeof(key); i++)
	{
		prev[i] = key[i];
	}

	prevclick = click;

	//	�L�[���擾
	keyboard->Acquire();
	//	�S�L�[�̓��͏��擾
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

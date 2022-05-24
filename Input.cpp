#include "Input.h"

Input::Input(const HWND& hwnd, const WNDCLASSEX& w)
{
	//	DirectInput������
	IDirectInput8* directInput = nullptr;
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
}

void Input::Update()
{
	//	�O�t���[���̏��擾
	for (size_t i = 0; i < sizeof(key); i++)
	{
		prev[i] = key[i];
	}
	//	�L�[���擾
	keyboard->Acquire();
	//	�S�L�[�̓��͏��擾
	keyboard->GetDeviceState(sizeof(key), key);
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

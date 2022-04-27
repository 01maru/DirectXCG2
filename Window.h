#pragma once
#include <Windows.h>

const int window_width = 1280;
const int window_height = 720;
class Window
{
public:
	//	�N���X�ݒ�
	WNDCLASSEX w{};
	HWND hwnd;
	MSG msg{};

public:
	//	�E�B���h�E�v���V�[�W��
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Window();
	void MessageUpdate();
	bool EndLoop();

	//	�Q�b�^�[
	//void GetWNDCLASSEX(WNDCLASSEX* wc);
};


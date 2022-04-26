#pragma once
#include <Windows.h>

const int window_width = 1280;
const int window_height = 720;
class Window
{
public:
	//	クラス設定
	WNDCLASSEX w{};
	HWND hwnd;
	MSG msg{};
public:

	Window(const WNDPROC& WindowProc);
	void MessageUpdate();
	bool EndLoop();

	//	ゲッター
	//void GetWNDCLASSEX(WNDCLASSEX* wc);
};


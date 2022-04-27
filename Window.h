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
	//	ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Window();
	void MessageUpdate();
	bool EndLoop();

	//	ゲッター
	//void GetWNDCLASSEX(WNDCLASSEX* wc);
};


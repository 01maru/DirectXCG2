#pragma once
#include <Windows.h>

// ウィンドウサイズ
const int window_width = 1280; // 横幅
const int window_height = 720; // 縦幅
class Window
{
public:
	// ウィンドウクラスの設定
	WNDCLASSEX w{};
	// ウィンドウオブジェクトの生成
	HWND hwnd;
	// メッセージ
	MSG msg{};
public:
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Window();
	~Window();
	void MsgUpdate();
	bool EndLoop();
};


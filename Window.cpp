#include "Window.h"

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:		//	ウィンドウが閉じられた
	case WM_DESTROY:	//	ウィンドウが破棄された
		PostQuitMessage(0);
		return 0;
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

Window::Window()
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = L"DirectXGame";
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	//	サイズ調節
	RegisterClassEx(&w);
	RECT wrc = { 0,0,window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//	生成
	hwnd = CreateWindow(w.lpszClassName,
		L"CG1_test",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr);

	ShowWindow(hwnd, SW_SHOW);
}

Window::~Window()
{
	//	ウィンドウクラス登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

void Window::MessageUpdate()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool Window::EndLoop()
{
	if (msg.message == WM_QUIT) return true;
	return false;
}

#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class EnumAdapter
{
private:
	IDXGIAdapter4* tmpAdapter = nullptr;	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIFactory7* dxgiFactory = nullptr;

public:
	EnumAdapter();

	//	ゲッター
	IDXGIAdapter4* TmpAdapter();
	IDXGIFactory7* DxgiFactory();
};


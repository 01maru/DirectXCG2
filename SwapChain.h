#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class SwapChain
{
private:
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

public:
	SwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue, HWND hwnd);

	IDXGISwapChain4* Chain();
	DXGI_SWAP_CHAIN_DESC1 Desc();
};


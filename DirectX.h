#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class MyDirectX
{
public:
	HRESULT result;

	IDXGIFactory7* dxgiFactory = nullptr;
	ID3D12Device* device = nullptr;

	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;

	ID3D12CommandQueue* cmdQueue = nullptr;

	IDXGISwapChain4* swapChain = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ID3D12DescriptorHeap* rtvHeap = nullptr;

	// バックバッファ
	std::vector<ID3D12Resource*> backBuffers;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	D3D12_RESOURCE_BARRIER barrierDesc{};

	FLOAT clearColor[4] = {};

	ID3D12DescriptorHeap* dsvHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
public:
	MyDirectX(HWND hwnd);

	void DrawAble(FLOAT* clearColor = nullptr);
	void DrawEnd();

	void ScreenClear(FLOAT* clearColor);
	void ScreenClear();
};


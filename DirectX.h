#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class MyDirectX
{
private:
	HRESULT result;

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Device> device;

	ComPtr<ID3D12CommandQueue> cmdQueue;
public:


	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ID3D12DescriptorHeap* rtvHeap = nullptr;

	// バックバッファ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	D3D12_RESOURCE_BARRIER barrierDesc{};

	FLOAT clearColor[4] = {};

	ID3D12DescriptorHeap* dsvHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

private:
	void DebugLayer();
public:
	MyDirectX(HWND hwnd);

	void DrawAble(FLOAT* clearColor = nullptr);
	void DrawEnd();

	void ScreenClear(FLOAT* clearColor);
	void ScreenClear();

	//	Getter
	ID3D12Device* Dev() { return device.Get(); }
};


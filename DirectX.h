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

	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;

	ComPtr<ID3D12CommandQueue> cmdQueue;

	ComPtr<IDXGISwapChain4> swapChain;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> rtvHeap;

	// �o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;

	D3D12_RESOURCE_BARRIER barrierDesc{};

	FLOAT clearColor[4] = {};

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	ComPtr<ID3D12Resource> depthBuff;

	// screenTexture
	ComPtr<ID3D12Resource> screenResource;
	D3D12_RESOURCE_BARRIER screenBarrierDesc;
	ComPtr<ID3D12DescriptorHeap> screenRTVHeap;
	ComPtr<ID3D12DescriptorHeap> screenSRVHeap;
private:
	void DebugLayer();

	void ScreenClear(FLOAT* clearColor, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
	void ScreenClear(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
	
	void SetResourceBarrier(D3D12_RESOURCE_BARRIER& desc, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter, ID3D12Resource* pResource = nullptr);
	void CmdListDrawAble(D3D12_RESOURCE_BARRIER& barrierDesc, ID3D12Resource* pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, FLOAT* clearColor = nullptr);
public:
	MyDirectX(HWND hwnd);
	void DrawAbleScreenTexture(FLOAT* clearColor = nullptr);
	void DrawEndScreenTexture();
	void DrawAble(FLOAT* clearColor = nullptr);
	void DrawEnd();
	void Setting();

	//	Getter
	ID3D12Device* Dev() { return device.Get(); }
	ID3D12GraphicsCommandList* CmdList() { return cmdList.Get(); }
};


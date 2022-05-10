#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class DesHeap
{
private:
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

public:
	DesHeap(DXGI_SWAP_CHAIN_DESC1 swapChainDesc, ID3D12Device* dev);

	ID3D12DescriptorHeap* RtvHeap();
	D3D12_DESCRIPTOR_HEAP_DESC Desc();
};


#pragma once
#include <vector>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "SwapChain.h"
#include "DesHeap.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Buffer
{
public:
	std::vector<ID3D12Resource*> backBuffers;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

public:
	Buffer(SwapChain swapChain, DesHeap desHeap, ID3D12Device* dev);
};


#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class CmdList
{
private:
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

public:
	CmdList(ID3D12Device* dev);

	ID3D12CommandAllocator* Allocator();
	ID3D12GraphicsCommandList* List();
};


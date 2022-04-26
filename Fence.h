#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Fence
{
public:
	ID3D12Fence* fnc = nullptr;
	UINT64 fenceVal = 0;

public:
	Fence(ID3D12Device* dev);
};


#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Fence
{
public:
	UINT64 fenceVal = 0;
private:
	ID3D12Fence* fnc = nullptr;

public:
	Fence(ID3D12Device* dev);

	ID3D12Fence* Fnc();
};


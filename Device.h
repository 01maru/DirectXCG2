#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Device
{
public:
	ID3D12Device* dev = nullptr;

public:
	Device(IDXGIAdapter4* tmpAdapter);
};


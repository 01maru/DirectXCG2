#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class ConstBuffer
{
public:
	struct ConstBufferDataMaterial {
		XMFLOAT4 color;	//	RGBA
	};

	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};
	ID3D12Resource* material = nullptr;
	ConstBufferDataMaterial* mapMaterial = nullptr;

public:
	ConstBuffer(ID3D12Device* dev);
};


#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")

class ConstBuff
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
	ConstBuff(ID3D12Device* dev);
	void Update(ID3D12GraphicsCommandList* cmdList);
};


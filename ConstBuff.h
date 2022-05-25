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

	struct ConstBufferDataTransform {
		XMMATRIX mat;
	};

	ID3D12Resource* transform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	D3D12_RESOURCE_DESC cbResourceDesc{};

	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};
	ID3D12Resource* material = nullptr;
	ConstBufferDataMaterial* mapMaterial = nullptr;

public:
	ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight);
	void Update(ID3D12GraphicsCommandList* cmdList);
	void CBUpdate(ID3D12GraphicsCommandList* cmdList);
};


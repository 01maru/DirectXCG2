#pragma once
#include "Vector3D.h"
#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")

class Object3D
{
public:
	XMMATRIX matWorld;

	XMMATRIX matScale;
	Vector3D scale;

	XMMATRIX matRot;
	Vector3D rotAngle;

	XMMATRIX matTrans;
	Vector3D trans;

	struct ConstBufferDataTransform {
		XMMATRIX mat;
	};
	ID3D12Resource* transform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	D3D12_RESOURCE_DESC cbResourceDesc{};
	Object3D* parent = nullptr;
public:
	Object3D(ID3D12Device* dev);
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	void Draw(ID3D12GraphicsCommandList* cmdList);
};


#pragma once
#include "Vector3D.h"

class Object3D
{
public:
	XMMATRIX matWorld;

	XMMATRIX matScale;
	XMFLOAT3 scale;

	XMMATRIX matRot;
	XMFLOAT3 rotAngle;

	XMMATRIX matTrans;
	XMFLOAT3 trans;

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


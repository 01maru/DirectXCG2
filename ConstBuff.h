#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
const float PI = 3.14159265358979f;
class ConstBuff
{
public:
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};


	struct ConstBufferDataMaterial {
		XMFLOAT4 color;	//	RGBA
	};
	ID3D12Resource* material = nullptr;
	ConstBufferDataMaterial* mapMaterial = nullptr;

	struct ConstBufferDataTransform {
		XMMATRIX mat;
	};
	XMMATRIX matWorld;
	XMMATRIX matScale;
	XMMATRIX matRot;
	XMMATRIX matTrans;
	XMMATRIX matView;
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),			//	�㉺��p45�x
		(float)1280 / 720,		//	�A�X�y�N�g��
		0.1f, 1000.0f						//	�O�[�A���[
	);

	ID3D12Resource* transform = nullptr;
	ConstBufferDataTransform* constMapTransform = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	D3D12_RESOURCE_DESC cbResourceDesc{};


public:
	ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight);
	void Update(ID3D12GraphicsCommandList* cmdList);
	void CBUpdate(ID3D12GraphicsCommandList* cmdList);
	void Move(XMMATRIX matTrans, float rot);
};


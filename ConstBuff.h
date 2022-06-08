#pragma once
#include <d3d12.h>
#include "Matrix.h"
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
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


	Matrix matView;
	Matrix matProjection;
public:
	ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight);
	void Update(ID3D12GraphicsCommandList* cmdList);

	void LookAtLH();
	void PerspectiveFovLH(const int winwidth, const int winheight);
	void CalcFovXSinCos(float* pSin, float* pCos, float fovY);
};


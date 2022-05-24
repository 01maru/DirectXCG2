#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class VertBuff
{
public:
	D3D12_VERTEX_BUFFER_VIEW vbView{};
public:
	VertBuff(UINT sizeVB, XMFLOAT3* vertices, UINT vertSize, ID3D12Device* dev);
	void Update(ID3D12GraphicsCommandList* cmdList);
};

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
	HRESULT result;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_RESOURCE_DESC resDesc{};
	D3D12_HEAP_PROPERTIES heapProp{}; // ÉqÅ[Évê›íË
public:
	VertBuff(UINT sizeVB, XMFLOAT3* vertices, UINT vertSize, UINT sizeIB, uint16_t* indices, UINT indicesSize, ID3D12Device* dev);
	void Update(ID3D12GraphicsCommandList* cmdList);
private:
	void SetResDesc(UINT size);
	void BuffTransferGPU(ID3D12Resource* buff, ID3D12Device* dev);
};

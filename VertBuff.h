#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <stdint.h>
#include "MyMath.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

struct Vertex
{
	Vector3D pos;
	Vector3D normal;
	Vector2D uv;
};

class VertBuff
{
private:
	HRESULT result;

	bool ibExist = false;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_RESOURCE_DESC resDesc{};
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�

private:
	void SetResDesc(UINT size);
	void BuffTransferGPU(ID3D12Resource* buff, ID3D12Device* dev);
public:
	VertBuff(ID3D12Device* dev, UINT sizeVB, Vertex* vertices, UINT vertSize, UINT sizeIB = NULL, uint16_t* indices = nullptr, UINT indicesSize = NULL);
	~VertBuff();
	void Update(ID3D12GraphicsCommandList* cmdList);

	//	Getter
	D3D12_RESOURCE_DESC ResDesc() { return resDesc; }
};


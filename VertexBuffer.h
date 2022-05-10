#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class VertexBuffer
{
public:
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	D3D12_RESOURCE_DESC resDesc{};
	ID3D12Resource* vertBuff = nullptr;
	Vertex* map = nullptr; 	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	D3D12_VERTEX_BUFFER_VIEW view{};	// 頂点バッファビュー

	//	インデックスバッファ
	ID3D12Resource* indexBuff = nullptr;
	uint16_t* indexMap = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView{};

public:
	VertexBuffer(UINT sizeVB, ID3D12Device* dev, Vertex* vertices, UINT size, UINT sizeIB, uint16_t* indices, UINT ibNum);
};


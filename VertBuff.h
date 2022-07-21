#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <stdint.h>
#include "MyMath.h"
#include <wrl.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

struct Vertex
{
	Vector3D pos;
	Vector3D normal;
	Vector2D uv;
};

struct VertexObj
{
	Vector3D pos;
	Vector3D normal;
	Vector2D uv;
	Vector4D color;
};

struct ScreenVertex
{
	Vector3D pos;
	Vector2D uv;
};

class VertBuff
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	HRESULT result;
public:
	bool ibExist = false;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_RESOURCE_DESC resDesc{};
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定

	// 頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> indexBuff;
private:
	void SetResDesc(UINT size);
	void BuffTransferGPU(ID3D12Resource* buff, ID3D12Device* dev);
public:
	VertBuff();
	VertBuff(ID3D12Device* dev, UINT sizeVB, Vertex* vertices, UINT vertSize, UINT sizeIB = NULL, uint16_t* indices = nullptr, UINT indicesSize = NULL);
	VertBuff(ID3D12Device* dev, UINT sizeVB, ScreenVertex* vertices, UINT vertSize);
	void Init(ID3D12Device* dev, UINT sizeVB, UINT vertSize, UINT sizeIB = NULL, uint16_t* indices = nullptr, UINT indicesSize = NULL);
	void Update(ID3D12GraphicsCommandList* cmdList);

	virtual void SetVertices();

	//	Getter
	D3D12_RESOURCE_DESC ResDesc() { return resDesc; }
};


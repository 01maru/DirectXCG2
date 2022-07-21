#include "VertBuff.h"
#include <wrl.h>
#include <vector>

VertBuff::VertBuff()
{
}

VertBuff::VertBuff(ID3D12Device* dev, UINT sizeVB, Vertex* vertices, UINT vertSize, UINT sizeIB, uint16_t* indices, UINT indicesSize)
{
#pragma region  lighting
	for (int i = 0; i < indicesSize / 3; i++)
	{
		unsigned short index0 = indices[i * 3];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		Vector4D p0(vertices[index0].pos, 0.0f);
		Vector4D p1(vertices[index1].pos, 0.0f);
		Vector4D p2(vertices[index2].pos, 0.0f);

		Vector4D v1 = p1 - p0;
		Vector4D v2 = p2 - p0;
		Vector4D normal = v1 - v2;

		normal.normalize();
		Vector3D normal3D(normal.x, normal.y, normal.z);
		vertices[index0].normal = normal3D;
		vertices[index1].normal = normal3D;
		vertices[index2].normal = normal3D;
	}
#pragma endregion
	Init(dev, sizeVB, vertSize, sizeIB, indices, indicesSize);
	//	GPUメモリの値書き換えよう
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < vertSize; i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);
}

void VertBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	// 頂点バッファビューの設定コマンド
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//	インデックスバッファビュー設定コマンド
	if (ibExist) {
		cmdList->IASetIndexBuffer(&ibView);
	}
}

void VertBuff::SetVertices()
{
}

VertBuff::VertBuff(ID3D12Device* dev, UINT sizeVB, ScreenVertex* vertices, UINT vertSize)
{
	Init(dev, sizeVB, vertSize);
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	//	GPUメモリの値書き換えよう
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	ScreenVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < vertSize; i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);
}

void VertBuff::Init(ID3D12Device* dev, UINT sizeVB, UINT vertSize, UINT sizeIB, uint16_t* indices, UINT indicesSize)
{
	//	ヒープの設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用(CPUからアクセスできる)

#pragma region VB
	// リソース設定
	SetResDesc(sizeVB);

	//	GPU側にメモリ確保
	result = dev->CreateCommittedResource(
		&heapProp,							// ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,							// リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// 頂点バッファビューの作成(GPUで利用するため)
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;

	SetVertices();
#pragma endregion

#pragma region IB
	if (indices != nullptr) {
		ibExist = true;

		SetResDesc(sizeIB);
		result = dev->CreateCommittedResource(
			&heapProp, // ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&resDesc, // リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuff));
		assert(SUCCEEDED(result));
		//	インデックスバッファマッピング
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		assert(SUCCEEDED(result));
		// 全頂点に対して
		for (int i = 0; i < indicesSize; i++) {
			indexMap[i] = indices[i]; // 座標をコピー
		}
		// 繋がりを解除
		indexBuff->Unmap(0, nullptr);
		//	インデックスバッファビュー作成
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeIB;
	}
#pragma endregion
}

void VertBuff::SetResDesc(UINT size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;		//	アンチエイリアシング用のパラメータ
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void VertBuff::BuffTransferGPU(ID3D12Resource* buff, ID3D12Device* dev)
{
#pragma region バッファの生成
	result = dev->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(result));
#pragma endregion
}

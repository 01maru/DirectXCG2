#include "Object2D.h"
#include "MyMath.h"

void Object2D::SetVertices()
{
	//	GPUメモリの値書き換えよう
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	HRESULT result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < vertexSize; i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);
}

Object2D::Object2D(ID3D12Device* dev, Shader shader, UINT vertexNum, float rad)
{
	vertexSize = vertexNum;
	if (vertexNum >= 3)vertexSize += 1;

	//	インデックスサイズ

	indexSize = vertexNum * 3;
	indices.resize(indexSize);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexSize);

	for (size_t i = 0; i < vertexNum; i++)
	{
		size_t n = i * 3;
		indices[n + 2] = vertexSize;
		indices[n] = i;
		if (i == vertexNum - 1) {
			indices[n + 1] = 0;
		}
		else {
			indices[n + 1] = i + 1;
		}
	}

	// 頂点データサイズ
	vertices.resize(vertexSize);

	float radian = MyMath::PI * 2 / (float)vertexNum;

	for (size_t i = 0; i < vertexNum; i++)
	{
		vertices[i].pos.x = rad * sin(radian * i);
		vertices[i].pos.y = rad * cos(radian * i);
		vertices[i].pos.z = 0;
	}
	vertices[vertexNum].pos.x = 0;
	vertices[vertexNum].pos.y = 0;
	vertices[vertexNum].pos.z = 0;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertexSize);
	Init(dev, sizeVB, vertexSize, sizeIB, &indices.front(), indexSize);
	SetVertices();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	法線ベクトル
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

	pipeline.Init(dev, shader, inputLayout, _countof(inputLayout), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
}

void Object2D::Draw(ID3D12GraphicsCommandList* cmdList)
{
	pipeline.Setting(cmdList);
	pipeline.Update(cmdList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Update(cmdList);
	cmdList->DrawIndexedInstanced(indexSize, 1, 0, 0, 0);
	cmdList->DrawInstanced(vertexSize, 1, 0, 0);
}

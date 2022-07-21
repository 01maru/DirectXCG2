#include "DrawSphere.h"

void SphereObj::SetVertices()
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

SphereObj::SphereObj(ID3D12Device* dev, Shader shader, const int u_size, const int v_size)
{
	// 頂点データサイズ
	vertexSize = u_size * (v_size + 1);
	vertices.resize(vertexSize);

	for (int v = 0; v <= v_size; v++)
	{
		for (int u = 0; u < u_size; u++)
		{
			float theta = MyMath::PI * v / v_size;
			float phi = MyMath::PI * 2 * u / u_size;
			Vector3D pos(sinf(theta) * cosf(phi) * 10,
				cosf(theta) * 10,
				sinf(theta) * sinf(phi) * 10);
			vertices[u_size * v + u].pos = pos;
		}
	}

	//	インデックスサイズ
	indexSize = 6 * v_size * u_size;
	indices.resize(indexSize);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexSize);

	int idx = 0;
	for (int v = 0; v < v_size; v++)
	{
		for (int u = 0; u < u_size; u++)
		{
			if (u == u_size - 1) {
				indices[idx] = u + v * u_size;
				idx++;
				indices[idx] = v * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;

				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = (v + 1) * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;
			}
			else {
				indices[idx] = u + v * u_size;
				idx++;
				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;

				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = u + 1 + (v + 1) * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;
			}
		}
	}

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertexSize);
	Init(dev, sizeVB, vertexSize, sizeIB, &indices.front(), indexSize);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	法線ベクトル
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

	pipeline.Init(dev, shader, inputLayout, _countof(inputLayout), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D12_FILL_MODE_WIREFRAME);
}

void SphereObj::Draw(ID3D12GraphicsCommandList* cmdList)
{
	pipeline.Setting(cmdList);
	pipeline.Update(cmdList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Update(cmdList);
	cmdList->DrawIndexedInstanced(indexSize, 1, 0, 0, 0);
}

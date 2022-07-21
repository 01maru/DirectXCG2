#pragma once
#include "GPipeline.h"
#include "VertBuff.h"
struct GritVertex
{
	Vector3D pos;
};
class DrawGrid :public VertBuff
{
private:
	GPipeline pipeline;
	//VertBuff vbuff;
	UINT vertexSize;
	std::vector<Vertex> vertices;
public:
	DrawGrid(ID3D12Device* dev, Shader shader, UINT g_size, UINT gridX, UINT gridZ);
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void SetVertices() override;
};


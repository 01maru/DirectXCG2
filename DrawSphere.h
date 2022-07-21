#pragma once
#include "GPipeline.h"
#include "VertBuff.h"
class SphereObj :public VertBuff
{
private:
	GPipeline pipeline;
	UINT vertexSize;
	std::vector<Vertex> vertices;
	UINT indexSize;
	std::vector<unsigned short> indices;
private:
	void SetVertices() override;
public:
	SphereObj(ID3D12Device* dev, Shader shader, const int u_size, const int v_size);
	void Draw(ID3D12GraphicsCommandList* cmdList);
};


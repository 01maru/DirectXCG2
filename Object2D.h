#pragma once
#include "GPipeline.h"
#include "VertBuff.h"
class Object2D:public VertBuff
{
private:
	GPipeline pipeline;
	UINT vertexSize;
	std::vector<Vertex> vertices;
	UINT indexSize;
	std::vector<unsigned short> indices;
private:
	void SetVertices();
public:
	Object2D(ID3D12Device* dev, Shader shader, UINT vertexNum, float rad = 0.5f);
	void Draw(ID3D12GraphicsCommandList* cmdList);
};


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
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	Matrix matWorld;

	Matrix matScale;
	Vector3D scale;

	Matrix matRot;
	Vector3D rotAngle;

	Matrix matTrans;
	Vector3D trans;

	struct ConstBufferDataTransform {
		Matrix mat;
	};
	ComPtr<ID3D12Resource> transform;
	ConstBufferDataTransform* constMapTransform = nullptr;
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	D3D12_RESOURCE_DESC cbResourceDesc{};

	GPipeline pipeline;
	UINT vertexSize;
	std::vector<Vertex> vertices;
private:
	void SetVertices() override;
	void SetMatScaling();
	void SetMatRotation();
	void SetMatTransform();

public:
	DrawGrid(ID3D12Device* dev, Shader shader, UINT g_size, UINT gridX, UINT gridZ);
	void MatUpdate(Matrix matView, Matrix matProjection);
	void Draw(ID3D12GraphicsCommandList* cmdList);
};


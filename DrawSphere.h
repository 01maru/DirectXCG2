#pragma once
#include "GPipeline.h"
#include "VertBuff.h"
#include "MyMath.h"
class SphereObj :public VertBuff
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
	UINT indexSize;
	std::vector<unsigned short> indices;
private:
	void SetVertices() override;
	void SetMatScaling();
	void SetMatRotation();
	void SetMatTransform();
public:
	SphereObj(ID3D12Device* dev, Shader shader, const int u_size, const int v_size);
	void MatUpdate(Matrix matView, Matrix matProjection);
	void Draw(ID3D12GraphicsCommandList* cmdList);
};


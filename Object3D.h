#pragma once
#include "MyMath.h"
#include <d3d12.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")

class Object3D
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
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
	Object3D* parent = nullptr;
public:
	Object3D(ID3D12Device* dev);
	void Update(Matrix matView, Matrix matProjection);
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT indeicesNum);

private:
	void SetMatScaling();
	void SetMatRotation();
	void SetMatTransform();
};


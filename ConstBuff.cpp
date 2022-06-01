#include "ConstBuff.h"
#include "Vector3D.h"

ConstBuff::ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight)
{	
	//	ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//	GPU転送用

	//	リソース設定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	resourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xFF) & ~0xFF;
	//	生成
	result = dev->CreateCommittedResource(
		&heapProp,	//	ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,	//	リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&material));
	assert(SUCCEEDED(result));

	//	定数バッファのマッピング
	result = material->Map(0, nullptr, (void**)&mapMaterial);	//	マッピング
	assert(SUCCEEDED(result));


	//	GPUのメモリにデータ転送
	mapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


#pragma region ViewMatrix
	PerspectiveFovLH(winwidth, winheight);
	LookAtLH();
#pragma endregion
}

void ConstBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	//	定数バッファビュー(CBV)の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(0, material->GetGPUVirtualAddress());
}

void ConstBuff::LookAtLH()
{
	Vector3D eye(0, 0, -100);	//	視点座標
	Vector3D target(0, 0, 0);	//	注視点座標
	Vector3D up(0, 1, 0);		//	上方向ベクトル

	Matrix mat2;
	mat2.m[0][3] = -eye.x;
	mat2.m[1][3] = -eye.y;
	mat2.m[2][3] = -eye.z;

	Vector3D baseX = up;
	Vector3D baseY;
	Vector3D baseZ = eye - target;

	baseZ.normalize();

	baseX.cross(baseZ);
	baseX.normalize();

	baseY = baseZ;
	baseY.cross(baseX);

	matView.m[0][0] = baseX.x;
	matView.m[0][1] = baseX.y;
	matView.m[0][2] = baseX.z;

	matView.m[1][0] = baseY.x;
	matView.m[1][1] = baseY.y;
	matView.m[1][2] = baseY.z;

	matView.m[2][0] = baseZ.x;
	matView.m[2][1] = baseZ.y;
	matView.m[2][2] = baseZ.z;
}

void ConstBuff::PerspectiveFovLH(const int winwidth, const int winheight)
{
	float FovAngleY = 45.0f;
	float AspectRatio = (float)winwidth / winheight;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
		

	float    SinFov;
	float    CosFov;
	XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
	
	float Width = CosFov / SinFov;
	float Height = Width * AspectRatio;
	float fRange = FarZ / (FarZ - NearZ);
	
	matProjection.m[0][0] = Width;
	matProjection.m[0][1] = 0.0f;
	matProjection.m[0][2] = 0.0f;
	matProjection.m[0][3] = 0.0f;
	
	matProjection.m[1][0] = 0.0f;
	matProjection.m[1][1] = Height;
	matProjection.m[1][2] = 0.0f;
	matProjection.m[1][3] = 0.0f;
	
	matProjection.m[2][0] = 0.0f;
	matProjection.m[2][1] = 0.0f;
	matProjection.m[2][2] = (FarZ + NearZ) / (FarZ - NearZ);
	matProjection.m[2][3] = 1.0f;
	
	matProjection.m[3][0] = 0.0f;
	matProjection.m[3][1] = 0.0f;
	matProjection.m[3][2] = -2*fRange * NearZ;
	matProjection.m[3][3] = 0.0f;
}

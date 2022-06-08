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
	Vector3D baseZ = target - eye;

	baseZ.normalize();

	baseX = baseX.cross(baseZ);
	baseX.normalize();

	baseY = baseZ;
	baseY = baseY.cross(baseX);

	matView.m[0][0] = baseX.x;
	matView.m[0][1] = baseX.y;
	matView.m[0][2] = baseX.z;

	matView.m[1][0] = baseY.x;
	matView.m[1][1] = baseY.y;
	matView.m[1][2] = baseY.z;

	matView.m[2][0] = baseZ.x;
	matView.m[2][1] = baseZ.y;
	matView.m[2][2] = baseZ.z;

	matView.m[3][0] = -baseX.dot(eye);
	matView.m[3][1] = -baseY.dot(eye);
	matView.m[3][2] = -baseZ.dot(eye);
}

void ConstBuff::PerspectiveFovLH(const int winwidth, const int winheight)
{
	//assert(NearZ > 0.f && FarZ > 0.f);
	//assert(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
	//assert(!XMScalarNearEqual(AspectRatio, 0.0f, 0.00001f));
	//assert(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

	float aspect = (float)winwidth / winheight;
	float fovY = XMConvertToRadians(45.0f);
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	//float tan = (float)sin(fovY/2.0f) / cos(fovY/2.0f) * aspect;
	float height = 1.0f / tanf(fovY / 2.0f);

	matProjection.Identity();
	matProjection.m[0][0] = height;
	matProjection.m[1][1] = height * aspect;
	matProjection.m[2][2] = (float)(farZ + nearZ) / (farZ - nearZ);
	matProjection.m[2][3] = 1.0f;
	matProjection.m[3][2] = -2.0f * farZ * nearZ / (float)(farZ - nearZ);
	matProjection.m[3][3] = 0.0f;

	//float FovAngleY = XMConvertToRadians(45.0f);
	//float AspectRatio = (float)winwidth / winheight;
	//float NearZ = 0.1f;
	//float FarZ = 1000.0f;


	//float    SinFov;
	//float    CosFov;
	//XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

	//float Height = CosFov / SinFov;
	//float Width = Height / AspectRatio;
	//float fRange = FarZ / (FarZ - NearZ);

	//matProjection.Identity();

	//matProjection.m[0][0] = Width;

	//matProjection.m[1][1] = Height;

	//matProjection.m[2][2] = fRange;
	//matProjection.m[2][3] = 1.0f;

	//matProjection.m[3][2] = -fRange * NearZ;
	//matProjection.m[3][3] = 0.0f;
}

void ConstBuff::CalcFovXSinCos(float* pSin, float* pCos, float fovY)
{
	assert(pSin);
	assert(pCos);


}

#include "Object3D.h"

Object3D::Object3D(ID3D12Device* dev)
{
	HRESULT result;

#pragma region  ConstBuffer
	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};
	//	ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//	GPU転送用

	//	リソース設定
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xFF) & ~0xFF;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//	生成
	result = dev->CreateCommittedResource(
		&cbHeapProp,	//	ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,	//	リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transform));
	assert(SUCCEEDED(result));

	//	定数バッファのマッピング
	result = transform->Map(0, nullptr, (void**)&constMapTransform);	//	マッピング
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region  WorldMatrix初期値
	scale = Vector3D(1.0f, 1.0f, 1.0f);
	rotAngle = Vector3D(0.0f, 0.0f, XMConvertToRadians(10.0f));
	trans = Vector3D(0.0f, 0.0f, 0.0f);
#pragma endregion

}


void Object3D::Update(Matrix matView, Matrix matProjection)
{
#pragma region WorldMatrix
	matWorld.Identity();
	
	//	スケーリング
	SetMatScaling();
	matWorld *= matScale;

	//	回転
	SetMatRotation();
	matWorld *= matRot;

	//	平行移動
	SetMatTransform();
	matWorld *= matTrans;
#pragma endregion

	constMapTransform->mat = matWorld;
	constMapTransform->mat *= matView;
	constMapTransform->mat *= matProjection;

	//constMapTransform->mat.m[0][0] = 1.33736396;
	//constMapTransform->mat.m[0][1] = 0.00000000;
	//constMapTransform->mat.m[0][2] = -0.173665538;
	//constMapTransform->mat.m[0][3] = -0.173648164;

	//constMapTransform->mat.m[1][0] = 0.00000000;
	//constMapTransform->mat.m[1][1] = 2.41421342;
	//constMapTransform->mat.m[1][2] = 0.00000000;
	//constMapTransform->mat.m[1][3] = 0.00000000;

	//constMapTransform->mat.m[2][0] = 0.235813349;
	//constMapTransform->mat.m[2][1] = 0.00000000;
	//constMapTransform->mat.m[2][2] = 0.984906256;
	//constMapTransform->mat.m[2][3] = 0.984807730;

	//constMapTransform->mat.m[3][0] = 0.00000000;
	//constMapTransform->mat.m[3][1] = 0.00000000;
	//constMapTransform->mat.m[3][2] = 99.9099884;
	//constMapTransform->mat.m[3][3] = 100.000000;
}

void Object3D::SetMatScaling()
{
	matScale.Identity();
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
}

void Object3D::SetMatRotation()
{
	matRot.Identity();
	Matrix matRotX;
	matRotX.m[1][1] = cos(rotAngle.x);
	matRotX.m[1][2] = sin(rotAngle.x);
	matRotX.m[2][1] = -sin(rotAngle.x);
	matRotX.m[2][2] = cos(rotAngle.x);
	Matrix matRotY;
	matRotY.m[0][0] = cos(rotAngle.y);
	matRotY.m[2][0] = sin(rotAngle.y);
	matRotY.m[0][2] = -sin(rotAngle.y);
	matRotY.m[2][2] = cos(rotAngle.y);
	Matrix matRotZ;
	matRotZ.m[0][0] = cos(rotAngle.z);
	matRotZ.m[0][1] = sin(rotAngle.z);
	matRotZ.m[1][0] = -sin(rotAngle.z);
	matRotZ.m[1][1] = cos(rotAngle.z);

	matRot = matRotZ;
	matRot *= matRotX;
	matRot *= matRotY;
}

void Object3D::SetMatTransform()
{
	matTrans.Identity();
	matTrans.m[3][0] = trans.x;
	matTrans.m[3][1] = trans.y;
	matTrans.m[3][2] = trans.z;
}

void Object3D::Draw(ID3D12GraphicsCommandList* cmdList, UINT indeicesNum)
{
	cmdList->SetGraphicsRootConstantBufferView(2, transform->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(indeicesNum, 1, 0, 0, 0);
}

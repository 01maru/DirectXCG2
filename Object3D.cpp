#include "Object3D.h"

Object3D::Object3D(ID3D12Device* dev)
{
	HRESULT result;

#pragma region  ConstBuffer
	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};
	//	�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//	GPU�]���p

	//	���\�[�X�ݒ�
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xFF) & ~0xFF;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//	����
	result = dev->CreateCommittedResource(
		&cbHeapProp,	//	�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,	//	���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transform));
	assert(SUCCEEDED(result));

	//	�萔�o�b�t�@�̃}�b�s���O
	result = transform->Map(0, nullptr, (void**)&constMapTransform);	//	�}�b�s���O
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region  WorldMatrix�����l
	scale = Vector3D(1.0f, 1.0f, 1.0f);
	rotAngle = Vector3D(0.0f, 0.0f, 0.0f);
	trans = Vector3D(0.0f, 0.0f, 0.0f);
#pragma endregion

}


void Object3D::Update(XMMATRIX matView, XMMATRIX matProjection)
{
	//	�P�ʍs����
	constMapTransform->mat = XMMatrixIdentity();

	constMapTransform->mat = XMMatrixOrthographicOffCenterLH(
		-1.0f, 1.0f,
		-1.0f, 1.0f,
		0.0f, 1.0f);

#pragma region WorldMatrix
	matWorld = XMMatrixIdentity();

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matWorld *= matScale;

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotAngle.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotAngle.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotAngle.y));
	matWorld *= matRot;

	matTrans = XMMatrixTranslation(trans.x, trans.y, trans.z);
	matWorld *= matTrans;
#pragma endregion

	constMapTransform->mat = matWorld * matView * matProjection;
}

void Object3D::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootConstantBufferView(2, transform->GetGPUVirtualAddress());
}

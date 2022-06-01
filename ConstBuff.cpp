#include "ConstBuff.h"
#include "Vector3D.h"

ConstBuff::ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight)
{	
	//	�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//	GPU�]���p

	//	���\�[�X�ݒ�
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	resourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xFF) & ~0xFF;
	//	����
	result = dev->CreateCommittedResource(
		&heapProp,	//	�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,	//	���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&material));
	assert(SUCCEEDED(result));

	//	�萔�o�b�t�@�̃}�b�s���O
	result = material->Map(0, nullptr, (void**)&mapMaterial);	//	�}�b�s���O
	assert(SUCCEEDED(result));


	//	GPU�̃������Ƀf�[�^�]��
	mapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


#pragma region ViewMatrix
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
#pragma endregion
}

void ConstBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	//	�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList->SetGraphicsRootConstantBufferView(0, material->GetGPUVirtualAddress());
}

void ConstBuff::LookAtLH()
{
	Vector3D eye(0, 0, -100);	//	���_���W
	Vector3D target(0, 0, 0);	//	�����_���W
	Vector3D up(0, 1, 0);		//	������x�N�g��

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

	mat.m[0][0] = baseX.x;
	mat.m[0][1] = baseX.y;
	mat.m[0][2] = baseX.z;

	mat.m[1][0] = baseY.x;
	mat.m[1][1] = baseY.y;
	mat.m[1][2] = baseY.z;

	mat.m[2][0] = baseZ.x;
	mat.m[2][1] = baseZ.y;
	mat.m[2][2] = baseZ.z;
}
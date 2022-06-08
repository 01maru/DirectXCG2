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
}

void ConstBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	//	�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList->SetGraphicsRootConstantBufferView(0, material->GetGPUVirtualAddress());
}

#include "VertBuff.h"
#include <wrl.h>

VertBuff::VertBuff(ID3D12Device* dev, UINT sizeVB, Vertex* vertices, UINT vertSize, UINT sizeIB, uint16_t* indices, UINT indicesSize)
{
	//	�q�[�v�̐ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p(CPU����A�N�Z�X�ł���)

#pragma region VB
	// ���\�[�X�ݒ�
	SetResDesc(sizeVB);

	// ���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;
	//	GPU���Ƀ������m��
	result = dev->CreateCommittedResource(
		&heapProp,							// �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,							// ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//	GPU�������̒l���������悤
	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < vertSize; i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);
	
	// ���_�o�b�t�@�r���[�̍쐬(GPU�ŗ��p���邽��)
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);
#pragma endregion

#pragma region IB
	if (indices != nullptr) {
		SetResDesc(sizeIB);
		ID3D12Resource* indexBuff = nullptr;
		result = dev->CreateCommittedResource(
			&heapProp, // �q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&resDesc, // ���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&indexBuff));
		assert(SUCCEEDED(result));
		//	�C���f�b�N�X�o�b�t�@�}�b�s���O
		uint16_t* indexMap = nullptr;
		result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		assert(SUCCEEDED(result));
		// �S���_�ɑ΂���
		for (int i = 0; i < indicesSize; i++) {
			indexMap[i] = indices[i]; // ���W���R�s�[
		}
		// �q���������
		indexBuff->Unmap(0, nullptr);
		//	�C���f�b�N�X�o�b�t�@�r���[�쐬
		ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibView.Format = DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes = sizeIB;
	}
#pragma endregion
}

VertBuff::~VertBuff()
{
}

void VertBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//	�C���f�b�N�X�o�b�t�@�r���[�ݒ�R�}���h
	cmdList->IASetIndexBuffer(&ibView);
}

void VertBuff::SetResDesc(UINT size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;		//	�A���`�G�C���A�V���O�p�̃p�����[�^
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void VertBuff::BuffTransferGPU(ID3D12Resource* buff, ID3D12Device* dev)
{
#pragma region �o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buff));
	assert(SUCCEEDED(result));
#pragma endregion
}

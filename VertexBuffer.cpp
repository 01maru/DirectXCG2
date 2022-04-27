#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(UINT sizeVB, ID3D12Device* dev, XMFLOAT3* vertices, UINT size)
{
	// ���_�o�b�t�@�̐ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

	// ���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���_�o�b�t�@�̐���
	HRESULT result = dev->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//	GPU�������̒l��������
	result = vertBuff->Map(0, nullptr, (void**)&map);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < size; i++) {
		map[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	// GPU���z�A�h���X
	view.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	view.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	view.StrideInBytes = sizeof(XMFLOAT3);
}

#include "Object2D.h"
#include "MyMath.h"

void Object2D::SetVertices()
{
	//	GPU�������̒l���������悤
	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	HRESULT result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < vertexSize; i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);
}

Object2D::Object2D(ID3D12Device* dev, Shader shader, UINT vertexNum, float rad)
{
	vertexSize = vertexNum;
	if (vertexNum >= 3)vertexSize += 1;

	//	�C���f�b�N�X�T�C�Y

	indexSize = vertexNum * 3;
	indices.resize(indexSize);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexSize);

	for (size_t i = 0; i < vertexNum; i++)
	{
		size_t n = i * 3;
		indices[n + 2] = vertexSize;
		indices[n] = i;
		if (i == vertexNum - 1) {
			indices[n + 1] = 0;
		}
		else {
			indices[n + 1] = i + 1;
		}
	}

	// ���_�f�[�^�T�C�Y
	vertices.resize(vertexSize);

	float radian = MyMath::PI * 2 / (float)vertexNum;

	for (size_t i = 0; i < vertexNum; i++)
	{
		vertices[i].pos.x = rad * sin(radian * i);
		vertices[i].pos.y = rad * cos(radian * i);
		vertices[i].pos.z = 0;
	}
	vertices[vertexNum].pos.x = 0;
	vertices[vertexNum].pos.y = 0;
	vertices[vertexNum].pos.z = 0;

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertexSize);
	Init(dev, sizeVB, vertexSize, sizeIB, &indices.front(), indexSize);
	SetVertices();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz���W
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	�@���x�N�g��
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv���W
	};

	pipeline.Init(dev, shader, inputLayout, _countof(inputLayout), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
}

void Object2D::Draw(ID3D12GraphicsCommandList* cmdList)
{
	pipeline.Setting(cmdList);
	pipeline.Update(cmdList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Update(cmdList);
	cmdList->DrawIndexedInstanced(indexSize, 1, 0, 0, 0);
	cmdList->DrawInstanced(vertexSize, 1, 0, 0);
}

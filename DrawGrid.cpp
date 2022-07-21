#include "DrawGrid.h"
#include <vector>

DrawGrid::DrawGrid(ID3D12Device* dev, Shader shader, UINT g_size, UINT gridX, UINT gridZ)
{
	float GridXStep = (float)g_size / gridX;
	float GridZStep = (float)g_size / gridZ;
	vertexSize = (gridX + gridZ) * 2;
	
	// ���_�f�[�^
	vertices.resize(vertexSize);

	for (size_t i = 0; i < vertexSize / 2; i++)
	{
		if (i % 2 == 0) {
			vertices[i].pos.z = g_size * 2;
			vertices[i].pos.x += GridXStep * i;

			vertices[i + gridX + gridZ].pos.x = g_size * 2;
			vertices[i+ gridX + gridZ].pos.z += GridZStep * i;
		}
		else {
			vertices[i].pos.x += GridXStep * (i - 1);
			vertices[i + gridX + gridZ].pos.z += GridZStep * (i - 1);
		}
	}

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertexSize);
	Init(dev, sizeVB, vertexSize);
	//SetVertices();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz���W
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	�@���x�N�g��
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv���W
	};

	pipeline.Init(dev, shader, inputLayout, _countof(inputLayout), D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}

void DrawGrid::Draw(ID3D12GraphicsCommandList* cmdList)
{
	pipeline.Setting(cmdList);
	pipeline.Update(cmdList, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	Update(cmdList);
	cmdList->DrawInstanced(vertexSize, 1, 0, 0);
}

void DrawGrid::SetVertices()
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
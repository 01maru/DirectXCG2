#include "DrawSphere.h"

void SphereObj::SetVertices()
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

SphereObj::SphereObj(ID3D12Device* dev, Shader shader, const int u_size, const int v_size)
{
	// ���_�f�[�^�T�C�Y
	vertexSize = u_size * (v_size + 1);
	vertices.resize(vertexSize);

	for (int v = 0; v <= v_size; v++)
	{
		for (int u = 0; u < u_size; u++)
		{
			float theta = MyMath::PI * v / v_size;
			float phi = MyMath::PI * 2 * u / u_size;
			Vector3D pos(sinf(theta) * cosf(phi) * 10,
				cosf(theta) * 10,
				sinf(theta) * sinf(phi) * 10);
			vertices[u_size * v + u].pos = pos;
		}
	}

	//	�C���f�b�N�X�T�C�Y
	indexSize = 6 * v_size * u_size;
	indices.resize(indexSize);
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indexSize);

	int idx = 0;
	for (int v = 0; v < v_size; v++)
	{
		for (int u = 0; u < u_size; u++)
		{
			if (u == u_size - 1) {
				indices[idx] = u + v * u_size;
				idx++;
				indices[idx] = v * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;

				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = (v + 1) * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;
			}
			else {
				indices[idx] = u + v * u_size;
				idx++;
				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;

				indices[idx] = u + 1 + v * u_size;
				idx++;
				indices[idx] = u + 1 + (v + 1) * u_size;
				idx++;
				indices[idx] = u + (v + 1) * u_size;
				idx++;
			}
		}
	}

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertexSize);
	Init(dev, sizeVB, vertexSize, sizeIB, &indices.front(), indexSize);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz���W
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	�@���x�N�g��
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv���W
	};

	pipeline.Init(dev, shader, inputLayout, _countof(inputLayout), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D12_FILL_MODE_WIREFRAME);
}

void SphereObj::Draw(ID3D12GraphicsCommandList* cmdList)
{
	pipeline.Setting(cmdList);
	pipeline.Update(cmdList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Update(cmdList);
	cmdList->DrawIndexedInstanced(indexSize, 1, 0, 0, 0);
}

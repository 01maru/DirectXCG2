#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


class VertexBuffer
{
public:
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	ID3D12Resource* vertBuff = nullptr;
	XMFLOAT3* map = nullptr; 	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	D3D12_VERTEX_BUFFER_VIEW view{};	// ���_�o�b�t�@�r���[

public:
	VertexBuffer(UINT sizeVB, ID3D12Device* dev, XMFLOAT3* vertices, UINT size);
};

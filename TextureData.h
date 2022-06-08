#pragma once
#include <DirectXTex.h>
using namespace DirectX;
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class TextureDeta
{
public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ID3D12DescriptorHeap* srvHeap = nullptr;

public:
	TextureDeta(ID3D12Device* dev, D3D12_RESOURCE_DESC resDesc);
	void Update(ID3D12GraphicsCommandList* cmdList);
};


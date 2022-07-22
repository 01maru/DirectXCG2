#pragma once
#include <DirectXTex.h>
using namespace DirectX;
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl.h>

class Texture
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ComPtr<ID3D12Resource> texBuff;

public:
	void Initialize(ID3D12Device* dev, D3D12_RESOURCE_DESC& tectureResourceDesc, D3D12_HEAP_PROPERTIES textureHeapProp);
	void SetSRV(ID3D12Device* dev, D3D12_RESOURCE_DESC resDesc, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle, size_t index = NULL);
	void LoadTexture(const wchar_t* textureName);
};


class TextureDeta
{
public:
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ID3D12DescriptorHeap* srvHeap = nullptr;
	UINT incrementSize;
	int textureNum = 0;
	
public:
	TextureDeta(ID3D12Device* dev, D3D12_RESOURCE_DESC resDesc, Texture* texture);
	~TextureDeta();
	void SetDescriptorTable(ID3D12GraphicsCommandList* cmdList);
	void Update(ID3D12GraphicsCommandList* cmdList);
};


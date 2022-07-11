#pragma once
#include <d3d12.h>
#include "MyMath.h"
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
class ConstBuff
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	D3D12_RESOURCE_DESC resourceDesc{};


	struct ConstBufferDataMaterial {
		MyMath::float4 color;	//	RGBA
	};
	ComPtr<ID3D12Resource> material;
	ConstBufferDataMaterial* mapMaterial = nullptr;

public:
	ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight);
	~ConstBuff();
	void Update(ID3D12GraphicsCommandList* cmdList);
};


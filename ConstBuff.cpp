#include "ConstBuff.h"
#include "Vector3D.h"
#include <cassert>

ConstBuff::ConstBuff(ID3D12Device* dev, const int winwidth, const int winheight)
{	
	//	ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//	GPU転送用

	//	リソース設定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	resourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xFF) & ~0xFF;
	//	生成
	result = dev->CreateCommittedResource(
		&heapProp,	//	ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,	//	リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&material));
	assert(SUCCEEDED(result));

	//	定数バッファのマッピング
	result = material->Map(0, nullptr, (void**)&mapMaterial);	//	マッピング
	assert(SUCCEEDED(result));


	//	GPUのメモリにデータ転送
	ChangeColor(Vector4D(1.0f, 1.0f, 1.0f, 1.0f));

	resourceDesc.Width = (sizeof(ConstBufferTimeMaterial) + 0xFF) & ~0xFF;
	//	生成
	result = dev->CreateCommittedResource(
		&heapProp,	//	ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,	//	リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&timeMaterial));
	assert(SUCCEEDED(result));

	//	定数バッファのマッピング
	result = timeMaterial->Map(0, nullptr, (void**)&timeMapMaterial);	//	マッピング
	assert(SUCCEEDED(result));


	//	GPUのメモリにデータ転送
	timeMapMaterial->time = 0;
}

ConstBuff::~ConstBuff()
{
	material->Unmap(0, nullptr);
	timeMaterial->Unmap(0, nullptr);
}

void ConstBuff::ChangeColor(const Vector4D& color)
{
	//	GPUのメモリにデータ転送
	mapMaterial->color = color;
}

void ConstBuff::TimeUpdate()
{
	timeMapMaterial->time += 0.1f;
}

void ConstBuff::SetBufferView(ID3D12GraphicsCommandList* cmdList)
{
	//	定数バッファビュー(CBV)の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(1, timeMaterial->GetGPUVirtualAddress());
}

void ConstBuff::Update(ID3D12GraphicsCommandList* cmdList)
{
	//	定数バッファビュー(CBV)の設定コマンド
	cmdList->SetGraphicsRootConstantBufferView(0, material->GetGPUVirtualAddress());
}

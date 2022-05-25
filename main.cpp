#include "Window.h"
#include "DirectXInit.h"
#include "VertBuff.h"
#include "VertShader.h"
#include "GPipeline.h"
#include "Input.h"
#include "ViewPort.h"
#include "ScissorRect.h"
#include "ConstBuff.h"
#include <DirectXTex.h>

#include <DirectXMath.h>
using namespace DirectX;


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	HRESULT result;
#pragma region Initialize
	Window win;

	DirectXInit dx(win.hwnd);

	//	描画初期化
	// 頂点データ
	Vertex vertices[] = {
		{{ 0.0f,100.0f,0.0f }, {0.0f, 1.0f}}, // 左下
		{{ 0.0f,0.0f,0.0f }, {0.0f, 0.0f}}, // 左上
		{{100.0f,100.0f,0.0f }, {1.0f, 1.0f}}, // 右下
		{{100.0f,0.0f,0.0f}, {1.0f, 0.0f}}, // 右上
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//	インデックスデータ
	unsigned short indices[] =
	{
		0,1,2,
		1,2,3,
	};
	//	全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	VertBuff vertBuff(sizeVB, vertices, _countof(vertices), sizeIB, indices, _countof(indices), dx.device);

	VertShader vertShade;

	Input input(win.hwnd, win.w);

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

#pragma region 画像イメージデータ作成
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resource/texture.jpg",
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	//	ミニマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);

	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	metadata.format = MakeSRGB(metadata.format);

	//const size_t textureWidth = 256;
	//const size_t textureHeight = 256;
	//const size_t imageDataCount = textureWidth * textureHeight;
	//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//for (size_t i = 0; i < imageDataCount; i++) {
	//	imageData[i].x = 1.0f;
	//	imageData[i].y = 0.0f;
	//	imageData[i].z = 0.0f;
	//	imageData[i].w = 1.0f;
	//}

	//	ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//	リソース設定
	D3D12_RESOURCE_DESC tectureResourceDesc{};
	tectureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	tectureResourceDesc.Format = metadata.format;
	tectureResourceDesc.Width = metadata.width;
	tectureResourceDesc.Height = (UINT)metadata.height;
	tectureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	tectureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	tectureResourceDesc.SampleDesc.Count = 1;


	//	テクスチャバッファ生成
	ID3D12Resource* texBuff = nullptr;
	result = dx.device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&tectureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	//	テクスチャバッファ転送
	//result = texBuff->WriteToSubresource(0, nullptr, imageData, sizeof(XMFLOAT4) * textureWidth, sizeof(XMFLOAT4) * imageDataCount);
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		const Image* img = scratchImg.GetImage(i, 0, 0);

		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);
		assert(SUCCEEDED(result));
	}

	//delete[] imageData;

	const size_t kMaxSRVCount = 2056;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	ID3D12DescriptorHeap* srvHeap = nullptr;
	result = dx.device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));


	//UINT descriptorSize = device.Dev()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//srvHandle.ptr += descriptorSize * 1;

	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	//device.Dev()->CreateConstantBufferView(&cbvDesc, srvHandle);


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = vertBuff.resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = vertBuff.resDesc.MipLevels;

	dx.device->CreateShaderResourceView(texBuff, &srvDesc, srvHandle);
#pragma endregion


	//	グラフィックスパイプライン
	GPipeline gPipeLine(vertShade, inputLayout, _countof(inputLayout), dx.device);

	//	ビューポート
	ViewPort viewPort(window_width, window_height, 0, 0);
	// シザー矩形
	ScissorRect scissorRect(0, window_width, 0, window_height);

	//	定数バッファ
	ConstBuff cBuff(dx.device, window_width, window_height);
#pragma endregion Initialize

	//	ゲームループ
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();
		
#pragma region DrawPrep
		dx.DrawAble();

		// 3.画面クリア			R	　G		 B	   A
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // 青っぽい色

		dx.ScreenClear(clearColor);

#pragma endregion	DrawPrep

#pragma region Draw
		// 4.描画コマンドここから
		viewPort.Update(dx.commandList);

		scissorRect.Update(dx.commandList);

		gPipeLine.Update(dx.commandList);

		// プリミティブ形状の設定コマンド
		dx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		vertBuff.Update(dx.commandList);

		cBuff.Update(dx.commandList);

#pragma region 04_02
		dx.commandList->SetDescriptorHeaps(1, &srvHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		dx.commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
#pragma endregion

		cBuff.CBUpdate(dx.commandList);

		// 描画コマンド
		//dx.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画
		dx.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);

		// 4.描画コマンドここまで
#pragma endregion Draw

		dx.DrawEnd();
	}

	return 0;
}
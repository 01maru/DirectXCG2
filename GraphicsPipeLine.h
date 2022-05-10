#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "VertexShader.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class GraphicsPipeLine
{
public:
	D3D12_ROOT_PARAMETER rootParam = {};		//	ルートパラメータ
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};	// グラフィックスパイプライン構造体
	ID3D12RootSignature* rootSignature;
	ID3D12PipelineState* state = nullptr;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
public:
	GraphicsPipeLine(VertexShader vertShade, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT size, ID3D12Device* dev);

	void AlphaBlend();
	void InversBlend();
	void SubBlend();
	void AddBlend();
	void NormalBlend();
};


#pragma once
#include "VertShader.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

enum BlendMord
{
	NONE_BLEND,
	ADD_BLEND,
	SUB_BLEND,
	INV_BLEND,
	ALPHA_BLEND,
};

class GPipeline
{
public:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	ID3D12RootSignature* rootSignature;
	ID3D12PipelineState* state = nullptr;
public:
	GPipeline(VertShader vShade, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT inputLayoutSize, ID3D12Device* dev);
	void Update(ID3D12GraphicsCommandList* cmdList);
	void Blend(D3D12_RENDER_TARGET_BLEND_DESC& blenddesc, const int mord = NONE_BLEND);
};


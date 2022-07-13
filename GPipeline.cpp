#include "GPipeline.h"
#include <vector>

GPipeline::GPipeline(D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT inputLayoutSize, ID3D12Device* dev, Shader shader)
{
	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;

	D3D12_ROOT_PARAMETER rp = {};
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp.DescriptorTable.pDescriptorRanges = &range;
	rp.DescriptorTable.NumDescriptorRanges = 1;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 1;
	rsDesc.NumStaticSamplers = 1;

#pragma region sampler
	//	テクスチャーサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC sampler{};
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.MinLOD = 0.0f;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

	rsDesc.pStaticSamplers = &sampler;
	rsDesc.pParameters = &rp;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> rsBlob;
	ComPtr<ID3DBlob> errBlob;

	HRESULT result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rsBlob.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf());

	result = dev->CreateRootSignature(
		0,
		rsBlob.Get()->GetBufferPointer(),
		rsBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));

	SetShader(shader);
	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DepthStencilState.StencilEnable = false;

	D3D12_INPUT_ELEMENT_DESC layout[2] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	pipelineDesc.InputLayout.NumElements = _countof(layout);
	pipelineDesc.InputLayout.pInputElementDescs = layout;
#pragma region  Blending
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	Blend(blenddesc);
#pragma endregion
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
#pragma region Rasterizer
	// 設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に
#pragma endregion
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.SampleDesc.Quality = 0;
	pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;


	pipelineDesc.pRootSignature = rootSignature.Get();
	result = dev->CreateGraphicsPipelineState(&pipelineDesc,
		IID_PPV_ARGS(state.ReleaseAndGetAddressOf()));
}

GPipeline::GPipeline(ID3D12Device* dev, Shader shader, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT inputLayoutSize,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
{
	HRESULT result;
	// シェーダーの設定
	SetShader(shader);

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

#pragma region Rasterizer
	// 設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に
#pragma endregion

#pragma region  Blending
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	Blend(blenddesc);
#pragma endregion

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = inputLayoutSize;

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = topologyType;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1;							  // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;							  // 1ピクセルにつき1回サンプリング

	//	デプスステンシルステート設定
	pipelineDesc.DepthStencilState.DepthEnable = true;								//	深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;		//	書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;			//	小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;									//	深度フォーマット

	SetRootSignature(dev, 3);

	// パイプランステートの生成
	result = dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&state));
	assert(SUCCEEDED(result));
}

void GPipeline::Update(ID3D12GraphicsCommandList* cmdList, D3D_PRIMITIVE_TOPOLOGY primitive)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	cmdList->SetPipelineState(state.Get());
	cmdList->IASetPrimitiveTopology(primitive);
}

void GPipeline::Setting(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
}

void GPipeline::Blend(D3D12_RENDER_TARGET_BLEND_DESC& blenddesc, const int mord)
{
	//	共通設定
	if (mord != NONE_BLEND) {
		blenddesc.BlendEnable = true;
		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	}

	switch (mord)
	{
	case ADD_BLEND:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	case SUB_BLEND:
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		break;
	case INV_BLEND:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		break;
	case ALPHA_BLEND:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	default:
		break;
	}
}

void GPipeline::SetRootParam(D3D12_ROOT_PARAMETER& rootParam, D3D12_ROOT_PARAMETER_TYPE type, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY shaderVisibility)
{
	rootParam.ParameterType = type;							//	定数バッファビュー
	rootParam.Descriptor.ShaderRegister = shaderRegister;	//	定数バッファ番号
	rootParam.Descriptor.RegisterSpace = registerSpace;		//	デフォルト値
	rootParam.ShaderVisibility = shaderVisibility;			//	すべてのシェーダから見る
}

void GPipeline::SetRootParam(D3D12_ROOT_PARAMETER& rootParam, D3D12_ROOT_PARAMETER_TYPE type, D3D12_DESCRIPTOR_RANGE pDescriptorRange, UINT numDescriptorRanges, D3D12_SHADER_VISIBILITY shaderVisibility)
{
	rootParam.ParameterType = type;
	rootParam.DescriptorTable.pDescriptorRanges = &pDescriptorRange;
	rootParam.DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
	rootParam.ShaderVisibility = shaderVisibility;
}

void GPipeline::SetRootSignature(ID3D12Device* dev, UINT rootParamNum)
{
	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma region	ルートパラメータ
	//	ルートパラメータの設定
	std::vector<D3D12_ROOT_PARAMETER> rootParams = {};
	rootParams.resize(rootParamNum);
	SetRootParam(rootParams[0], D3D12_ROOT_PARAMETER_TYPE_CBV, 0, 0);
	SetRootParam(rootParams[1], D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, descriptorRange, 1);
	SetRootParam(rootParams[2], D3D12_ROOT_PARAMETER_TYPE_CBV, 1, 0);
#pragma endregion

#pragma region sampler
	//	テクスチャーサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

#pragma region ルートシグネチャ
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParams.front();						//	先頭アドレス
	rootSignatureDesc.NumParameters = rootParams.size();						//	ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		rootSigBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();
#pragma endregion
}

void GPipeline::SetScreenRootSignature(ID3D12Device* dev)
{
	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma region	ルートパラメータ
	//	ルートパラメータの設定
	std::vector<D3D12_ROOT_PARAMETER> rootParams = {};
	rootParams.resize(1);
	SetRootParam(rootParams[0], D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, descriptorRange, 1, D3D12_SHADER_VISIBILITY_PIXEL);
#pragma endregion

#pragma region sampler
	//	テクスチャーサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
#pragma endregion

#pragma region ルートシグネチャ
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParams.front();						//	先頭アドレス
	rootSignatureDesc.NumParameters = rootParams.size();						//	ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		rootSigBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();
#pragma endregion
}

void GPipeline::SetBlend(ID3D12Device* dev, int mord)
{
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	Blend(blenddesc, mord);
	HRESULT result = dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&state));
	assert(SUCCEEDED(result));
}

void GPipeline::SetShader(Shader shader)
{
#pragma region VertexShader
	pipelineDesc.VS.pShaderBytecode = shader.VSBlob()->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = shader.VSBlob()->GetBufferSize();
#pragma endregion
#pragma region HShader
	if (shader.HSBlob() != nullptr) {
		pipelineDesc.HS.pShaderBytecode = shader.HSBlob()->GetBufferPointer();
		pipelineDesc.HS.BytecodeLength = shader.HSBlob()->GetBufferSize();
	}
#pragma endregion
#pragma region DShader
	if (shader.DSBlob() != nullptr) {
		pipelineDesc.DS.pShaderBytecode = shader.DSBlob()->GetBufferPointer();
		pipelineDesc.DS.BytecodeLength = shader.DSBlob()->GetBufferSize();
	}
#pragma endregion
#pragma region GShader
	if (shader.GSBlob() != nullptr) {
		pipelineDesc.GS.pShaderBytecode = shader.GSBlob()->GetBufferPointer();
		pipelineDesc.GS.BytecodeLength = shader.GSBlob()->GetBufferSize();
	}
#pragma endregion
#pragma region PixcelShader
	pipelineDesc.PS.pShaderBytecode = shader.PSBlob()->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = shader.PSBlob()->GetBufferSize();
#pragma endregion
}

#include "GraphicsPipeLine.h"

GraphicsPipeLine::GraphicsPipeLine(VertexShader vertShade, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT size, ID3D12Device* dev)
{
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

	//デスクリプタレンジの設定
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//	ルートパラメータの設定
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	定数バッファビュー
	rootParam[0].Descriptor.ShaderRegister = 0;					//	定数バッファ番号
	rootParam[0].Descriptor.RegisterSpace = 0;						//	デフォルト値
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	すべてのシェーダから見る

	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//	定数バッファビュー
	rootParam[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//	定数バッファ番号
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;						//	デフォルト値
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	すべてのシェーダから見る

	// シェーダーの設定
	desc.VS.pShaderBytecode = vertShade.vsBlob->GetBufferPointer();
	desc.VS.BytecodeLength = vertShade.vsBlob->GetBufferSize();
	desc.PS.pShaderBytecode = vertShade.psBlob->GetBufferPointer();
	desc.PS.BytecodeLength = vertShade.psBlob->GetBufferSize();

	// サンプルマスクの設定
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶしorワイヤーフレーム
	desc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	//desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//	RGBAすべてのチャンネル描画
	
	// 頂点レイアウトの設定
	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = size;

	// 図形の形状設定
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	desc.NumRenderTargets = 1; // 描画対象は1つ
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	desc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// ルートシグネチャ
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParam;		//	先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParam);			//	ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &vertShade.errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	desc.pRootSignature = rootSignature;

	// パイプランステートの生成
	result = dev->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&state));
	assert(SUCCEEDED(result));
}

void GraphicsPipeLine::AlphaBlend()
{
	//	半透明合成
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;		//	ソースのα値を使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	//	1.0f-ソースのα値を使う
}

void GraphicsPipeLine::InversBlend()
{
	//	色反転
	blenddesc.BlendEnable = true;					//	有効に
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_COLOR;	//	1.0f-デストカラーの値を使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;			//	使わない
}

void GraphicsPipeLine::SubBlend()
{
	//	減算合成
	blenddesc.BlendEnable = true;					//	有効に
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;	//	減算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;				//	ソースの値を100％使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;				//	デストの値を100％使う
}

void GraphicsPipeLine::AddBlend()
{
	//	加算合成
	blenddesc.BlendEnable = true;					//	有効に
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	ソースの値を100％使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;		//	デストの値を100％使う
}

void GraphicsPipeLine::NormalBlend()
{
	blenddesc.BlendEnable = true;					//	有効に
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	ソースの値を100％使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//	使わない
}

#include "GraphicsPipeLine.h"

GraphicsPipeLine::GraphicsPipeLine(VertexShader vertShade, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT size, ID3D12Device* dev)
{
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
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//	RGBAすべてのチャンネル描画

	//blenddesc.BlendEnable = true;					//	有効に
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	加算
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	ソースの値を100％使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//	使わない

	////	加算合成
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	加算
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	ソースの値を100％使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;		//	デストの値を100％使う
	////	減算合成
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;	//	減算
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;				//	ソースの値を100％使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;				//	デストの値を100％使う
	////	色反転
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	加算
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_COLOR;	//	1.0f-デストカラーの値を使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;			//	使わない
	////	半透明合成
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	加算
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;		//	ソースのα値を使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	//	1.0f-ソースのα値を使う
	

	// 頂点レイアウトの設定
	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = size;

	// 図形の形状設定
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	desc.NumRenderTargets = 1; // 描画対象は1つ
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255指定のRGBA
	desc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// ルートシグネチャ
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
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

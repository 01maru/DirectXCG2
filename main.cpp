#include "Window.h"
#include "DirectXInit.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3dcompiler.lib")

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
#pragma region Initialize
	Window win;

	DirectXInit(win.hwnd);

#pragma endregion Initialize

	//	描画初期化
	// 頂点データ
	//XMFLOAT3 vertices[] = {
	//	{ -0.5f, -0.5f, 0.0f }, // 左下
	//	{ -0.5f, +0.5f, 0.0f }, // 左上
	//	{ +0.5f, -0.5f, 0.0f }, // 右下
	//};
	//// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	//UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//// 頂点バッファの設定
	//D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	//heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	//// リソース設定
	//D3D12_RESOURCE_DESC resDesc{};
	//resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	//resDesc.Height = 1;
	//resDesc.DepthOrArraySize = 1;
	//resDesc.MipLevels = 1;
	//resDesc.SampleDesc.Count = 1;
	//resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//// 頂点バッファの生成
	//ID3D12Resource* vertBuff = nullptr;
	//result = device->CreateCommittedResource(
	//	&heapProp, // ヒープ設定
	//	D3D12_HEAP_FLAG_NONE,
	//	&resDesc, // リソース設定
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&vertBuff));

	////	GPUメモリの値書き換えよう
	//// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	//XMFLOAT3* vertMap = nullptr;
	//result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	//assert(SUCCEEDED(result));
	//// 全頂点に対して
	//for (int i = 0; i < _countof(vertices); i++) {
	//	vertMap[i] = vertices[i]; // 座標をコピー
	//}
	//// 繋がりを解除
	//vertBuff->Unmap(0, nullptr);

	//// 頂点バッファビューの作成
	//D3D12_VERTEX_BUFFER_VIEW vbView{};
	//// GPU仮想アドレス
	//vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//// 頂点バッファのサイズ
	//vbView.SizeInBytes = sizeVB;
	//// 頂点1つ分のデータサイズ
	//vbView.StrideInBytes = sizeof(XMFLOAT3);

	////	頂点シェーダファイル読み込み＆コンパイル
	//ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	//ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	//ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	//// 頂点シェーダの読み込みとコンパイル
	//result = D3DCompileFromFile(
	//	L"BasicVS.hlsl", // シェーダファイル名
	//	nullptr,
	//	D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
	//	"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
	//	0,
	//	&vsBlob, &errorBlob);

	//// エラーなら
	//if (FAILED(result)) {
	//	// errorBlobからエラー内容をstring型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());
	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferSize(),
	//		error.begin());
	//	error += "\n";
	//	// エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//// ピクセルシェーダの読み込みとコンパイル
	//result = D3DCompileFromFile(
	//	L"BasicPS.hlsl", // シェーダファイル名
	//	nullptr,
	//	D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
	//	"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
	//	D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
	//	0,
	//	&psBlob, &errorBlob);

	//// エラーなら
	//if (FAILED(result)) {
	//	// errorBlobからエラー内容をstring型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());
	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferSize(),
	//		error.begin());
	//	error += "\n";
	//	// エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	//{
	//"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, 
	//};

	////	グラフィックスパイプライン
	//// グラフィックスパイプライン構造体設定
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//// シェーダーの設定
	//pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	//pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	//pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	//pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//// サンプルマスクの設定
	//pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	//// ラスタライザの設定
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	//pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	//pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	//// ブレンドステート
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

	//// 頂点レイアウトの設定
	//pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	//pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//// 図形の形状設定
	//pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//// その他の設定
	//pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	//pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	//pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//// ルートシグネチャ
	//ID3D12RootSignature* rootSignature;
	//// ルートシグネチャの設定
	//D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	//rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//// ルートシグネチャのシリアライズ
	//ID3DBlob* rootSigBlob = nullptr;
	//result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
	//	&rootSigBlob, &errorBlob);
	//assert(SUCCEEDED(result));
	//result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
	//	IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(result));
	//rootSigBlob->Release();
	//// パイプラインにルートシグネチャをセット
	//pipelineDesc.pRootSignature = rootSignature;

	//// パイプランステートの生成
	//ID3D12PipelineState* pipelineState = nullptr;
	//result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	//assert(SUCCEEDED(result));

	//	ゲームループ
	while (true)
	{
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
		
		////	リリースバリア
		//// バックバッファの番号を取得(2つなので0番か1番)
		//UINT bbIndex = swapChain->GetCurrentBackBufferIndex();
		//// 1.リソースバリアで書き込み可能に変更
		//D3D12_RESOURCE_BARRIER barrierDesc{};
		//barrierDesc.Transition.pResource = backBuffers[bbIndex]; // バックバッファを指定
		//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		//commandList->ResourceBarrier(1, &barrierDesc);

		////	描画先指定コマンド
		//// 2.描画先の変更
		//// レンダーターゲットビューのハンドルを取得
		//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		////	画面クリアコマンド
		//// 3.画面クリア			R	　G		B	 A
		//FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // 青っぽい色
		//commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//// 4.描画コマンドここから
		//
		//// ビューポート設定コマンド
		//D3D12_VIEWPORT viewport{};
		//viewport.Width = window_width;
		//viewport.Height = window_height;
		//viewport.TopLeftX = 0;
		//viewport.TopLeftY = 0;
		//viewport.MinDepth = 0.0f;
		//viewport.MaxDepth = 1.0f;
		//// ビューポート設定コマンドを、コマンドリストに積む
		//commandList->RSSetViewports(1, &viewport);

		//// シザー矩形
		//D3D12_RECT scissorRect{};
		//scissorRect.left = 0; // 切り抜き座標左
		//scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		//scissorRect.top = 0; // 切り抜き座標上
		//scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		//// シザー矩形設定コマンドを、コマンドリストに積む
		//commandList->RSSetScissorRects(1, &scissorRect);

		//// パイプラインステートとルートシグネチャの設定コマンド
		//commandList->SetPipelineState(pipelineState);
		//commandList->SetGraphicsRootSignature(rootSignature);

		//// プリミティブ形状の設定コマンド
		//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		//// 頂点バッファビューの設定コマンド
		//commandList->IASetVertexBuffers(0, 1, &vbView);

		//// 描画コマンド
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画

		//// 4.描画コマンドここまで

		//// 5.リソースバリアを戻す
		//barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		//barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		//commandList->ResourceBarrier(1, &barrierDesc);

		//// 命令のクローズ
		//result = commandList->Close();
		//assert(SUCCEEDED(result));
		//// コマンドリストの実行
		//ID3D12CommandList* commandLists[] = { commandList };
		//commandQueue->ExecuteCommandLists(1, commandLists);
		//// 画面に表示するバッファをフリップ(裏表の入替え)
		//result = swapChain->Present(1, 0);
		//assert(SUCCEEDED(result));

		////	画面入れ替え
		//// コマンドの実行完了を待つ
		//commandQueue->Signal(fence, ++fenceVal);
		//if (fence->GetCompletedValue() != fenceVal) {
		//	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		//	fence->SetEventOnCompletion(fenceVal, event);
		//	WaitForSingleObject(event, INFINITE);
		//	CloseHandle(event);
		//}
		//// キューをクリア
		//result = commandAllocator->Reset();
		//assert(SUCCEEDED(result));
		//// 再びコマンドリストを貯める準備
		//result = commandList->Reset(commandAllocator, nullptr);
		//assert(SUCCEEDED(result));
	}

	return 0;
}
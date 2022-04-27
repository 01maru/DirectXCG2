#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <d3dcompiler.h>
#include "Window.h"
#include "EnumAdapter.h"
#include "Device.h"
#include "CmdList.h"
#include "CmdQueue.h"
#include "SwapChain.h"
#include "DesHeap.h"
#include "Buffer.h"
#include "Fence.h"
#include "Input.h"
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:		//	ウィンドウが閉じられた
	case WM_DESTROY:	//	ウィンドウが破棄された
		PostQuitMessage(0);
		return 0;
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Window win((WNDPROC)WindowProc);

	//	初期化
	HRESULT result;

	EnumAdapter adapter;

#ifdef _DEBUG
	//	デバッグレイヤーをオン
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG


	Device device(adapter.tmpAdapter);

	//	コマンドリスト
	CmdList cmdList(device.dev);

	//コマンドキュー
	CmdQueue cmdQueue(device.dev);

	//	スワップチェーン(ダブルバッファリング用)
	SwapChain swapChain(adapter.dxgiFactory, cmdQueue.commandQueue, win.hwnd);

	// デスクリプタヒープの設定(GPUにメモリ領域確保しそれから使う)
	DesHeap desHeap(swapChain.swapChainDesc, device.dev);

	// バックバッファ
	Buffer buffer(swapChain, desHeap, device.dev);

	// フェンスの生成
	Fence fence(device.dev);

	//	キー入力
	Input input(win.hwnd, win.w);

	//	描画初期化
	// 頂点データ
	XMFLOAT3 vertices[] = {
	{ -0.5f, -0.5f, 0.0f }, // 左下
	{ -0.5f, +0.5f, 0.0f }, // 左上
	{ +0.5f, -0.5f, 0.0f }, // 右下
	{ +0.5f, +0.5f, 0.0f }, // 右上
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = device.dev->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//	GPUメモリの値書き換えよう
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	//	頂点シェーダファイル読み込み＆コンパイル
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION",
	0,
	DXGI_FORMAT_R32G32B32_FLOAT,
	0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	0
	}, 
	};

	//	グラフィックスパイプライン
	// グラフィックスパイプライン構造体設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶしorワイヤーフレーム
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	// ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 2; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device.dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	// パイプランステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	result = device.dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//	ゲームループ
	while (true)
	{
		//	ウィンドウメッセージ処理
		win.MessageUpdate();
		if (win.EndLoop()) { break; }

		//	DirectX毎フレーム処理
		//	キー情報取得
		input.Update();
		
		//	リリースバリア
		// バックバッファの番号を取得(2つなので0番か1番)
		UINT bbIndex = swapChain.swapChain->GetCurrentBackBufferIndex();
		// 1.リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = buffer.backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		cmdList.commandList->ResourceBarrier(1, &barrierDesc);

		//	描画先指定コマンド
		// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = desHeap.rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device.dev->GetDescriptorHandleIncrementSize(desHeap.rtvHeapDesc.Type);
		cmdList.commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//	画面クリアコマンド
		// 3.画面クリア			R	　G		B	 A
		FLOAT clearColor[] = { 1,0.25f, 0.25f,0.0f }; // 青っぽい色
		
		cmdList.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.描画コマンドここから

		if (input.GetTrigger(DIK_1)) {
			if (pipelineDesc.RasterizerState.FillMode == D3D12_FILL_MODE_SOLID) {
				pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			} else if (pipelineDesc.RasterizerState.FillMode == D3D12_FILL_MODE_WIREFRAME) {
				pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			}
			result = device.dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
			assert(SUCCEEDED(result));
		}

		// シザー矩形
		D3D12_RECT scissorRect{};
		// シザー矩形
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		cmdList.commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		cmdList.commandList->SetPipelineState(pipelineState);
		cmdList.commandList->SetGraphicsRootSignature(rootSignature);

		// プリミティブ形状の設定コマンド
		cmdList.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		cmdList.commandList->IASetVertexBuffers(0, 1, &vbView);
		
		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport[4]{};
		for (size_t i = 0; i < _countof(viewport); i++)
		{
			viewport[i].Width = window_width / 4;		//	横幅
			viewport[i].Height = window_height;	//	縦幅
			viewport[i].TopLeftX = window_width / 4 * i;				//	左上X
			viewport[i].TopLeftY = 0;				//	左上Y
			viewport[i].MinDepth = 0.0f;			//	最小深度
			viewport[i].MaxDepth = 1.0f;			//	最大深度

			// ビューポート設定コマンドを、コマンドリストに積む
			cmdList.commandList->RSSetViewports(1, &viewport[i]);
			

			// 描画コマンド
			cmdList.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画
		}
		

		if (input.GetKey(DIK_SPACE))	//	スペース押したら色変え
		{
			cmdList.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}
		// 4.描画コマンドここまで

		// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		cmdList.commandList->ResourceBarrier(1, &barrierDesc);

		// 命令のクローズ
		result = cmdList.commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { cmdList.commandList };
		cmdQueue.commandQueue->ExecuteCommandLists(1, commandLists);
		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain.swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//	画面入れ替え
		// コマンドの実行完了を待つ
		cmdQueue.commandQueue->Signal(fence.fnc, ++fence.fenceVal);
		if (fence.fnc->GetCompletedValue() != fence.fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence.fnc->SetEventOnCompletion(fence.fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = cmdList.commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = cmdList.commandList->Reset(cmdList.commandAllocator, nullptr);
		assert(SUCCEEDED(result));
	}

	UnregisterClass(win.w.lpszClassName, win.w.hInstance);

	return 0;
}

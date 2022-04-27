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
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "GraphicsPipeLine.h"
#include "ConstBuffer.h"
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Window win;

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
	{ -0.5f, +0.5f, 0.0f }, // 左上
	{ +0.5f, +0.5f, 0.0f }, // 右上
	{ -0.5f, 0.0f, 0.0f },	// 左
	{ +0.5f, 0.0f, 0.0f },	// 右
	{ -0.5f, -0.5f, 0.0f }, // 左下
	{ +0.5f, -0.5f, 0.0f }, // 右下
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));
	//	インデックスデータ
	uint16_t indices[] =
	{
		0,1,
		1,2,
		2,3,
		3,4,
		4,5,
		0,3,
		2,5,
	};
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// 頂点バッファ
	VertexBuffer vertBuff(sizeVB, device.dev, vertices, _countof(vertices), sizeIB, indices, _countof(indices));

	//	頂点シェーダファイル読み込み＆コンパイル
	VertexShader vertShade;

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//	ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	定数バッファビュー
	rootParam.Descriptor.ShaderRegister = 0;					//	定数バッファ番号
	rootParam.Descriptor.RegisterSpace = 0;						//	デフォルト値
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	すべてのシェーダから見る

	//	グラフィックスパイプライン
	GraphicsPipeLine gPipeLine(vertShade, inputLayout, _countof(inputLayout), device.dev, rootParam);

	//	定数バッファ
	ConstBuffer cBuff(device.dev);


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
		FLOAT clearColor[] = { 0.1, 0.25f, 0.5f, 0.0f }; // 青っぽい色
		cmdList.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.描画コマンドここから
		
		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;		//	横幅
		viewport.Height = window_height;	//	縦幅
		viewport.TopLeftX = 0;				//	左上X
		viewport.TopLeftY = 0;				//	左上Y
		viewport.MinDepth = 0.0f;			//	最小深度
		viewport.MaxDepth = 1.0f;			//	最大深度
		// ビューポート設定コマンドを、コマンドリストに積む
		cmdList.commandList->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		cmdList.commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		cmdList.commandList->SetPipelineState(gPipeLine.state);
		cmdList.commandList->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// プリミティブ形状の設定コマンド
		cmdList.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		cmdList.commandList->IASetVertexBuffers(0, 1, &vertBuff.view);



		//	グラフィックスコマンド
		cmdList.commandList->SetGraphicsRootConstantBufferView(0, cBuff.material->GetGPUVirtualAddress());	//	定数バッファビューの設定コマンド

		//	インデックスバッファビューの設定
		cmdList.commandList->IASetIndexBuffer(&vertBuff.ibView);

		// 描画コマンド
		cmdList.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画

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

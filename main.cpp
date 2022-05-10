#include "Window.h"
#include "EnumAdapter.h"
#include "Device.h"
#include "CmdList.h"
#include "CmdQueue.h"
#include "SwapChain.h"
#include "DesHeap.h"
#include "BackBuffer.h"
#include "Fence.h"
#include "Input.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "GraphicsPipeLine.h"
#include "ConstBuffer.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	#pragma region Initialize
	Window win;

	//	初期化
	HRESULT result;

	//	アダプター列挙(一つだけ)
	EnumAdapter adapter;

#ifdef _DEBUG
	//	デバッグレイヤーをオン
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG

	Device device(adapter.TmpAdapter());

	//	コマンドリスト
	CmdList cmdList(device.Dev());

	//コマンドキュー
	CmdQueue cmdQueue(device.Dev());

	//	スワップチェーン(ダブルバッファリング用)
	SwapChain swapChain(adapter.DxgiFactory(), cmdQueue.Queue(), win.hwnd);

	// デスクリプタヒープの設定(GPUにメモリ領域確保しそれから使う)
	DesHeap desHeap(swapChain.Desc(), device.Dev());

	// バックバッファ
	BackBuffer buffer(swapChain, desHeap, device.Dev());

	// フェンスの生成
	Fence fence(device.Dev());

	//	キー入力
	Input input(win.hwnd, win.w);

	//	描画初期化
	// 頂点データ
	Vertex vertices[] = {
		{{ -0.5f, -0.5f, 0.0f }, {0.0f, 1.0f}}, // 左下
		{{ -0.5f, +0.5f, 0.0f }, {0.0f, 0.0f}}, // 左上
		{{ +0.5f, -0.5f, 0.0f }, {1.0f, 1.0f}}, // 右下
		{{ +0.5f, +0.5f, 0.0f }, {1.0f, 0.0f}}, // 右上
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	//	インデックスデータ
	uint16_t indices[] =
	{
		0,1,2,
		1,2,3,
	};
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// 頂点バッファ
	VertexBuffer vertBuff(sizeVB, device.Dev(), vertices, _countof(vertices), sizeIB, indices, _countof(indices));

	//	頂点シェーダファイル読み込み＆コンパイル
	VertexShader vertShade;

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//	グラフィックスパイプライン
	GraphicsPipeLine gPipeLine(vertShade, inputLayout, _countof(inputLayout), device.Dev());

	//	定数バッファ
	ConstBuffer cBuff(device.Dev());

	#pragma endregion Initialize

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
		UINT bbIndex = swapChain.Chain()->GetCurrentBackBufferIndex();
		// 1.リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = buffer.BackBuff()[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
		cmdList.List()->ResourceBarrier(1, &barrierDesc);

		//	描画先指定コマンド
		// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = desHeap.RtvHeap()->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device.Dev()->GetDescriptorHandleIncrementSize(desHeap.Desc().Type);
		cmdList.List()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//	画面クリアコマンド
		// 3.画面クリア			R	　G		B	 A
		FLOAT clearColor[] = { 0.1, 0.25f, 0.5f, 0.0f }; // 青っぽい色
		cmdList.List()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.描画コマンドここから
		#pragma region DrawCommand
		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;		//	横幅
		viewport.Height = window_height;	//	縦幅
		viewport.TopLeftX = 0;				//	左上X
		viewport.TopLeftY = 0;				//	左上Y
		viewport.MinDepth = 0.0f;			//	最小深度
		viewport.MaxDepth = 1.0f;			//	最大深度
		// ビューポート設定コマンドを、コマンドリストに積む
		cmdList.List()->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		cmdList.List()->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		cmdList.List()->SetPipelineState(gPipeLine.state);
		cmdList.List()->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// プリミティブ形状の設定コマンド
		cmdList.List()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		cmdList.List()->IASetVertexBuffers(0, 1, &vertBuff.view);



		//	グラフィックスコマンド
		cmdList.List()->SetGraphicsRootConstantBufferView(0, cBuff.material->GetGPUVirtualAddress());	//	定数バッファビューの設定コマンド

		//	インデックスバッファビューの設定
		cmdList.List()->IASetIndexBuffer(&vertBuff.ibView);

		// 描画コマンド
		cmdList.List()->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画

		#pragma endregion DrawCommand
		// 4.描画コマンドここまで

		// 5.リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
		cmdList.List()->ResourceBarrier(1, &barrierDesc);

		// 命令のクローズ
		result = cmdList.List()->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { cmdList.List() };
		cmdQueue.Queue()->ExecuteCommandLists(1, commandLists);
		// 画面に表示するバッファをフリップ(裏表の入替え)
		result = swapChain.Chain()->Present(1, 0);
		assert(SUCCEEDED(result));

		//	画面入れ替え
		// コマンドの実行完了を待つ
		cmdQueue.Queue()->Signal(fence.Fnc(), ++fence.fenceVal);
		if (fence.Fnc()->GetCompletedValue() != fence.fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence.Fnc()->SetEventOnCompletion(fence.fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// キューをクリア
		result = cmdList.Allocator()->Reset();
		assert(SUCCEEDED(result));
		// 再びコマンドリストを貯める準備
		result = cmdList.List()->Reset(cmdList.Allocator(), nullptr);
		assert(SUCCEEDED(result));
	}

	return 0;
}

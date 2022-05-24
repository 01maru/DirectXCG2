#include "Window.h"
#include "DirectXInit.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "VertBuff.h"
#include "VertShader.h"
#include "GPipeline.h"
#include "Input.h"
using namespace DirectX;

#pragma comment(lib, "d3dcompiler.lib")

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	HRESULT result;
#pragma region Initialize
	Window win;

	DirectXInit dx(win.hwnd);

#pragma endregion Initialize

	//	描画初期化
	// 頂点データ
	XMFLOAT3 vertices[] = {
		{ -0.5f, -0.5f, 0.0f }, // 左下
		{ -0.5f, +0.5f, 0.0f }, // 左上
		{ +0.5f, -0.5f, 0.0f }, // 右下
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	VertBuff vertBuff(sizeVB, vertices, _countof(vertices), dx.device);

	VertShader vertShade;

	Input input(win.hwnd, win.w);

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか(今は最低限の３次元座標のみ))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, 
	};

	//	グラフィックスパイプライン
	GPipeline gPipeLine(vertShade, inputLayout, _countof(inputLayout), dx.device);

	//	ゲームループ
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();
		
		dx.DrawAble();

		// 3.画面クリア			R	　G		B	 A
		FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // 青っぽい色
		FLOAT clearColorR[] = { 1.0f,0.0f,0.0f,0.0f };
		if (input.GetKey(DIK_SPACE))
		{
			dx.ScreenClear(clearColorR);
		}
		else {
			dx.ScreenClear(clearColor);
		}

#pragma region Draw
		// 4.描画コマンドここから
		
		// ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		// ビューポート設定コマンドを、コマンドリストに積む
		dx.commandList->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // 切り抜き座標左
		scissorRect.right = scissorRect.left + window_width; // 切り抜き座標右
		scissorRect.top = 0; // 切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height; // 切り抜き座標下
		// シザー矩形設定コマンドを、コマンドリストに積む
		dx.commandList->RSSetScissorRects(1, &scissorRect);

		// パイプラインステートとルートシグネチャの設定コマンド
		dx.commandList->SetPipelineState(gPipeLine.state);
		dx.commandList->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// プリミティブ形状の設定コマンド
		dx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		// 頂点バッファビューの設定コマンド
		dx.commandList->IASetVertexBuffers(0, 1, &vertBuff.vbView);

		// 描画コマンド
		dx.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画

		// 4.描画コマンドここまで
#pragma endregion Draw

		dx.DrawEnd();
	}

	return 0;
}
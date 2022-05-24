#include "Window.h"
#include "DirectXInit.h"
#include "VertBuff.h"
#include "VertShader.h"
#include "GPipeline.h"
#include "Input.h"
#include "ViewPort.h"
#include "ScissorRect.h"

#include <DirectXMath.h>
using namespace DirectX;


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	HRESULT result;
#pragma region Initialize
	Window win;

	DirectXInit dx(win.hwnd);

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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	//	グラフィックスパイプライン
	GPipeline gPipeLine(vertShade, inputLayout, _countof(inputLayout), dx.device);

	//	ビューポート
	ViewPort viewPort(window_width, window_height, 0, 0);
	// シザー矩形
	ScissorRect scissorRect(0, window_width, 0, window_height);
#pragma endregion Initialize

	//	ゲームループ
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();
		
#pragma region DrawPrep
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
#pragma endregion	DrawPrep

#pragma region Draw
		// 4.描画コマンドここから
		viewPort.Update(dx.commandList);

		scissorRect.Update(dx.commandList);

		gPipeLine.Update(dx.commandList);

		// プリミティブ形状の設定コマンド
		dx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		vertBuff.Update(dx.commandList);

		// 描画コマンド
		dx.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // 全ての頂点を使って描画

		// 4.描画コマンドここまで
#pragma endregion Draw

		dx.DrawEnd();
	}

	return 0;
}
#include "Window.h"
#include "DirectX.h"
#include "Input.h"
#include "GPipeline.h"
#include "ViewPort.h"
#include "ScissorRect.h"
#include "MyMath.h"
#include "Object3D.h"

#include "VertBuff.h"
#include "ConstBuff.h"
#include "TextureData.h"

#include <DirectXMath.h>
using namespace DirectX;


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
#pragma region Initialize
	Window win;

	MyDirectX dx(win.hwnd);

	Input input(win.hwnd, win.w);

	//	描画初期化
#pragma region InputAssembler
	// 頂点データ
	Vertex vertices[] = {
		//	前
		{{ -5.0f,-5.0f,-5.0f },{},{0.0f, 1.0f}}, // 左下
		{{ -5.0f, 5.0f,-5.0f },{},{0.0f, 0.0f}}, // 左上
		{{  5.0f,-5.0f,-5.0f },{},{1.0f, 1.0f}}, // 右下
		{{  5.0f, 5.0f,-5.0f },{},{1.0f, 0.0f}}, // 右上
		//	後
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f, 5.0f, 5.0f },{}, {0.0f, 0.0f}}, // 左上
		{{  5.0f,-5.0f, 5.0f },{}, {1.0f, 1.0f}}, // 右下
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // 右上
		//	左
		{{ -5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // 左上
		{{ -5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // 右下
		{{ -5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // 右上
		//	右
		{{  5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // 左下
		{{  5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // 左上
		{{  5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // 右下
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // 右上
		//	下
		{{ -5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // 左上
		{{  5.0f,-5.0f,-5.0f },{}, {1.0f, 1.0f}}, // 右下
		{{  5.0f,-5.0f, 5.0f },{}, {1.0f, 0.0f}}, // 右上
		//	上
		{{ -5.0f, 5.0f,-5.0f },{}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f, 5.0f, 5.0f },{}, {0.0f, 0.0f}}, // 左上
		{{  5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // 右下
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // 右上
	};
	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//	インデックスデータ
	unsigned short indices[] =
	{
		0,1,2,
		2,1,3,
		6,5,4,
		7,5,6,
		8,9,10,
		10,9,11,
		14,13,12,
		15,13,14,
		18,17,16,
		19,17,18,
		20,21,22,
		22,21,23,
	};
	//	全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

#pragma region 06_03
	for (int i = 0; i < _countof(indices) / 3; i++)
	{
		unsigned short index0 = indices[i * 3];
		unsigned short index1 = indices[i * 3 + 1];
		unsigned short index2 = indices[i * 3 + 2];

		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		XMVECTOR normal = XMVectorSubtract(v1, v2);

		normal = XMVector3Normalize(normal);

		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}
#pragma endregion

#pragma endregion

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか)
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	法線ベクトル
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

	VertBuff vertBuff(sizeVB, vertices, _countof(vertices), sizeIB, indices, _countof(indices), dx.device);

	TextureDeta texture(dx.device, vertBuff.resDesc);

	//	グラフィックスパイプライン
	GPipeline gPipeLine(inputLayout, _countof(inputLayout), dx.device);

	//	ビューポート
	ViewPort viewPort(window_width, window_height, 0, 0);
	// シザー矩形
	ScissorRect scissorRect(0, window_width, 0, window_height);

	//	定数バッファ
	ConstBuff cBuff(dx.device, window_width, window_height);

	//	objの動きに用
	Vector3D pos;
	float rot = 0.0f;

	Object3D obj(dx.device);
	Object3D obj2(dx.device);

	MatView matView(Vector3D(0.0f, 0.0f, -100.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	Matrix matProjection = PerspectiveFovLH(window_width, window_height, ConvertToRad(45.0f), 0.1f, 1000.0f);
#pragma endregion Initialize

	//	ゲームループ
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();

#pragma region Update
		pos.z += input.GetKey(DIK_UP) - input.GetKey(DIK_DOWN);
		pos.x += input.GetKey(DIK_RIGHT) - input.GetKey(DIK_LEFT);
		rot += (input.GetKey(DIK_Q) - input.GetKey(DIK_E)) * PI / 4;


		obj.trans = pos;
		obj2.rotAngle.y = rot;

		//	定数バッファに転送
		obj.Update(matView.mat, matProjection);
		obj2.Update(matView.mat, matProjection);
#pragma endregion

#pragma region Draw
		// 3.画面クリア			R	　G		 B	   A
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // 青っぽい色
		dx.DrawAble(clearColor);

		// 4.描画コマンドここから
		viewPort.Update(dx.cmdList);

		scissorRect.Update(dx.cmdList);

		gPipeLine.Update(dx.cmdList);

		// プリミティブ形状の設定コマンド
		dx.cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

		vertBuff.Update(dx.cmdList);

		cBuff.Update(dx.cmdList);

		texture.Update(dx.cmdList);

		obj.Draw(dx.cmdList, _countof(indices));
		obj2.Draw(dx.cmdList, _countof(indices));

		// 4.描画コマンドここまで
		
		dx.DrawEnd();
#pragma endregion Draw
	}

	return 0;
}
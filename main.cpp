#include "Window.h"
#include "DirectX.h"
#include "Input.h"
#include "InputJoypad.h"
#include "GPipeline.h"
#include "ViewPort.h"
#include "ScissorRect.h"
#include "MyMath.h"
#include "Object3D.h"
#include "MyDxSound.h"

#include "VertBuff.h"
#include "ConstBuff.h"
#include "TextureData.h"
#include "Shader.h"
#include "MyDebugCamera.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
#pragma region Initialize
	Window win;
	MyDirectX dx(win.hwnd);

	MyDebugCamera debugcamera(Vector3D(0.0f, 0.0f, -100.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));

	DxSound sound(win);
	sound.CreateSoundBuff("Resource\\fanfare.wav");

	Input input(win.hwnd, win.w);
	InputJoypad joypad;

	Shader shader(L"BasicVS.hlsl", L"BasicPS.hlsl");
	Shader screenShader(L"VShader.hlsl", L"PShader.hlsl");
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

#pragma region Screen
	ScreenVertex pv[4] = {
							{ {-1.0f,-1.0f,0.1f},{0,1}},
							{ {-1.0f,1.0f,0.1f},{0,0}},
							{ {1.0f,-1.0f,0.1f},{1,1}},
							{ {1.0f,1.0f,0.1f},{1,0}}, };
#pragma endregion
#pragma endregion

	// 頂点レイアウト(頂点一つ分のデータに何を持たせるか)
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	法線ベクトル
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz座標
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv座標
	};

	VertBuff vertBuff(dx.Dev(), sizeVB, vertices, _countof(vertices), sizeIB, indices, _countof(indices));
	UINT sizePV = static_cast<UINT>(sizeof(pv[0]) * _countof(pv));
	VertBuff screenVBuff(dx.Dev(), sizePV, pv, _countof(pv));

	Texture texture[2];
	texture[0].LoadTexture(L"Resource/texture.jpg");
	texture[1].LoadTexture(L"Resource/reimu.png");
	TextureDeta textureDeta(dx.Dev(), vertBuff.ResDesc(), texture);

	//	グラフィックスパイプライン
	GPipeline gPipeLine(dx.Dev(), shader, inputLayout, _countof(inputLayout));
	GPipeline screenPipeline(layout, _countof(layout), dx.Dev(), screenShader);

	//	ビューポート
	ViewPort viewPort(window_width, window_height, 0, 0);
	// シザー矩形
	ScissorRect scissorRect(0, window_width, 0, window_height);

	//	定数バッファ
	ConstBuff cBuff(dx.Dev(), window_width, window_height);

	//	objの動きに用
	Vector3D pos;
	float rot = 0.0f;

	Object3D obj(dx.Dev());
	Object3D obj2(dx.Dev());

	//MyMath::MatView matView(Vector3D(0.0f, 0.0f, -100.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	Matrix matProjection = MyMath::PerspectiveFovLH(window_width, window_height, MyMath::ConvertToRad(45.0f), 0.1f, 1000.0f);
#pragma endregion Initialize

	//	ゲームループ
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update(win.hwnd);
		joypad.Update();


#pragma region Update
		pos.z += input.GetKey(DIK_A) - input.GetKey(DIK_D);
		pos.x += input.GetKey(DIK_W) - input.GetKey(DIK_S);
		rot += (input.GetKey(DIK_Q) - input.GetKey(DIK_E)) * MyMath::PI / 4;
		obj.trans = pos;
		obj2.rotAngle.y = rot;

		debugcamera.Update(input);
		//	定数バッファに転送
		obj.Update(debugcamera.mat, matProjection);
		obj2.Update(debugcamera.mat, matProjection);

		if (input.GetTrigger(DIK_SPACE)) {
			textureDeta.textureNum++;

			if (textureDeta.textureNum >= 2)	textureDeta.textureNum = 0;
		}
#pragma endregion

#pragma region Draw
		dx.DrawAbleScreenTexture();

		// 描画コマンド
		viewPort.Update(dx.CmdList());

		scissorRect.Update(dx.CmdList());

		gPipeLine.Setting(dx.CmdList());

		gPipeLine.Update(dx.CmdList());

		vertBuff.Update(dx.CmdList());

		cBuff.Update(dx.CmdList());

		textureDeta.Update(dx.CmdList());

		obj.Draw(dx.CmdList(), _countof(indices));
		obj2.Draw(dx.CmdList(), _countof(indices));

		// 描画コマンド
		dx.DrawEndScreenTexture();

		// 3.画面クリア			R	　G		 B	   A
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // 青っぽい色
		dx.DrawAble(clearColor);

		viewPort.Update(dx.CmdList());

		scissorRect.Update(dx.CmdList());

		screenVBuff.Update(dx.CmdList());

		screenPipeline.Setting(dx.CmdList());

		dx.Setting();
		
		screenPipeline.Update(dx.CmdList(), D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		dx.CmdList()->DrawInstanced(4, 1, 0, 0);

		dx.DrawEnd();
#pragma endregion Draw
	}

	return 0;
}
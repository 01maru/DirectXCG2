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
#include "Shader.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
#pragma region Initialize
	Window win;
	MyDirectX dx(win.hwnd);

	Input input(win.hwnd, win.w);

	Shader shader(L"BasicVS.hlsl", L"BasicPS.hlsl");
	Shader screenShader(L"VShader.hlsl", L"PShader.hlsl");
	//	�`�揉����
#pragma region InputAssembler
	// ���_�f�[�^
	Vertex vertices[] = {
		//	�O
		{{ -5.0f,-5.0f,-5.0f },{},{0.0f, 1.0f}}, // ����
		{{ -5.0f, 5.0f,-5.0f },{},{0.0f, 0.0f}}, // ����
		{{  5.0f,-5.0f,-5.0f },{},{1.0f, 1.0f}}, // �E��
		{{  5.0f, 5.0f,-5.0f },{},{1.0f, 0.0f}}, // �E��
		//	��
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 1.0f}}, // ����
		{{ -5.0f, 5.0f, 5.0f },{}, {0.0f, 0.0f}}, // ����
		{{  5.0f,-5.0f, 5.0f },{}, {1.0f, 1.0f}}, // �E��
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // �E��
		//	��
		{{ -5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // ����
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // ����
		{{ -5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // �E��
		{{ -5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // �E��
		//	�E
		{{  5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // ����
		{{  5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // ����
		{{  5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // �E��
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // �E��
		//	��
		{{ -5.0f,-5.0f,-5.0f },{}, {0.0f, 1.0f}}, // ����
		{{ -5.0f,-5.0f, 5.0f },{}, {0.0f, 0.0f}}, // ����
		{{  5.0f,-5.0f,-5.0f },{}, {1.0f, 1.0f}}, // �E��
		{{  5.0f,-5.0f, 5.0f },{}, {1.0f, 0.0f}}, // �E��
		//	��
		{{ -5.0f, 5.0f,-5.0f },{}, {0.0f, 1.0f}}, // ����
		{{ -5.0f, 5.0f, 5.0f },{}, {0.0f, 0.0f}}, // ����
		{{  5.0f, 5.0f,-5.0f },{}, {1.0f, 1.0f}}, // �E��
		{{  5.0f, 5.0f, 5.0f },{}, {1.0f, 0.0f}}, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//	�C���f�b�N�X�f�[�^
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
	//	�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

#pragma region Screen
	ScreenVertex pv[4] = {
							{ {-1.0f,-1.0f,0.1f},{0,1}},
							{ {-1.0f,1.0f,0.1f},{0,0}},
							{ {1.0f,-1.0f,0.1f},{1,1}},
							{ {1.0f,1.0f,0.1f},{1,0}}, };
#pragma endregion
#pragma endregion

	// ���_���C�A�E�g(���_����̃f�[�^�ɉ����������邩)
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz���W
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	�@���x�N�g��
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv���W
	};

	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},		//	xyz���W
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},				//	uv���W
	};

	VertBuff vertBuff(dx.Dev(), sizeVB, vertices, _countof(vertices), sizeIB, indices, _countof(indices));
	UINT sizePV = static_cast<UINT>(sizeof(pv[0]) * _countof(pv));
	VertBuff screenVBuff(dx.Dev(), sizePV, pv, _countof(pv));

	Texture texture[2];
	texture[0].LoadTexture(L"Resource/texture.jpg");
	texture[1].LoadTexture(L"Resource/reimu.png");
	TextureDeta textureDeta(dx.Dev(), vertBuff.ResDesc(), texture);

	//	�O���t�B�b�N�X�p�C�v���C��
	GPipeline gPipeLine(dx.Dev(), shader, inputLayout, _countof(inputLayout));
	GPipeline screenPipeline(layout, _countof(layout), dx.Dev(), screenShader);

	//	�r���[�|�[�g
	ViewPort viewPort(window_width, window_height, 0, 0);
	// �V�U�[��`
	ScissorRect scissorRect(0, window_width, 0, window_height);

	//	�萔�o�b�t�@
	ConstBuff cBuff(dx.Dev(), window_width, window_height);

	//	obj�̓����ɗp
	Vector3D pos;
	float rot = 0.0f;

	Object3D obj(dx.Dev());
	Object3D obj2(dx.Dev());

	MyMath::MatView matView(Vector3D(0.0f, 0.0f, -100.0f), Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	Matrix matProjection = MyMath::PerspectiveFovLH(window_width, window_height, MyMath::ConvertToRad(45.0f), 0.1f, 1000.0f);
#pragma endregion Initialize

	//	�Q�[�����[�v
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();

#pragma region Update
		pos.z += input.GetKey(DIK_UP) - input.GetKey(DIK_DOWN);
		pos.x += input.Click(LeftClick) - input.Click(RightClick);
		rot += (input.GetKey(DIK_Q) - input.GetKey(DIK_E)) * MyMath::PI / 4;


		obj.trans = pos;
		obj2.rotAngle.y = rot;

		//	�萔�o�b�t�@�ɓ]��
		obj.Update(matView.mat, matProjection);
		obj2.Update(matView.mat, matProjection);

		if (input.GetTrigger(DIK_SPACE)) {
			textureDeta.textureNum++;

			if (textureDeta.textureNum >= 2)	textureDeta.textureNum = 0;
		}
#pragma endregion

#pragma region Draw
		dx.DrawAbleScreenTexture();

		// �`��R�}���h
		viewPort.Update(dx.CmdList());

		scissorRect.Update(dx.CmdList());

		gPipeLine.Setting(dx.CmdList());

		gPipeLine.Update(dx.CmdList());

		vertBuff.Update(dx.CmdList());

		cBuff.Update(dx.CmdList());

		textureDeta.Update(dx.CmdList());

		obj.Draw(dx.CmdList(), _countof(indices));
		obj2.Draw(dx.CmdList(), _countof(indices));

		// �`��R�}���h
		dx.DrawEndScreenTexture();

		// 3.��ʃN���A			R	�@G		 B	   A
		FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // ���ۂ��F
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
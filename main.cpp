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

	//	�`�揉����
	// ���_�f�[�^
	XMFLOAT3 vertices[] = {
		{ -0.5f, -0.5f, 0.0f }, // ����
		{ -0.5f, +0.5f, 0.0f }, // ����
		{ +0.5f, -0.5f, 0.0f }, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	VertBuff vertBuff(sizeVB, vertices, _countof(vertices), dx.device);

	VertShader vertShade;

	Input input(win.hwnd, win.w);

	// ���_���C�A�E�g(���_����̃f�[�^�ɉ����������邩(���͍Œ���̂R�������W�̂�))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	//	�O���t�B�b�N�X�p�C�v���C��
	GPipeline gPipeLine(vertShade, inputLayout, _countof(inputLayout), dx.device);

	//	�r���[�|�[�g
	ViewPort viewPort(window_width, window_height, 0, 0);
	// �V�U�[��`
	ScissorRect scissorRect(0, window_width, 0, window_height);
#pragma endregion Initialize

	//	�Q�[�����[�v
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();
		
#pragma region DrawPrep
		dx.DrawAble();

		// 3.��ʃN���A			R	�@G		B	 A
		FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // ���ۂ��F
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
		// 4.�`��R�}���h��������
		viewPort.Update(dx.commandList);

		scissorRect.Update(dx.commandList);

		gPipeLine.Update(dx.commandList);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		dx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		vertBuff.Update(dx.commandList);

		// �`��R�}���h
		dx.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��

		// 4.�`��R�}���h�����܂�
#pragma endregion Draw

		dx.DrawEnd();
	}

	return 0;
}
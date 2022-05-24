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
	{
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, 
	};

	//	�O���t�B�b�N�X�p�C�v���C��
	GPipeline gPipeLine(vertShade, inputLayout, _countof(inputLayout), dx.device);

	//	�Q�[�����[�v
	while (true)
	{
#pragma region  WinMsg
		win.MsgUpdate();
		if (win.EndLoop()) { break; }
#pragma endregion

		input.Update();
		
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

#pragma region Draw
		// 4.�`��R�}���h��������
		
		// �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		dx.commandList->RSSetViewports(1, &viewport);

		// �V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // �؂蔲�����W��
		scissorRect.right = scissorRect.left + window_width; // �؂蔲�����W�E
		scissorRect.top = 0; // �؂蔲�����W��
		scissorRect.bottom = scissorRect.top + window_height; // �؂蔲�����W��
		// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		dx.commandList->RSSetScissorRects(1, &scissorRect);

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		dx.commandList->SetPipelineState(gPipeLine.state);
		dx.commandList->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		dx.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		dx.commandList->IASetVertexBuffers(0, 1, &vertBuff.vbView);

		// �`��R�}���h
		dx.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��

		// 4.�`��R�}���h�����܂�
#pragma endregion Draw

		dx.DrawEnd();
	}

	return 0;
}
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

	//	������
	HRESULT result;

	//	�A�_�v�^�[��(�����)
	EnumAdapter adapter;

#ifdef _DEBUG
	//	�f�o�b�O���C���[���I��
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG

	Device device(adapter.TmpAdapter());

	//	�R�}���h���X�g
	CmdList cmdList(device.Dev());

	//�R�}���h�L���[
	CmdQueue cmdQueue(device.Dev());

	//	�X���b�v�`�F�[��(�_�u���o�b�t�@�����O�p)
	SwapChain swapChain(adapter.DxgiFactory(), cmdQueue.Queue(), win.hwnd);

	// �f�X�N���v�^�q�[�v�̐ݒ�(GPU�Ƀ������̈�m�ۂ����ꂩ��g��)
	DesHeap desHeap(swapChain.Desc(), device.Dev());

	// �o�b�N�o�b�t�@
	BackBuffer buffer(swapChain, desHeap, device.Dev());

	// �t�F���X�̐���
	Fence fence(device.Dev());

	//	�L�[����
	Input input(win.hwnd, win.w);

	//	�`�揉����
	// ���_�f�[�^
	Vertex vertices[] = {
		{{ -0.5f, -0.5f, 0.0f }, {0.0f, 1.0f}}, // ����
		{{ -0.5f, +0.5f, 0.0f }, {0.0f, 0.0f}}, // ����
		{{ +0.5f, -0.5f, 0.0f }, {1.0f, 1.0f}}, // �E��
		{{ +0.5f, +0.5f, 0.0f }, {1.0f, 0.0f}}, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	//	�C���f�b�N�X�f�[�^
	uint16_t indices[] =
	{
		0,1,2,
		1,2,3,
	};
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// ���_�o�b�t�@
	VertexBuffer vertBuff(sizeVB, device.Dev(), vertices, _countof(vertices), sizeIB, indices, _countof(indices));

	//	���_�V�F�[�_�t�@�C���ǂݍ��݁��R���p�C��
	VertexShader vertShade;

	// ���_���C�A�E�g(���_����̃f�[�^�ɉ����������邩(���͍Œ���̂R�������W�̂�))
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

	//	�O���t�B�b�N�X�p�C�v���C��
	GraphicsPipeLine gPipeLine(vertShade, inputLayout, _countof(inputLayout), device.Dev());

	//	�萔�o�b�t�@
	ConstBuffer cBuff(device.Dev());

	#pragma endregion Initialize

	//	�Q�[�����[�v
	while (true)
	{
		//	�E�B���h�E���b�Z�[�W����
		win.MessageUpdate();
		if (win.EndLoop()) { break; }

		//	DirectX���t���[������
		//	�L�[���擾
		input.Update();
		
		//	�����[�X�o���A
		// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
		UINT bbIndex = swapChain.Chain()->GetCurrentBackBufferIndex();
		// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = buffer.BackBuff()[bbIndex]; // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
		cmdList.List()->ResourceBarrier(1, &barrierDesc);

		//	�`���w��R�}���h
		// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = desHeap.RtvHeap()->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device.Dev()->GetDescriptorHandleIncrementSize(desHeap.Desc().Type);
		cmdList.List()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//	��ʃN���A�R�}���h
		// 3.��ʃN���A			R	�@G		B	 A
		FLOAT clearColor[] = { 0.1, 0.25f, 0.5f, 0.0f }; // ���ۂ��F
		cmdList.List()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.�`��R�}���h��������
		#pragma region DrawCommand
		// �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;		//	����
		viewport.Height = window_height;	//	�c��
		viewport.TopLeftX = 0;				//	����X
		viewport.TopLeftY = 0;				//	����Y
		viewport.MinDepth = 0.0f;			//	�ŏ��[�x
		viewport.MaxDepth = 1.0f;			//	�ő�[�x
		// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		cmdList.List()->RSSetViewports(1, &viewport);

		// �V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // �؂蔲�����W��
		scissorRect.right = scissorRect.left + window_width; // �؂蔲�����W�E
		scissorRect.top = 0; // �؂蔲�����W��
		scissorRect.bottom = scissorRect.top + window_height; // �؂蔲�����W��
		// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		cmdList.List()->RSSetScissorRects(1, &scissorRect);

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		cmdList.List()->SetPipelineState(gPipeLine.state);
		cmdList.List()->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		cmdList.List()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		cmdList.List()->IASetVertexBuffers(0, 1, &vertBuff.view);



		//	�O���t�B�b�N�X�R�}���h
		cmdList.List()->SetGraphicsRootConstantBufferView(0, cBuff.material->GetGPUVirtualAddress());	//	�萔�o�b�t�@�r���[�̐ݒ�R�}���h

		//	�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
		cmdList.List()->IASetIndexBuffer(&vertBuff.ibView);

		// �`��R�}���h
		cmdList.List()->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��

		#pragma endregion DrawCommand
		// 4.�`��R�}���h�����܂�

		// 5.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
		cmdList.List()->ResourceBarrier(1, &barrierDesc);

		// ���߂̃N���[�Y
		result = cmdList.List()->Close();
		assert(SUCCEEDED(result));
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { cmdList.List() };
		cmdQueue.Queue()->ExecuteCommandLists(1, commandLists);
		// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
		result = swapChain.Chain()->Present(1, 0);
		assert(SUCCEEDED(result));

		//	��ʓ���ւ�
		// �R�}���h�̎��s������҂�
		cmdQueue.Queue()->Signal(fence.Fnc(), ++fence.fenceVal);
		if (fence.Fnc()->GetCompletedValue() != fence.fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence.Fnc()->SetEventOnCompletion(fence.fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// �L���[���N���A
		result = cmdList.Allocator()->Reset();
		assert(SUCCEEDED(result));
		// �ĂуR�}���h���X�g�𒙂߂鏀��
		result = cmdList.List()->Reset(cmdList.Allocator(), nullptr);
		assert(SUCCEEDED(result));
	}

	return 0;
}

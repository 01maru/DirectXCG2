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

	//	������
	HRESULT result;

	EnumAdapter adapter;

#ifdef _DEBUG
	//	�f�o�b�O���C���[���I��
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG

	Device device(adapter.tmpAdapter);

	//	�R�}���h���X�g
	CmdList cmdList(device.dev);

	//�R�}���h�L���[
	CmdQueue cmdQueue(device.dev);

	//	�X���b�v�`�F�[��(�_�u���o�b�t�@�����O�p)
	SwapChain swapChain(adapter.dxgiFactory, cmdQueue.commandQueue, win.hwnd);

	// �f�X�N���v�^�q�[�v�̐ݒ�(GPU�Ƀ������̈�m�ۂ����ꂩ��g��)
	DesHeap desHeap(swapChain.swapChainDesc, device.dev);

	// �o�b�N�o�b�t�@
	Buffer buffer(swapChain, desHeap, device.dev);

	// �t�F���X�̐���
	Fence fence(device.dev);

	//	�L�[����
	Input input(win.hwnd, win.w);

	//	�`�揉����
	// ���_�f�[�^
	XMFLOAT3 vertices[] = {
	{ -0.5f, +0.5f, 0.0f }, // ����
	{ +0.5f, +0.5f, 0.0f }, // �E��
	{ -0.5f, 0.0f, 0.0f },	// ��
	{ +0.5f, 0.0f, 0.0f },	// �E
	{ -0.5f, -0.5f, 0.0f }, // ����
	{ +0.5f, -0.5f, 0.0f }, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));
	//	�C���f�b�N�X�f�[�^
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

	// ���_�o�b�t�@
	VertexBuffer vertBuff(sizeVB, device.dev, vertices, _countof(vertices), sizeIB, indices, _countof(indices));

	//	���_�V�F�[�_�t�@�C���ǂݍ��݁��R���p�C��
	VertexShader vertShade;

	// ���_���C�A�E�g(���_����̃f�[�^�ɉ����������邩(���͍Œ���̂R�������W�̂�))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//	���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	�萔�o�b�t�@�r���[
	rootParam.Descriptor.ShaderRegister = 0;					//	�萔�o�b�t�@�ԍ�
	rootParam.Descriptor.RegisterSpace = 0;						//	�f�t�H���g�l
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	���ׂẴV�F�[�_���猩��

	//	�O���t�B�b�N�X�p�C�v���C��
	GraphicsPipeLine gPipeLine(vertShade, inputLayout, _countof(inputLayout), device.dev, rootParam);

	//	�萔�o�b�t�@
	ConstBuffer cBuff(device.dev);


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
		UINT bbIndex = swapChain.swapChain->GetCurrentBackBufferIndex();
		// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = buffer.backBuffers[bbIndex]; // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
		cmdList.commandList->ResourceBarrier(1, &barrierDesc);

		//	�`���w��R�}���h
		// 2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = desHeap.rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device.dev->GetDescriptorHandleIncrementSize(desHeap.rtvHeapDesc.Type);
		cmdList.commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//	��ʃN���A�R�}���h
		// 3.��ʃN���A			R	�@G		B	 A
		FLOAT clearColor[] = { 0.1, 0.25f, 0.5f, 0.0f }; // ���ۂ��F
		cmdList.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// 4.�`��R�}���h��������
		
		// �r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = window_width;		//	����
		viewport.Height = window_height;	//	�c��
		viewport.TopLeftX = 0;				//	����X
		viewport.TopLeftY = 0;				//	����Y
		viewport.MinDepth = 0.0f;			//	�ŏ��[�x
		viewport.MaxDepth = 1.0f;			//	�ő�[�x
		// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		cmdList.commandList->RSSetViewports(1, &viewport);

		// �V�U�[��`
		D3D12_RECT scissorRect{};
		scissorRect.left = 0; // �؂蔲�����W��
		scissorRect.right = scissorRect.left + window_width; // �؂蔲�����W�E
		scissorRect.top = 0; // �؂蔲�����W��
		scissorRect.bottom = scissorRect.top + window_height; // �؂蔲�����W��
		// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		cmdList.commandList->RSSetScissorRects(1, &scissorRect);

		// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		cmdList.commandList->SetPipelineState(gPipeLine.state);
		cmdList.commandList->SetGraphicsRootSignature(gPipeLine.rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		cmdList.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		cmdList.commandList->IASetVertexBuffers(0, 1, &vertBuff.view);



		//	�O���t�B�b�N�X�R�}���h
		cmdList.commandList->SetGraphicsRootConstantBufferView(0, cBuff.material->GetGPUVirtualAddress());	//	�萔�o�b�t�@�r���[�̐ݒ�R�}���h

		//	�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
		cmdList.commandList->IASetIndexBuffer(&vertBuff.ibView);

		// �`��R�}���h
		cmdList.commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // �S�Ă̒��_���g���ĕ`��

		// 4.�`��R�}���h�����܂�

		// 5.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
		cmdList.commandList->ResourceBarrier(1, &barrierDesc);

		// ���߂̃N���[�Y
		result = cmdList.commandList->Close();
		assert(SUCCEEDED(result));
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { cmdList.commandList };
		cmdQueue.commandQueue->ExecuteCommandLists(1, commandLists);
		// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
		result = swapChain.swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//	��ʓ���ւ�
		// �R�}���h�̎��s������҂�
		cmdQueue.commandQueue->Signal(fence.fnc, ++fence.fenceVal);
		if (fence.fnc->GetCompletedValue() != fence.fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence.fnc->SetEventOnCompletion(fence.fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		// �L���[���N���A
		result = cmdList.commandAllocator->Reset();
		assert(SUCCEEDED(result));
		// �ĂуR�}���h���X�g�𒙂߂鏀��
		result = cmdList.commandList->Reset(cmdList.commandAllocator, nullptr);
		assert(SUCCEEDED(result));
	}

	UnregisterClass(win.w.lpszClassName, win.w.hInstance);

	return 0;
}

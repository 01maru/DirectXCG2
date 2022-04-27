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
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:		//	�E�B���h�E������ꂽ
	case WM_DESTROY:	//	�E�B���h�E���j�����ꂽ
		PostQuitMessage(0);
		return 0;
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Window win((WNDPROC)WindowProc);

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
	{ -0.5f, -0.5f, 0.0f }, // ����
	{ -0.5f, +0.5f, 0.0f }, // ����
	{ +0.5f, -0.5f, 0.0f }, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	// ���_�o�b�t�@
	VertexBuffer vertBuff(sizeVB, device.dev, vertices, _countof(vertices));

	//	���_�V�F�[�_�t�@�C���ǂݍ��݁��R���p�C��
	VertexShader vertShade;

	// ���_���C�A�E�g(���_����̃f�[�^�ɉ����������邩(���͍Œ���̂R�������W�̂�))
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION",
	0,
	DXGI_FORMAT_R32G32B32_FLOAT,
	0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	0
	}, 
	};

	//	�O���t�B�b�N�X�p�C�v���C��
	// �O���t�B�b�N�X�p�C�v���C���\���̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vertShade.vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vertShade.vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = vertShade.psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = vertShade.psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�or���C���[�t���[��
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//	RGBA���ׂẴ`�����l���`��

	//blenddesc.BlendEnable = true;					//	�L����
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	���Z
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	�\�[�X�̒l��100���g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//	�g��Ȃ�

	////	���Z����
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	���Z
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	�\�[�X�̒l��100���g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;		//	�f�X�g�̒l��100���g��
	////	���Z����
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;	//	���Z
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;				//	�\�[�X�̒l��100���g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;				//	�f�X�g�̒l��100���g��
	////	�F���]
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	���Z
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_COLOR;	//	1.0f-�f�X�g�J���[�̒l���g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;			//	�g��Ȃ�
	////	����������
	//blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	���Z
	//blenddesc.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;		//	�\�[�X�̃��l���g��
	//blenddesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	//	1.0f-�\�[�X�̃��l���g��


	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// ���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &vertShade.errorBlob);
	assert(SUCCEEDED(result));
	result = device.dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	// �p�C�v�����X�e�[�g�̐���
	ID3D12PipelineState* pipelineState = nullptr;
	result = device.dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

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
		cmdList.commandList->SetPipelineState(pipelineState);
		cmdList.commandList->SetGraphicsRootSignature(rootSignature);

		// �v���~�e�B�u�`��̐ݒ�R�}���h
		cmdList.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

		// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
		cmdList.commandList->IASetVertexBuffers(0, 1, &vertBuff.view);

		// �`��R�}���h
		cmdList.commandList->DrawInstanced(_countof(vertices), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��

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
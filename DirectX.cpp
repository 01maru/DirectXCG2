#include "DirectX.h"
#include "Window.h"

// �Ή����x���̔z��
D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
};

MyDirectX::MyDirectX(HWND hwnd)
{
	DebugLayer();

#pragma region GPU��
#pragma region Adapter
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));
	// �A�_�v�^�[�̗񋓗p
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter4>> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	Microsoft::WRL::ComPtr<IDXGIAdapter4> tmpAdapter;
	// �p�t�H�[�}���X���������̂��珇�ɁA�S�ẴA�_�v�^�[��񋓂���
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		// ���I�z��ɒǉ�����
		adapters.push_back(tmpAdapter);
	}

	//	�O���{�A�_�v�^�I��
	// �Ó��ȃA�_�v�^��I�ʂ���
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		// �A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);
		// �\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// �f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
#pragma endregion Adapter

#pragma region Device
	//	�f�o�C�X����
	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++) {
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK) {
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion Device
#pragma endregion

#pragma region CmdList
	// �R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�𐶐�
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));
	assert(SUCCEEDED(result));
#pragma endregion CmdList

#pragma region CmdQueue
	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(SUCCEEDED(result));
#pragma endregion CmdQueue

#pragma region swapChain
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	// �ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;										//	��ʕ��𑜓x
	swapChainDesc.Height = 720;										//	��ʍ����𑜓x
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// �F���̏���
	swapChainDesc.SampleDesc.Count = 1;								// �}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;				// �o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2;									// �o�b�t�@����2�ɐݒ�(�\�Ɨ�)
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// �t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// �X���b�v�`�F�[���̐���
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr,
		&swapChain1);

	swapChain1.As(&swapChain);		//	1��4�ɕϊ�
	assert(SUCCEEDED(result));
#pragma endregion swapChain

#pragma region DesHeap
	// �f�X�N���v�^�q�[�v�̐ݒ�(GPU�Ƀ������̈�m�ۂ����ꂩ��g��)
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount; // ���\��2��
	// �f�X�N���v�^�q�[�v�̐���
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
#pragma endregion DesHeap

#pragma region BackBuff
	backBuffers.resize(swapChainDesc.BufferCount);
#pragma endregion BackBuff

#pragma region RTV
	// �X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers.size(); i++) {
		// �X���b�v�`�F�[������o�b�t�@���擾
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾(�擪�A�h���X)
		rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		// �����\���ŃA�h���X�������
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}
#pragma endregion RTV

#pragma region �[�x�o�b�t�@
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = window_width;
	depthResourceDesc.Height = window_height;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	//	�[�x�n�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//	�[�x�n�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	//	Resource����
	ID3D12Resource* depthBuff = nullptr;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));
	//	�f�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	//	view
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion

#pragma region fence
	// �t�F���X�̐���
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
#pragma endregion fence
}

void MyDirectX::DrawAble(FLOAT* clearColor)
{
	// 1.���\�[�X�o���A�ŏ������݉\�ɕύX
#pragma region ReleaseBarrier
	// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	UINT bbIndex = swapChain->GetCurrentBackBufferIndex();		//	���݂̃o�b�N�o�b�t�@�ݒ�
	barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
	cmdList->ResourceBarrier(1, &barrierDesc);
#pragma endregion ReleaseBarrier
	
	// 2.�`���̕ύX
#pragma region Change
	// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
	dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion Change

	// 3.��ʃN���A
#pragma region ScreenClear
	if (clearColor == nullptr) {
		ScreenClear();
	}
	else {
		ScreenClear(clearColor);
	}
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#pragma endregion
}

void MyDirectX::DebugLayer()
{
#ifdef _DEBUG
	//	�I��
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif
}

void MyDirectX::DrawEnd()
{
	// 5.���\�[�X�o���A��߂�
#pragma region ReleaseBarrier
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;	// �`���Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;			// �\����Ԃ�
	cmdList->ResourceBarrier(1, &barrierDesc);
#pragma endregion ReleaseBarrier

	// ���߂̃N���[�Y
#pragma region CmdClose
	result = cmdList->Close();
	assert(SUCCEEDED(result));
	// ���߂Ă����R�}���h���X�g�̎��s(close�K�{)
	ID3D12CommandList* commandLists[] = { cmdList.Get()};
	cmdQueue->ExecuteCommandLists(1, commandLists);
	// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
	result = swapChain->Present(1, 0);
	assert(SUCCEEDED(result));
#pragma endregion CmdClose

#pragma region ChangeScreen
	// �R�}���h�̎��s������҂�
	cmdQueue->Signal(fence, ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)	//	GPU�̏�������������������
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	// �L���[���N���A
	result = cmdAllocator->Reset();
	assert(SUCCEEDED(result));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result = cmdList->Reset(cmdAllocator.Get(), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion ChangeScreen
}

void MyDirectX::ScreenClear(FLOAT* clearColor)
{
	cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void MyDirectX::ScreenClear()
{
	FLOAT clearColor[] = { 0.1f,0.25f, 0.5f,0.0f };
	cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

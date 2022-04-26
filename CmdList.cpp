#include "CmdList.h"

CmdList::CmdList(ID3D12Device* dev)
{
	// �R�}���h�A���P�[�^�𐶐�
	HRESULT result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));
}

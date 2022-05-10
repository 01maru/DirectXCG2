#include "CmdQueue.h"

CmdQueue::CmdQueue(ID3D12Device* dev)
{
	//�R�}���h�L���[�𐶐�
	HRESULT result = dev->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
}

ID3D12CommandQueue* CmdQueue::Queue()
{
	return commandQueue;
}

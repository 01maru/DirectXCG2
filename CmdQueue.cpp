#include "CmdQueue.h"

CmdQueue::CmdQueue(ID3D12Device* dev)
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	HRESULT result =dev->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
}

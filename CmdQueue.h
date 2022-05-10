#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class CmdQueue
{
private:
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	ID3D12CommandQueue* commandQueue = nullptr;

public:
	CmdQueue(ID3D12Device* dev);

	ID3D12CommandQueue* Queue();
};


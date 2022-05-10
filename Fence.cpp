#include "Fence.h"

Fence::Fence(ID3D12Device* dev)
{
	HRESULT result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fnc));
}

ID3D12Fence* Fence::Fnc()
{
	return fnc;
}

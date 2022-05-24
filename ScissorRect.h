#pragma once
#include <d3d12.h>

class ScissorRect
{
private:
	D3D12_RECT scissorRect{};
public:
	ScissorRect(const int left, const int width, const int top, const int height);
	void Update(ID3D12GraphicsCommandList* cmdList);
};


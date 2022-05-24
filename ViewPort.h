#pragma once
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

class ViewPort
{
private:
	D3D12_VIEWPORT viewport{};
public:
	ViewPort(const int width, const int height, const int topLftX, const int topLftY, const float MinDepth = 0.0f, const float MaxDepth = 1.0f);
	void Update(ID3D12GraphicsCommandList* cmdList);
};


#include "ViewPort.h"

ViewPort::ViewPort(const int width, const int height, const int topLftX, const int topLftY, const float MinDepth, const float MaxDepth)
{
	viewport.Width = width;
	viewport.Height = height;
	viewport.TopLeftX = topLftX;
	viewport.TopLeftY = topLftY;
	viewport.MinDepth = MinDepth;
	viewport.MaxDepth = MaxDepth;
}

void ViewPort::Update(ID3D12GraphicsCommandList* cmdList)
{
	// ビューポート設定コマンドを、コマンドリストに積む
	cmdList->RSSetViewports(1, &viewport);
}

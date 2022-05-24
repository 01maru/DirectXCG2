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
	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	cmdList->RSSetViewports(1, &viewport);
}
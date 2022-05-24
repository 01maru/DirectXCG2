#include "ScissorRect.h"

ScissorRect::ScissorRect(const int left, const int width, const int top, const int height)
{
	scissorRect.left = left; // 切り抜き座標左
	scissorRect.right = scissorRect.left + width; // 切り抜き座標右
	scissorRect.top = top; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + height; // 切り抜き座標下
}

void ScissorRect::Update(ID3D12GraphicsCommandList* cmdList)
{
	// シザー矩形設定コマンドを、コマンドリストに積む
	cmdList->RSSetScissorRects(1, &scissorRect);
}

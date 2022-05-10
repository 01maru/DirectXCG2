#include "BackBuffer.h"

BackBuffer::BackBuffer(SwapChain swapChain, DesHeap desHeap, ID3D12Device* dev)
{
	backBuffers.resize(swapChain.Desc().BufferCount);

	// スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++) {
		// スワップチェーンからバッファを取得
		swapChain.Chain()->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// デスクリプタヒープのハンドルを取得
		rtvHandle = desHeap.RtvHeap()->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かでアドレスがずれる
		rtvHandle.ptr += i * dev->GetDescriptorHandleIncrementSize(desHeap.Desc().Type);
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		dev->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}
}

std::vector<ID3D12Resource*> BackBuffer::BackBuff()
{
	return backBuffers;
}

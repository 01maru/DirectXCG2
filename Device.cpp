#include "Device.h"


// 対応レベルの配列
D3D_FEATURE_LEVEL levels[] = {
D3D_FEATURE_LEVEL_12_1,
D3D_FEATURE_LEVEL_12_0,
D3D_FEATURE_LEVEL_11_1,
D3D_FEATURE_LEVEL_11_0,
};

Device::Device(IDXGIAdapter4* tmpAdapter)
{
	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++) {
		// 採用したアダプターでデバイスを生成
		HRESULT result = D3D12CreateDevice(tmpAdapter, levels[i],
			IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

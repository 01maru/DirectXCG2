#include "Device.h"


// �Ή����x���̔z��
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
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		HRESULT result = D3D12CreateDevice(tmpAdapter, levels[i],
			IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
}

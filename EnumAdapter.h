#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class EnumAdapter
{
private:
	IDXGIAdapter4* tmpAdapter = nullptr;	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIFactory7* dxgiFactory = nullptr;

public:
	EnumAdapter();

	//	�Q�b�^�[
	IDXGIAdapter4* TmpAdapter();
	IDXGIFactory7* DxgiFactory();
};


#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class EnumAdapter
{
public:
	IDXGIAdapter4* tmpAdapter = nullptr;	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIFactory7* dxgiFactory = nullptr;

public:
	EnumAdapter();
};


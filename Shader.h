#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Shader
{
public:
	HRESULT result;

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g
public:
	Shader();
	void Error();
};

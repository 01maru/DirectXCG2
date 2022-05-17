#include "GraphicsPipeLine.h"

GraphicsPipeLine::GraphicsPipeLine(VertexShader vertShade, D3D12_INPUT_ELEMENT_DESC* inputLayout, UINT size, ID3D12Device* dev)
{
	//	�e�N�X�`���[�T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//�f�X�N���v�^�����W�̐ݒ�
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//	���[�g�p�����[�^�̐ݒ�
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//	�萔�o�b�t�@�r���[
	rootParam[0].Descriptor.ShaderRegister = 0;					//	�萔�o�b�t�@�ԍ�
	rootParam[0].Descriptor.RegisterSpace = 0;						//	�f�t�H���g�l
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	���ׂẴV�F�[�_���猩��

	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//	�萔�o�b�t�@�r���[
	rootParam[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//	�萔�o�b�t�@�ԍ�
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;						//	�f�t�H���g�l
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//	���ׂẴV�F�[�_���猩��

	// �V�F�[�_�[�̐ݒ�
	desc.VS.pShaderBytecode = vertShade.vsBlob->GetBufferPointer();
	desc.VS.BytecodeLength = vertShade.vsBlob->GetBufferSize();
	desc.PS.pShaderBytecode = vertShade.psBlob->GetBufferPointer();
	desc.PS.BytecodeLength = vertShade.psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�or���C���[�t���[��
	desc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	// �u�����h�X�e�[�g
	//desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//	RGBA���ׂẴ`�����l���`��
	
	// ���_���C�A�E�g�̐ݒ�
	desc.InputLayout.pInputElementDescs = inputLayout;
	desc.InputLayout.NumElements = size;

	// �}�`�̌`��ݒ�
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	desc.NumRenderTargets = 1; // �`��Ώۂ�1��
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	desc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// ���[�g�V�O�l�`��
	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParam;		//	�擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParam);			//	���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &vertShade.errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	desc.pRootSignature = rootSignature;

	// �p�C�v�����X�e�[�g�̐���
	result = dev->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&state));
	assert(SUCCEEDED(result));
}

void GraphicsPipeLine::AlphaBlend()
{
	//	����������
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;		//	�\�[�X�̃��l���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;	//	1.0f-�\�[�X�̃��l���g��
}

void GraphicsPipeLine::InversBlend()
{
	//	�F���]
	blenddesc.BlendEnable = true;					//	�L����
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;			//	���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_COLOR;	//	1.0f-�f�X�g�J���[�̒l���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;			//	�g��Ȃ�
}

void GraphicsPipeLine::SubBlend()
{
	//	���Z����
	blenddesc.BlendEnable = true;					//	�L����
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;	//	���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;				//	�\�[�X�̒l��100���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;				//	�f�X�g�̒l��100���g��
}

void GraphicsPipeLine::AddBlend()
{
	//	���Z����
	blenddesc.BlendEnable = true;					//	�L����
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	�\�[�X�̒l��100���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;		//	�f�X�g�̒l��100���g��
}

void GraphicsPipeLine::NormalBlend()
{
	blenddesc.BlendEnable = true;					//	�L����
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//	���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//	�\�[�X�̒l��100���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//	�g��Ȃ�
}

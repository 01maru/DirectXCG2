cbuffer ConstBufferDataMaterial:register(b0)
{
	float4 color;	//	RGBA
};

cbuffer ConstBufferDataTransform:register(b1)
{
	matrix mat;
};

struct VSOutput
{
	//	�Z�}���e�B�b�N(SV_��System Value)
	float4 svpos : SV_POSITION;	//	���W���
	float3 normal :	NORMAL;
	float2 uv :TEXCOORD;
};

struct Output
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};
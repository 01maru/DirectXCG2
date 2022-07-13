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
	//	セマンティック(SV_はSystem Value)
	float4 svpos : SV_POSITION;	//	座標情報
	float3 normal :	NORMAL;
	float2 uv :TEXCOORD;
};

struct Output
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};
cbuffer ConstBufferDataMaterial:register(b0)
{
	float4 color;	//	RGBA
};

struct VSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv :TEXCOORD;
};
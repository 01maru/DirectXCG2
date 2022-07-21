#include "Basic.hlsli"

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 obj_main(VSObjOutput input) : SV_TARGET
{
	return input.color;
}

float4 color_main() : SV_TARGET
{
	return color;
}
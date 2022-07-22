#include "Basic.hlsli"

float4 sphere_main(VSOutput input) : SV_TARGET
{
	float3 light = normalize(float3(-1,-1,1));
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.2;
	return float4(float3(1, 1, 1) * brightness, 1);
}

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
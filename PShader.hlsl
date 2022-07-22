#include "Basic.hlsli"

Texture2D<float4>tex : register(t0);
SamplerState smp : register(s0);

float4 main(Output input) : SV_TARGET
{
	float2 samplePoint = input.uv;
	float4 Tex = tex.Sample(smp, input.uv);
	float sinv = sin(input.uv.y * 2 + time * -0.1);
	float steped = step(0.99, sinv * sinv);
	Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 50.0 + time * 1.0)) * 0.05;
	Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 100.0 - time * 2.0)) * 0.08;
	Tex.rgb += steped * 0.1;
	return Tex;
	//return float4(tex.Sample(smp,input.uv));
}
#include"ShaderStructs.hlsli"

float4 PS_Main(VS_Output input) : SV_TARGET
{
    return float4(1.f, 1.f, 1.f, 1.f);
    float4 color = float4(input.normalInWorld.xyz, 1.f);
	return color;
}
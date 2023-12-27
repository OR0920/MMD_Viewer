#include"ShaderStructs.hlsli"

float4 PS_Main(VS_Output input) : SV_TARGET
{
    float4 color = (input.normalInWorld.xyz, 1.f);
	return color;
}
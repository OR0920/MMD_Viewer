#include "BasicShaderHeader.hlsli"

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    return float4(tex.Sample(smp, vso.uv));
}
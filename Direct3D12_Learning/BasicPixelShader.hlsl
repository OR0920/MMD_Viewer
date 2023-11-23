#include "BasicShaderHeader.hlsli"

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    return float4(vso.uv, 1.f, 1.f);
}
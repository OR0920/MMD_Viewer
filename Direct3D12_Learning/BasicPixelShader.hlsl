#include "BasicShaderHeader.hlsli"

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    float4 col = vso.normal;
    return col;
}


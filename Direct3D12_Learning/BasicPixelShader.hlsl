#include "BasicShaderHeader.hlsli"


float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    float3 light = normalize(float3(1.f, -1.f, 1.f));

    float brightness = dot(-light, vso.normal.xyz);
    
    float4 col;
    col.xyz = brightness;
    col.w = 1.f;
    col *= diffuse;
    return col;
}


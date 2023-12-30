#include "MMD_ShaderStructs.hlsli"

cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

float4 main(VS_Output input) : SV_TARGET
{
    float4 color;
    
    float blightness = -dot(input.normal.xyz, lightDir);
    color.rgb = float3(blightness, blightness, blightness);
    color.a = 1.f;
     
    return color;
    
    //return input.normal;
}
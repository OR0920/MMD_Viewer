#include "MMD_ShaderStructs.hlsli"

cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

cbuffer Material : register(b2)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
};

float4 main(VS_Output input) : SV_TARGET
{
    return diffuse;
    float4 color;
        
    float blightness = -dot(input.normal.xyz, lightDir);
    color.xyz = diffuse.xyz * blightness;
    color.w = diffuse.w;
    
    return color;
    
    //return input.normal;
}
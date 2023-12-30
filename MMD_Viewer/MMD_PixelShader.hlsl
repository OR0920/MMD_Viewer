#include "MMD_ShaderStructs.hlsli"

cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

cbuffer Material : register(b2)
{
    float4 diffuse;
    float3 specular;
    float specularity;
    float3 ambient;
};

float4 main(VS_Output input) : SV_TARGET
{
    float4 color;
        
    float blightness = -dot(input.normal.xyz, lightDir);
    color.xyz = diffuse.xyz * blightness;
    color.w = diffuse.w;
    
    return color;
    
    //return input.normal;
}
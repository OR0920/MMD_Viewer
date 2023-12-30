#include"MMD_ShaderStructs.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

cbuffer Transform : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float3 eyePos;
}

VS_Output main(VS_Input input)
{
    VS_Output output;

    float4 pos = input.position;
    pos = mul(world, pos);

    output.ray = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);
    pos = mul(proj, pos);
    
    output.screenPos = pos;
    output.normal = mul(world, input.normal);
    
    return output;
}


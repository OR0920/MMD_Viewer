#include"MMD_ShaderStructs.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float edgeRage : EDGE_RATE;
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

    input.position.w = 1.f;
    input.normal.w = 0.f;
    
    float4 pos = input.position;
    pos = mul(world, pos);

    output.ray = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);
    pos = mul(proj, pos);
    
    output.screenPos = pos;
    output.normal = mul(world, input.normal);
    output.vnormal = mul(view, output.normal);
    
    output.uv = input.uv;
    
    
    return output;
}


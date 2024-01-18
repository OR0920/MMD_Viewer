
#include"OutlineStruct.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float edge : EDGE_RATE;
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
    
    float4 position = input.position;
    position.w = 1.f;
    
    
    position = mul(world, position);
    position = mul(view, position);
    position = mul(proj, position);
 
    output.position = position;
    
    return output;
}


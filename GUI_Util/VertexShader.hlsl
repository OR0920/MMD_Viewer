#include"ShaderStructs.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

cbuffer Transforms : register(b0)
{
    matrix W;
    matrix V;
    matrix P;
};

VS_Output VS_Main(VS_Input input ) 
{
    VS_Output output;
    
    input.position.w = 1.f;
    
    output.position = mul(W, input.position);
    output.position = mul(V, output.position);
    output.position = mul(P, output.position);
    
    output.normalInWorld = mul(W, input.normal);
    
    return output;
}
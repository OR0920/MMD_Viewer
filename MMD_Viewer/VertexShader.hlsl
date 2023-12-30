#include "ShaderStructs.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 color : COLOR;
};

cbuffer TransForm : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

VS_Output main( VS_Input input) 
{
    VS_Output output;
    
    output.screenPos = mul(world, input.position);
    output.screenPos = mul(view, output.screenPos);
    output.screenPos = mul(proj, output.screenPos);
    //output.screenPos = input.position;
    output.color = input.color; 
    
    return output;    
}
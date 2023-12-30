#include "ShaderStructs2D.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 color : COLOR;
};

cbuffer TransForm : register(b0)
{
    matrix Translation;
    matrix Rotation;
}

VS_Output main( VS_Input input) 
{
    VS_Output output;
    
    output.screenPos = mul(Rotation, input.position);
    output.screenPos = mul(Translation, output.screenPos);
    //output.screenPos = input.position;
    output.color = input.color; 
    
    return output;    
}
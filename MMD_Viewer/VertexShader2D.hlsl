#include "ShaderStructs2D.hlsli"

struct VS_Input
{
    float4 position : POSITION;
    float4 color : COLOR;
};

VS_Output main( VS_Input input) 
{
    VS_Output output;
    
    output.screenPos = input.position;
    output.color = input.color;
    
    return output;    
}
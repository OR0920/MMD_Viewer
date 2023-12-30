#include "ShaderStructs2D.hlsli"

cbuffer Color : register(b1)
{
    float4 color;
}

float4 main(VS_Output input) : SV_TARGET
{
    return color;
}


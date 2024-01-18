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
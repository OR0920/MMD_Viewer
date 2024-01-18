struct VS_Output
{
    float4 position : SV_POSITION;
};

cbuffer Material : register(b1)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
};
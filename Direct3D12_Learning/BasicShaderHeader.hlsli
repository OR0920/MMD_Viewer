
struct VS_OutPut
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D<float4> tex : register(t0);
Texture2D<float4> sph : register(t1);
Texture2D<float4> spa : register(t2);
SamplerState smp : register(s0);

cbuffer transform : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer material : register(b1)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
}
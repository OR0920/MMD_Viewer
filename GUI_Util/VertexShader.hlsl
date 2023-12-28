#include "ShaderStructs.hlsli"

struct VS_Input
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};


cbuffer scene : register(b0)
{
    matrix view;
    matrix projection;
    float3 eye;
}

cbuffer transform : register(b1)
{
    matrix world;
};

VS_OutPut VS_Main(VS_Input vsi)
{
    vsi.pos.w = 1.f;
    VS_OutPut vso;
    
    vso.pos = vsi.pos;
    vsi.pos = mul(world, vsi.pos);

    vso.ray = normalize(eye - vsi.pos.xyz);
    vso.svpos = mul(mul(projection, view), vsi.pos);

    vsi.normal.w = 0.f;
    vso.normal = mul(world, vsi.normal);
    vso.vnormal = mul(view, vso.normal);

    vso.uv = vsi.uv;

    return vso;
}
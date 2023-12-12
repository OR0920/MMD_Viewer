#include "BasicShaderHeader.hlsli"

struct VS_Input
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint4 boneno : BONE_NO;
    float4 weight : WEIGHT;
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
    matrix bone[256];
};

VS_OutPut BasicVS(VS_Input vsi)
{
    vsi.pos.w = 1.f;
    VS_OutPut vso;
    
    //vsi.pos = mul(bone[vsi.boneno[0]], vsi.pos);
    
    matrix bm =
        bone[vsi.boneno[0]] * vsi.weight[0] +
        bone[vsi.boneno[1]] * vsi.weight[1] +
        bone[vsi.boneno[2]] * vsi.weight[2] +
        bone[vsi.boneno[3]] * vsi.weight[3];
    
    vsi.pos = mul(bm, vsi.pos);
    
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
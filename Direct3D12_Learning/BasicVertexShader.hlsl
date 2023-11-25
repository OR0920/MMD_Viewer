#include "BasicShaderHeader.hlsli"

struct VS_Input
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
    min16int2 boneno : BONE_NO;
    min16int weight : WEIGHT;   
};

VS_OutPut BasicVS(VS_Input vsi)
{
    VS_OutPut vso;
    vso.pos = vsi.pos;  
    vso.svpos = mul(mat, vsi.pos);
    vso.uv = vsi.uv;
    return vso;
}
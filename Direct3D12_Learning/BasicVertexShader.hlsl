#include "BasicShaderHeader.hlsli"

struct VS_Input
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

VS_OutPut BasicVS(VS_Input vsi)
{
    VS_OutPut vso;
    vso.pos = vsi.pos;  
    vso.svpos = vsi.pos;
    vso.uv = vsi.uv;
    return vso;
}
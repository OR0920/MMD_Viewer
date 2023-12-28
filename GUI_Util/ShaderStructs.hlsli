
struct VS_OutPut
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
    float4 normal : NORMAL;
    float4 vnormal : NORMAL1;
    float2 uv : TEXCOORD;
    float3 ray : VECTOR;
};


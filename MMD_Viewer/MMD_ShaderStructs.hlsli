

struct VS_Output
{
    float4 screenPos : SV_POSITION;
    float4 normal : NORMAL;
    float4 vnormal : NORMAL1;
    float2 uv : UV;
    float3 ray : RAY;
};


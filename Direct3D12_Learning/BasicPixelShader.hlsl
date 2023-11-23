struct VS_OutPut
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
};

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    return float4((float2(0.f, 1.f) + vso.pos.xy) * 0.5f, 1.f, 1.f);
}
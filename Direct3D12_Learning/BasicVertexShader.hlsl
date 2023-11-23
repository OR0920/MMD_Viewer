struct VS_OutPut
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
};

VS_OutPut BasicVS( float4 pos : POSITION )
{
    VS_OutPut vso;
    vso.pos = pos;  
    vso.svpos = pos;
    return vso;
}
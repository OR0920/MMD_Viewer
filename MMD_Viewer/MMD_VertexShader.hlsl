#include "MMD_VS_Output.hlsli"
#include "MMD_VS_Structs.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;

    input.position.w = 1.f;
    input.normal.w = 0.f;
    
    float4 pos = input.position;
    pos = mul(world, pos);

    output.ray = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);
    pos = mul(proj, pos);
    
    output.screenPos = pos;
    output.normal = mul(world, input.normal);
    output.vnormal = mul(view, output.normal);
    
    output.uv = input.uv;
    
    
    return output;
}


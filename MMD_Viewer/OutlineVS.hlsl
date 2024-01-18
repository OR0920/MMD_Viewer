#include"MMD_VS_Output.hlsli"
#include"MMD_VS_Structs.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;
    
    float4 position = input.position;
    position.w = 1.f;
     
    position.xyz += input.normal.xyz * 0.05f;
    
    position = mul(world, position);
    position = mul(view, position);
    position = mul(proj, position);
 
    output.screenPos = position;
    
    return output;
}


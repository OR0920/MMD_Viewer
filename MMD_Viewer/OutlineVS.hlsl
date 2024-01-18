#include"OutlineStruct.hlsli"
#include"MMD_ShaderStructs.hlsli"

// 輪郭線を描画
// 表面をカリングしたモデルを、少し拡大し、重ねて描画
OutlineVS_Output main(VS_Input input)
{
    OutlineVS_Output output;
    
    float4 position = input.position;
    position.w = 1.f;
    
    // 法線方向に頂点を少しだけ動かす
    position.xyz += normalize(input.normal.xyz) * 0.025f * edgeSize * input.edgeRate;
    
    // 普通に座標変換
    position = mul(world, position);
    position = mul(view, position);
    position = mul(proj, position);
 
    output.position = position;
    
    return output;
}


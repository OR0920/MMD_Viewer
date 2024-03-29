#include"MMD_ShaderStructs.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;

    // 移動しない
    input.normal.w = 0.f;
    
    // 頂点変換 移動させるために同時座標化
    float4 pos = float4(input.position, 1.f);
    pos = mul(world, pos);

    // 頂点への視線ベクトル
    output.ray = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);
    pos = mul(proj, pos);
    
    output.screenPos = pos;

    // 法線を変換
    output.normal = mul(world, input.normal);
    output.vnormal = mul(view, output.normal);
    
    // UVは変換の必要なし
    output.uv = input.uv;
    
    return output;
}


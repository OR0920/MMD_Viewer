#include"OutlineStruct.hlsli"
#include"MMD_ShaderStructs.hlsli"

// マテリアルに設定されている輪郭線の色で描画
float4 main(OutlineVS_Output input) : SV_TARGET
{
    return edgeColor;
}
#include"OutlineStruct.hlsli"

float4 main(VS_Output input) : SV_TARGET
{
    return edgeColor;
	//return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
#include "MMD_ShaderStructs.hlsli"


float4 main(VS_Output input) : SV_TARGET
{
	return input.normal;
}
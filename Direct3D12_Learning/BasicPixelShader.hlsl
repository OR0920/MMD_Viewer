#include "BasicShaderHeader.hlsli"


float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
	float3 light = normalize(float3(1.f, -1.f, 1.f));

	float brightness = dot(-light, vso.normal.xyz);

	//if (brightness > 0.5)
	//{
	//	brightness = 1;
	//}
	//else if(brightness > 0.1)
	//{
	//	brightness = 0.75;
	//}
	//else
	//{
	//	brightness = 0.5;
	//}


	float4 col;
	col.xyz = brightness;
	col.w = 1.f;
	col *= diffuse;
	col *= tex.Sample(smp, vso.uv);
	return col;
}


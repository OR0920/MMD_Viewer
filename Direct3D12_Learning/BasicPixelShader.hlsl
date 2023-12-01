#include "BasicShaderHeader.hlsli"


float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
	float3 light = normalize(float3(1.f, -1.f, 1.f));

	float brightness = dot(-light, vso.normal.xyz);

    //if (brightness > 0.25)
    //{
    //    brightness = 1;
    //}
    //else
    //{
    //    brightness = 0.5;
    //}

    float4 col;
	col.xyz = brightness;
	col.w = 1.f;
	col *= diffuse;
	col *= tex.Sample(smp, vso.uv);
    	
    float2 normalUV = (vso.normal.xy + float2(1, -1)) * float2(0.5, -0.5);
    //float2 sphereMapUV = vso.vnormal.xy;
    
    col *= sph.Sample(smp, normalUV);
    col += spa.Sample(smp, normalUV);
	
	return col;
}


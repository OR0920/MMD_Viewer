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
    float4 texCol = tex.Sample(smp, vso.uv);    	
    col *= texCol;
    
    float2 sphereMapUV = vso.vnormal.xy;
    sphereMapUV= (sphereMapUV+ float2(1.f, -1.f)) * float2(0.5f, -0.5f);
    col *= sph.Sample(smp, sphereMapUV);
    col += spa.Sample(smp, sphereMapUV);
    col += float4(ambient * texCol.xyz, 1.f);

	return col;
}


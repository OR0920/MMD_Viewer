#include "BasicShaderHeader.hlsli"

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    float3 light = normalize(float3(1.f, -1.f, 1.f));
    float diffuseB = dot(-light, vso.normal.xyz);
    float4 toonDiffuse = toon.Sample(toonSmp, float2(0, 1.f - diffuseB));

    float3 refLight = normalize(reflect(light, vso.normal.xyz));
    float3 specularB = pow(saturate(dot(refLight, -vso.ray)), specular.a);
    
    float2 sphereMapUV = vso.vnormal.xy;
    sphereMapUV = (sphereMapUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);

    float4 texColor = tex.Sample(smp, vso.uv);
    float4 sphColor = sph.Sample(smp, sphereMapUV);
    float4 spaColor = spa.Sample(smp, sphereMapUV);
   
    //if(diffuseB > 0.3f)
    //{
    //    diffuseB = 0.7f; 
    //}
    //else
    //{
    //    diffuseB = 0.3f;
    //}
     
    
    float4 diffuseColor = toonDiffuse * diffuse * texColor;
    diffuseColor *= sphColor;
    diffuseColor += spaColor * texColor;
    float4 specularColor = float4(specularB * specular.rgb, 1.f);
    float4 ambientColor = float4(ambient * texColor.rgb, 1.f) * 0.5f;

    static int count = 0;
    count++;
    count %= 3;
    
    
    return diffuseColor + specularColor + ambientColor;
    
    //if (diffuseB > 0.25)
    //{
    //    diffuseB = 1;
    //}
    //else
    //{
    //    diffuseB = 0.5;
    //}
}


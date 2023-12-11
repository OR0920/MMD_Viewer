#include "BasicShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float4> sph : register(t1);
Texture2D<float4> spa : register(t2);
Texture2D<float4> toon : register(t3);
SamplerState smp : register(s0);
SamplerState toonSmp : register(s1);

cbuffer material : register(b2)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
}

float4 DefaultPS(VS_OutPut vso);
float4 SamplePS(VS_OutPut vso);
float4 MyPS(VS_OutPut vso);

float4 BasicPS(VS_OutPut vso) : SV_TARGET
{
    float2 sphereUV = vso.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
        
    float4 finalColor = tex.Sample(smp, vso.uv) * sph.Sample(smp, sphereUV) + spa.Sample(smp, sphereUV);
    
    float3 light = normalize(float3(-1.f, -1.f, 1.f));
    
    float diffuseB = -dot(vso.normal.xyz, light);
    if (diffuseB < 0.f)
    {
        diffuseB = 0.f;
    }
    
    float4 toonDiffse = toon.Sample(toonSmp, float2(0.f, 1 - diffuseB));
    
    float4 diffuseLight = float4(diffuse.rgb * toonDiffse.rgb, diffuse.a);

    float3 ref = reflect(light, vso.normal.xyz);
    float3 toEye = normalize(vso.ray);
    float specularB = dot(ref, toEye);
    if (specularB < 0.f)
    {
        specularB = 0.f;
    }
    specularB = pow(specularB, 5.f);
    float4 specularLight = float4(specular.rgb * specularB, 0.f);
    
    float4 ambientLight = float4(ambient, 0.f) * 0.2;
    
    float4 finalLight =  diffuseLight + specularLight + ambientLight;
    
    return finalColor * finalLight;
    
    return MyPS(vso);
    return SamplePS(vso);
    return DefaultPS(vso);
}

float4 DefaultPS(VS_OutPut vso)
{
    float3 light = normalize(float3(-1.f, 0.f, 1.f));
    float diffuseB = -dot(light, vso.normal.xyz);
    float4 toonDiffuse = (toon.Sample(toonSmp, float2(0, 1.f - diffuseB)), diffuseB);

    float3 refLight = normalize(reflect(light, vso.normal.xyz));
    float3 specularB = pow(saturate(dot(refLight, -vso.ray)), specular.a);
    
    float2 sphereMapUV = vso.vnormal.xy;
    sphereMapUV = (sphereMapUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);

    float4 texColor = tex.Sample(smp, vso.uv);
    
    float4 sphColor = sph.Sample(smp, sphereMapUV);
    float4 spaColor = spa.Sample(smp, sphereMapUV);
   
   
    //return diffuse * texColor * sphColor + spaColor;
    
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
    
    float4 finalColor = diffuseColor + specularColor + ambientColor;
    
    finalColor = float4(finalColor.rgb, diffuse.a);
    
    return finalColor;
    //if (diffuseB > 0.25)
    //{
    //    diffuseB = 1;
    //}
    //else
    //{
    //    diffuseB = 0.5;
    //}
}

float4 SamplePS(VS_OutPut vso)
{
    float3 light = normalize(float3(1, -1, 1)); //光の向かうベクトル(平行光線)
    float3 lightColor = float3(1, 1, 1); //ライトのカラー(1,1,1で真っ白)

	//ディフューズ計算
    float diffuseB = saturate(dot(-light, vso.normal.xyz));
    float4 toonDif = float4(toon.Sample(toonSmp, float2(0, 1.0 - diffuseB)).rgb, diffuse.a);

	//光の反射ベクトル
    float3 refLight = normalize(reflect(light, vso.normal.xyz));
    float specularB = pow(saturate(dot(refLight, -vso.ray)), specular.a);
	
	//スフィアマップ用UV
    float2 sphereMapUV = vso.vnormal.xy;
    sphereMapUV = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);

    float4 texColor = tex.Sample(smp, vso.uv); //テクスチャカラー

    return saturate(toonDif //輝度(トゥーン)
		* diffuse //ディフューズ色
		* texColor //テクスチャカラー
		* sph.Sample(smp, sphereMapUV)) //スフィアマップ(乗算)
		+ saturate(spa.Sample(smp, sphereMapUV) * texColor //スフィアマップ(加算)
		+ float4(specularB * specular.rgb, 0)) //スペキュラー
		+ float4(texColor.rgb * ambient * 0.5, 0); //アンビエント(明るくなりすぎるので0.5にしてます)
}

float4 MyPS(VS_OutPut vso)
{
    if (diffuse.a == 0.f)
        return 0.f;
    
    float3 light = float3(1.f, -1.f, 1.f);
    
    float4 texColor = tex.Sample(smp, vso.uv);
        
    float2 sphereMapUV = vso.vnormal.xy;
    sphereMapUV = (sphereMapUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
    float4 sphColor = sph.Sample(smp, sphereMapUV);
    float4 spaColor = spa.Sample(smp, sphereMapUV);
    
    float diffuseB = -dot(vso.normal.xyz, light);
    if (diffuseB < 0.f)
    {
        diffuseB = 0.f;
    }
    float4 diffuseB_Color = float4(diffuseB, diffuseB, diffuseB, 1.f);

    float4 toonColor = float4(toon.Sample(toonSmp, float2(0.f, 1.f - diffuseB)).rgb, 1.f);

    //return toonColor;
    
    
    float4 diffuseColor = toonColor * diffuse * texColor * sphColor + spaColor;
    return diffuseColor;
}
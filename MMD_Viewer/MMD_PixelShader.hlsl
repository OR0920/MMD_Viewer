#include"MMD_ShaderStructs.hlsli"

// 通常テクスチャ
Texture2D<float4> tex : register(t0);
// スフィアテクスチャ(乗算)
Texture2D<float4> sph : register(t1);
// スフィアテクスチャ(加算) 
Texture2D<float4> spa : register(t2);
// トゥーンテクスチャ
Texture2D<float4> toon : register(t3);


SamplerState smp : register(s0);
// トゥーン用のサンプラー
SamplerState toonSmp : register(s1);


float4 main(VS_Output input) : SV_TARGET
{
    float4 lightColor = float4(0.6f, 0.6f, 0.6f, 1.f);
    
    // テクスチャから色を取得
    float4 texColor = tex.Sample(smp, input.uv);
    float2 sphereUV = input.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
    float4 sphColor = float4(sph.Sample(smp, sphereUV).rgb, 1.f);
    float4 spaColor = float4(spa.Sample(smp, sphereUV).rgb, 0.f);
    float4 finalTexColor = texColor * sphColor + spaColor;
    
    // ディフューズ計算
    float diffuseBrightness = saturate(dot(-normalize(lightDir), normalize(input.normal.xyz)));
    diffuseBrightness /= 2.f;
    diffuseBrightness += 0.5f;
    float4 toonBrightness = toon.Sample(toonSmp, float2(0.f, 1.f - diffuseBrightness));
    float4 diffuseLight = toonBrightness * diffuse * lightColor;
    

    // スペキュラ計算
    float3 refVec = reflect(normalize(lightDir), normalize(input.normal.xyz));
    float specularBrightness = saturate(dot(refVec, input.ray));
    specularBrightness = saturate(pow(specularBrightness, specular.a));
    float4 specularLight = float4(specular.rgb, 0.f) * specularBrightness * lightColor;
    
    // アンビエント計算     
    float4 ambientLight = float4(ambient, 0.f);

    float4 finalLight = saturate(diffuseLight + specularLight + ambientLight);
    
    return finalLight * finalTexColor;
}
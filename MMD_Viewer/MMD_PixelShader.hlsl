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
    // 色の計算
    
    // スフィアテクスチャ用UV
    float2 sphereUV = input.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
        
    // 最終的な色
    float4 finalColor = tex.Sample(smp, input.uv) * float4(sph.Sample(smp, sphereUV).rgb, 1.f) + float4(spa.Sample(smp, sphereUV).rgb, 0.f);
    
    // ライトの計算
    
    float3 light = normalize(lightDir);
    
    // ディフューズ
    float diffuseB = -dot(input.normal.xyz, light);
    if (diffuseB < 0.f)
    {
        diffuseB = 0.f;
    }
    
    // 陰影をトゥーン化
    float4 toonDiffse = toon.Sample(toonSmp, float2(0.f, 1 - diffuseB));

    float4 diffuseLight = float4(diffuse.rgb * toonDiffse.rgb, diffuse.a);

    // スペキュラ
    float3 ref = reflect(light, input.normal.xyz);
    float3 toEye = normalize(input.ray);
    float specularB = dot(ref, toEye);
    if (specularB < 0.f)
    {
        specularB = 0.f;
    }
    specularB = pow(specularB, 5.f);
    float4 specularLight = float4(specular.rgb * specularB, 0.f);
    
    // アンビエント　
    float4 ambientLight = float4(ambient, 0.f) * 0.1f;
    
    // 最終的なライトの強さ
    float4 finalLight = diffuseLight + specularLight + ambientLight;
    
    // ライトと色を合わせて描画
    return finalColor * finalLight;
}
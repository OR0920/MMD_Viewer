#include "MMD_ShaderStructs.hlsli"

cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

cbuffer Material : register(b2)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
};

Texture2D<float4> tex : register(t0);
Texture2D<float4> sph : register(t1);
Texture2D<float4> spa : register(t2);
Texture2D<float4> toon : register(t3);
SamplerState smp : register(s0);
SamplerState toonSmp : register(s1);

float4 main(VS_Output input) : SV_TARGET
{
    return spa.Sample(smp, input.uv);
    
    float3 light = normalize(float3(-1.f, -1.f, 1.f));
    
    float diffuseB = -dot(input.normal.xyz, light);

    float4 diffuseLight = float4(diffuse.rgb * diffuseB, diffuse.a);

    float3 ref = reflect(light, input.normal.xyz);
    float3 toEye = normalize(input.ray);
    float specularB = dot(ref, toEye);

    specularB = pow(specularB, 5.f);

    float4 specularLight = float4(specular.rgb * specularB, 0.f);
    
    float4 ambientLight = float4(ambient, 0.f) * 0.2;
    
    float4 finalLight = diffuseLight + specularLight + ambientLight;
    
    float4 finalColor = tex.Sample(smp, input.uv);
    
    return finalLight * finalColor;
}
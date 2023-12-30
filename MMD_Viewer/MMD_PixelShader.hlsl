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

float4 main(VS_Output input) : SV_TARGET
{
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
    
    return finalLight;
}
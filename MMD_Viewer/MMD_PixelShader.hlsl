#include"MMD_ShaderStructs.hlsli"

// �ʏ�e�N�X�`��
Texture2D<float4> tex : register(t0);
// �X�t�B�A�e�N�X�`��(��Z)
Texture2D<float4> sph : register(t1);
// �X�t�B�A�e�N�X�`��(���Z) 
Texture2D<float4> spa : register(t2);
// �g�D�[���e�N�X�`��
Texture2D<float4> toon : register(t3);


SamplerState smp : register(s0);
// �g�D�[���p�̃T���v���[
SamplerState toonSmp : register(s1);


float4 main(VS_Output input) : SV_TARGET
{
    float4 lightColor = float4(0.6f, 0.6f, 0.6f, 1.f);
    
    // �e�N�X�`������F���擾
    float4 texColor = tex.Sample(smp, input.uv);
    float2 sphereUV = input.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
    float4 sphColor = float4(sph.Sample(smp, sphereUV).rgb, 1.f);
    float4 spaColor = float4(spa.Sample(smp, sphereUV).rgb, 0.f);
    texColor *= sphColor;
    texColor += spaColor;
    
    // �f�B�t���[�Y�v�Z
    float diffuseBrightness = dot(-lightDir, input.normal.xyz);
    if(diffuseBrightness < 0.f)
    {
        diffuseBrightness = 0.f;
    }
    float4 toonBrightness = toon.Sample(toonSmp, float2(0.f, 1.f - diffuseBrightness));
    float4 diffuseLight = float4(toonBrightness.rgb * diffuse.rgb * lightColor.rgb, diffuse.a);
    
    // �X�y�L�����v�Z
    float3 refVec = reflect(lightDir, input.normal.xyz);
    float specularBrightness = dot(refVec, input.ray);
    if(specularBrightness < 0.f)
    {
        specularBrightness = 0.f;
    }
    specularBrightness = saturate(pow(specularBrightness, specular.a));
    float4 specularLight = float4(specularBrightness * specular.rgb * lightColor.rgb, 0.f);
    
    
    // �A���r�G���g�v�Z     
    float4 ambientLight = float4(ambient, 0.f);

    float4 finalLight = diffuseLight + specularLight + ambientLight;
    
    return texColor * finalLight;
}
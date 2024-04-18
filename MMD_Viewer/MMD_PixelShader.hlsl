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
    float3 lightColor = float3(0.6f, 0.6f, 0.6f);
 
    // diffuse�F���擾
    float3 finalColor = diffuse.rgb * lightColor;
    float finalAlpha = diffuse.a;
    
    // ambient�ŕ␳
    finalColor += ambient;
    finalColor = saturate(finalColor);
     
    // �e�N�X�`������F���擾���K�p
    float4 texColor = tex.Sample(smp, input.uv);
    finalColor *= texColor.rgb;
    finalAlpha *= texColor.a;
 
    // �X�t�B�A�}�b�v����F���擾���K�p
    float2 sphereUV = input.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
    float3 sphColor = sph.Sample(smp, sphereUV).rgb;
    float3 spaColor = spa.Sample(smp, sphereUV).rgb;
    finalColor *= sphColor;
    finalColor += spaColor;
    
    // diffuse���v�Z
    float diffuseBrightness = dot(normalize(-lightDir), normalize(input.normal.xyz));
    // ���������肷���Ȃ��悤�␳
    diffuseBrightness *= 0.5f;
    diffuseBrightness += 0.5f;
    float3 toonDiffuse = toon.Sample(toonSmp, float2(0.f, 1.f - diffuseBrightness)).rgb;
    finalColor *= toonDiffuse;
    
    //return float4(texColor.rgb, finalAlpha);
 
    // specular���v�Z
    if (specular.a > 0.f)
    {
        float3 refVec = normalize(reflect(lightDir, input.normal.xyz));
        float specularBrightness = saturate(dot(refVec, input.ray));
        specularBrightness = pow(specularBrightness, specular.a);
        specularBrightness = specularBrightness * specular.rgb * lightColor;
        finalColor += specularBrightness;
    }
    
    return float4(finalColor, finalAlpha);
}
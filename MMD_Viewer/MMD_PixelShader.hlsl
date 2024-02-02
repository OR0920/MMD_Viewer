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
    // �F�̌v�Z
    
    // �X�t�B�A�e�N�X�`���pUV
    float2 sphereUV = input.vnormal.xy;
    sphereUV = (sphereUV + float2(1.f, -1.f)) * float2(0.5f, -0.5f);
        
    // �ŏI�I�ȐF
    float4 finalColor = tex.Sample(smp, input.uv) * float4(sph.Sample(smp, sphereUV).rgb, 1.f) + float4(spa.Sample(smp, sphereUV).rgb, 0.f);
    
    // ���C�g�̌v�Z
    
    float3 light = normalize(lightDir);
    
    // �f�B�t���[�Y
    float diffuseB = -dot(input.normal.xyz, light);
    if (diffuseB < 0.f)
    {
        diffuseB = 0.f;
    }
    
    // �A�e���g�D�[����
    float4 toonDiffse = toon.Sample(toonSmp, float2(0.f, 1 - diffuseB));

    float4 diffuseLight = float4(diffuse.rgb * toonDiffse.rgb, diffuse.a);

    // �X�y�L����
    float3 ref = reflect(light, input.normal.xyz);
    float3 toEye = normalize(input.ray);
    float specularB = dot(ref, toEye);
    if (specularB < 0.f)
    {
        specularB = 0.f;
    }
    specularB = pow(specularB, 5.f);
    float4 specularLight = float4(specular.rgb * specularB, 0.f);
    
    // �A���r�G���g�@
    float4 ambientLight = float4(ambient, 0.f) * 0.1f;
    
    // �ŏI�I�ȃ��C�g�̋���
    float4 finalLight = diffuseLight + specularLight + ambientLight;
    
    // ���C�g�ƐF�����킹�ĕ`��
    return finalColor * finalLight;
}
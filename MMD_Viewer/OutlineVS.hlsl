#include"OutlineStruct.hlsli"
#include"MMD_ShaderStructs.hlsli"

// �֊s����`��
// �\�ʂ��J�����O�������f�����A�����g�債�A�d�˂ĕ`��
OutlineVS_Output main(VS_Input input)
{
    OutlineVS_Output output;
    
    float4 position = input.position;
    position.w = 1.f;
    
    // �@�������ɒ��_����������������
    position.xyz += normalize(input.normal.xyz) * 0.025f * edgeSize * input.edgeRate;
    
    // ���ʂɍ��W�ϊ�
    position = mul(world, position);
    position = mul(view, position);
    position = mul(proj, position);
 
    output.position = position;
    
    return output;
}


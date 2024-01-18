#include"MMD_ShaderStructs.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;

    // �ړ�����
    input.position.w = 1.f;
    // �ړ����Ȃ�
    input.normal.w = 0.f;
    
    // ���_�ϊ�
    float4 pos = input.position;
    pos = mul(world, pos);

    // ���_�ւ̎����x�N�g��
    output.ray = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);
    pos = mul(proj, pos);
    
    output.screenPos = pos;

    // �@����ϊ�
    output.normal = mul(world, input.normal);
    output.vnormal = mul(view, output.normal);
    
    // UV�͕ϊ��̕K�v�Ȃ�
    output.uv = input.uv;
    
    return output;
}


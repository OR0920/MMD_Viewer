#include"MMD_ShaderStructs.hlsli"

VS_Output main(VS_Input input)
{
    VS_Output output;

    // �ړ����Ȃ�
    input.normal.w = 0.f;
    
    // ���_�ϊ� �ړ������邽�߂ɓ������W��
    float4 pos = float4(input.position, 1.f);
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


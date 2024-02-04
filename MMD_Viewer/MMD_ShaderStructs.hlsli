
// ���͒��_�\����
struct VS_Input
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float edgeRate : EDGE_RATE;
};

// �o�͒��_�\����
struct VS_Output
{
    float4 screenPos : SV_POSITION;
    float4 normal : NORMAL;
    float4 vnormal : NORMAL1;
    float2 uv : UV;
    float3 ray : RAY;
};

// �ϊ��s��
cbuffer Transform : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float3 eyePos;
}

// ���C�g�Ȃǂ̃��f���Ɉˑ����Ȃ����
cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

// �}�e���A�����
cbuffer Material : register(b2)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
    float4 edgeColor;
    float edgeSize;
};


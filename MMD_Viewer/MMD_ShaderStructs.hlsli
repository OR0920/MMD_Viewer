
// 入力頂点構造体
struct VS_Input
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
    float edgeRate : EDGE_RATE;
};

// 出力頂点構造体
struct VS_Output
{
    float4 screenPos : SV_POSITION;
    float4 normal : NORMAL;
    float4 vnormal : NORMAL1;
    float2 uv : UV;
    float3 ray : RAY;
};

// 変換行列
cbuffer Transform : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float3 eyePos;
}

// ライトなどのモデルに依存しない情報
cbuffer PS_Data : register(b1)
{
    float3 lightDir;
}

// マテリアル情報
cbuffer Material : register(b2)
{
    float4 diffuse;
    float4 specular;
    float3 ambient;
    float4 edgeColor;
    float edgeSize;
};


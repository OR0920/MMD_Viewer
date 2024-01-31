#ifndef _MODEL_H_
#define _MODEL_H_

#include"GUI_Util.h"

#include"MathUtil.h"
#include"MMDsdk.h"

#include"System.h"

class Model
{
public:
	// MMDモデルの頂点構造体
	struct ModelVertex
	{
		MathUtil::float3 position;
		MathUtil::float3 normal;
		MathUtil::float2 uv;
		float edgeRate;

		void Load(const MMDsdk::PmdFile::Vertex& data);
		void Load(const MMDsdk::PmxFile::Vertex& data);
	};

	// 変換行列等
	struct ModelTransform
	{
		MathUtil::Matrix world;
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
		MathUtil::float3 eye;
	};

	// ピクセルシェーダーから参照される値
	struct PixelShaderData
	{
		MathUtil::float3 lightDir;
	};

	// シェーダーから参照されるマテリアルのデータ
	struct Material
	{
		MathUtil::float4 diffuse;
		MathUtil::float3 specular;
		float specularity;
		MathUtil::float3 ambient;
		float damy1;
		MathUtil::float4 edgeColor;
		float edgeSize;

		void Load(const MMDsdk::PmdFile::Material& data);
		void Load(const MMDsdk::PmxFile::Material& data);
	};

	// デバイスを渡して初期化
	Model(GUI::Graphics::Device& device);

	~Model();

	// 読み込み
	GUI::Result	Load(const char* const filepath);
	
	// 行列をデフォルトの値に設定
	GUI::Result SetDefaultSceneData(const float aspectRatio);

	// 描画
	void Draw(GUI::Graphics::GraphicsCommand& command);
	
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVertexBuffer;
	GUI::Graphics::IndexBuffer mIndexBuffer;

	// 全モデル共通のディスクリプタ数
	static const int sDefaultTextureCount = 2;
	static const int sDefaultToonTextureCount = 10;
	static const int sSceneDataCount = 2;
	// モデル固有のものを含めたディスクリプタ数
	int mDescriptorCount;

	// モデル描画時にバインドされるビューのヒープ
	GUI::Graphics::DescriptorHeap mHeap;

	// 定数バッファ
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mPS_DataBuffer;
	GUI::Graphics::ConstantBuffer mMaterialBuffer;

	// シェーダーから参照されないマテリアルのデータ
	struct MaterialInfo
	{
		int materialIndexCount = 0;
		int texID = -1;
		int sphID = -1;
		int spaID = -1;
		int toonID = -1;
		bool isShared = true;
		bool isEdgeEnable = false;

		void Load(const MMDsdk::PmdFile::Material& data);
		void Load(const MMDsdk::PmxFile::Material& data);
	};

	// マテリアル情報の可変長配列
	System::varray<MaterialInfo> mMaterialInfo;
	int mMaterialCount;

	// パイプラインの設定
	GUI::Graphics::InputElementDesc mInputLayout;
	GUI::Graphics::GraphicsPipeline mPipeline;
	GUI::Graphics::RootSignature mRootSignature;

	// アウトライン描画時のパイプライン設定
	GUI::Graphics::GraphicsPipeline mOutlinePipeline;

	// 全モデル共有のトゥーンテクスチャファイル
	static const wchar_t* mToonPath[10];

	// モデル固有のテクスチャデータ
	GUI::Graphics::Texture2D* mUniqueTexture;

	// モデル共有のテクスチャデータ
	GUI::Graphics::Texture2D mDefaultTextureWhite;
	GUI::Graphics::Texture2D mDefaultTextureBlack;
	GUI::Graphics::Texture2D mDefaultTextureToon[10];


	
	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);

	GUI::Result CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount);
	GUI::Result CreateIndexBuffer(const int index[], const int indexCount);
	GUI::Result CreateMaterialBuffer(const Material material[], const int materialCount);
	GUI::Result CreateTexture(const char* const dirPath, const char* const filename, const int texID);

	GUI::Result CreateDefaultBufferData();
};

#endif // !_MODEL_H_


#ifndef _MODEL_H_
#define _MODEL_H_

#include"MathUtil.h"
#include"MMDsdk.h"

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
	Model(GUI_Util::Graphics::Device& device);

	~Model();

	// 読み込み
	GUI_Util::Result	Load(const char* const filepath);
	
	// 行列をデフォルトの値に設定
	GUI_Util::Result SetDefaultSceneData(const float aspectRatio);

	// 更新
	void Update(const float frameTime);

	// 描画
	void Draw(GUI_Util::Graphics::GraphicsCommand& command);
	
private:
	GUI_Util::Graphics::Device& mDevice;

	GUI_Util::Graphics::VertexBuffer mVertexBuffer;
	GUI_Util::Graphics::IndexBuffer mIndexBuffer;

	// 全モデル共通のディスクリプタ数
	static const int sDefaultTextureCount = 2;
	static const int sDefaultToonTextureCount = 10;
	static const int sSceneDataCount = 2;
	// モデル固有のものを含めたディスクリプタ数
	int mDescriptorCount;

	// モデル描画時にバインドされるビューのヒープ
	GUI_Util::Graphics::DescriptorHeap mHeap;

	// 定数バッファ
	GUI_Util::Graphics::ConstantBuffer mTransformBuffer;
	GUI_Util::Graphics::ConstantBuffer mPS_DataBuffer;
	GUI_Util::Graphics::ConstantBuffer mMaterialBuffer;

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
		bool isNotCull = false;
		bool isTransparent = false;

		void Load(const MMDsdk::PmdFile::Material& data);
		void Load(const MMDsdk::PmxFile::Material& data);
	};

	// マテリアル情報の可変長配列
	System::varray<MaterialInfo> mMaterialInfo;
	int mMaterialCount;

	// パイプラインの設定
	GUI_Util::Graphics::InputLayout mInputLayout;
	GUI_Util::Graphics::GraphicsPipeline mPipeline;
	GUI_Util::Graphics::GraphicsPipeline mNotCullPipeline;
	GUI_Util::Graphics::RootSignature mRootSignature;

	// アウトライン描画時のパイプライン設定
	GUI_Util::Graphics::GraphicsPipeline mOutlinePipeline;

	// 全モデル共有のトゥーンテクスチャファイル
	static const wchar_t* mToonPath[10];

	// モデル固有のテクスチャデータ
	System::varray<GUI_Util::Graphics::Texture2D> mUniqueTexture;

	// モデル共有のテクスチャデータ
	GUI_Util::Graphics::Texture2D mDefaultTextureWhite;
	GUI_Util::Graphics::Texture2D mDefaultTextureBlack;
	GUI_Util::Graphics::Texture2D mDefaultTextureToon[10];

	enum class TransparentConfig : bool
	{
		DRAW_TRANSPARENT = false,
		DRAW_NOT_TRANSPARENT = true,
	};

	void DrawMaterial(GUI_Util::Graphics::GraphicsCommand& command, TransparentConfig config);
	void DrawOutline(GUI_Util::Graphics::GraphicsCommand& command);
	

	GUI_Util::Result LoadPMD(const char* const filepath);
	GUI_Util::Result LoadPMX(const char* const filepath);

	GUI_Util::Result CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount);
	GUI_Util::Result CreateIndexBuffer(const int index[], const int indexCount);
	GUI_Util::Result CreateMaterialBuffer(const Material material[], const int materialCount);
	GUI_Util::Result CreateTexture(const char* const dirPath, const char* const filename, const int texID);

	GUI_Util::Result CreateDefaultBufferData();
};

#endif // !_MODEL_H_


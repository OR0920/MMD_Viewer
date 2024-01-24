#ifndef _MODEL_H_
#define _MODEL_H_

#include"GUI_Util.h"

#include"MathUtil.h"
#include"MMDsdk.h"

class Model
{
public:

	struct ModelVertex
	{
		MathUtil::float3 position;
		MathUtil::float3 normal;
		MathUtil::float2 uv;
		float edgeRate;

		void Load(const MMDsdk::PmdFile::Vertex& data);
		void Load(const MMDsdk::PmxFile::Vertex& data);
	};

	struct ModelTransform
	{
		MathUtil::Matrix world;
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
		MathUtil::float3 eye;
	};

	struct PixelShaderData
	{
		MathUtil::float3 lightDir;
	};

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

	Model(GUI::Graphics::Device& device);

	~Model();

	GUI::Result	Load(const char* const filepath);
	GUI::Result IsSuccessLoad() const;

	GUI::Result SetDefaultSceneData(const float aspectRatio);

	void Draw(GUI::Graphics::GraphicsCommand& command);
	
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	static const int sDefaultTextureCount = 2;
	static const int sDefaultToonTextureCount = 10;
	static const int sSceneDataCount = 2;
	int mDescriptorCount;

	GUI::Graphics::DescriptorHeap mHeap;
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mPS_DataBuffer;

	GUI::Graphics::ConstantBuffer mMaterialBuffer;

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

	MaterialInfo* mMaterialInfo;

	int mMaterialCount;

	GUI::Graphics::InputElementDesc inputLayout;
	GUI::Graphics::GraphicsPipeline mPipeline;
	GUI::Graphics::RootSignature mRootSignature;

	GUI::Graphics::GraphicsPipeline mOutlinePipeline;
	GUI::Graphics::RootSignature mOutlineSignature;

	GUI::Result isSuccessLoad;

	static const wchar_t* mToonPath[10];

	GUI::Graphics::Texture2D* mUniqueTexture;

	GUI::Graphics::Texture2D mDefaultTextureWhite;
	GUI::Graphics::Texture2D mDefaultTextureBlack;
	GUI::Graphics::Texture2D mDefaultTextureToon[10];

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);

	GUI::Result CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount);
	GUI::Result CreateIndexBuffer(const int index[], const int indexCount);
	GUI::Result CreateMaterialBuffer(const Material material[], const int materialCount);
	GUI::Result CreateTexture(const char* const dirPath, const char* const filename, const int texID);
};

#endif // !_MODEL_H_


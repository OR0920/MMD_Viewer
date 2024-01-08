#ifndef _MODEL_H_
#define _MODEL_H_

#include"GUI_Util.h"

#include<vector>
#include<string>

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

		void Load(const MMDsdk::PmdFile::Material& data);
		void Load(const MMDsdk::PmxFile::Material& data);
	};

	Model(GUI::Graphics::Device& device);

	~Model();

	GUI::Result	Load(const char* const filepath);
	GUI::Result IsSuccessLoad() const;
	const GUI::Graphics::VertexBuffer& GetVB() const;
	const GUI::Graphics::IndexBuffer& GetIB() const;

	GUI::Result SetDefaultSceneData();

	void Draw(GUI::Graphics::GraphicsCommand& command) const;

	void DebugOut() const;
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	GUI::Graphics::DescriptorHeap mHeap;
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mPS_DataBuffer;

	GUI::Graphics::ConstantBuffer mMaterialBuffer;

	struct MaterialInfo
	{
		int materialIndexCount;
		int texID = -1;
		int sphID = -1;
		int spaID = -1;
		int toonID = -1;
		bool isShared = true;

		void Load(const MMDsdk::PmdFile::Material& data);
		void Load(const MMDsdk::PmxFile::Material& data);
	};
	MaterialInfo* mMaterialInfo;

	int mMaterialCount;

	GUI::Graphics::InputElementDesc inputLayout;
	GUI::Graphics::GraphicsPipeline mPipeline;
	GUI::Graphics::RootSignature mRootSignature;

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);
	GUI::Result isSuccessLoad;

	std::vector<std::string> mTexPath;
	std::vector<std::wstring> mToonPath;

	GUI::Graphics::Texture2D* mUniqueTexture;

	// todo;
	GUI::Graphics::Texture2D mDefaultTextureWhite;
	GUI::Graphics::Texture2D mDefaultTextureBlack;
	GUI::Graphics::Texture2D mDefaultTextureToon[10];
};

#endif // !_MODEL_H_


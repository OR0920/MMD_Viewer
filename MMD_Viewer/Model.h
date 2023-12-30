#ifndef _MODEL_H_

#include"MathUtil.h"
#include"GUI_Util.h"

class Model
{
public:

	struct ModelVertex
	{
		MathUtil::float3 position;
		MathUtil::float3 normal;
	};

	struct ModelTransform
	{
		MathUtil::Matrix world;
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
	};

	struct PixelShaderData
	{
		MathUtil::float3 lightDir;
		float dammy;
		MathUtil::float4 testCol;
	};

	struct Material
	{
		MathUtil::float4 diffuse;
		MathUtil::float3 specular;
		float specularity;
		MathUtil::float3 ambient;
	};

	Model(GUI::Graphics::Device& device);

	~Model();

	GUI::Result	Load(const char* const filepath);
	GUI::Result IsSuccessLoad() const;
	const GUI::Graphics::VertexBuffer& GetVB() const;
	const GUI::Graphics::IndexBuffer& GetIB() const;

	GUI::Result SetDefaultSceneData();

	void Draw(GUI::Graphics::GraphicsCommand& command) const;
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	GUI::Graphics::DescriptorHeapForShaderData mHeap;
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mPS_DataBuffer;

	GUI::Graphics::ConstantBuffer mMaterialBuffer;
	int* mMaterialIndexCounts;
	int mMaterialCount;

	GUI::Graphics::InputElementDesc inputLayout;
	GUI::Graphics::GraphicsPipeline mPipeline;
	GUI::Graphics::RootSignature mRootSignature;

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);
	GUI::Result isSuccessLoad;
};

#endif // !_MODEL_H_


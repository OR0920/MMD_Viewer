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
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	GUI::Graphics::DescriptorHeapForShaderData mHeap;
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mMaterialBuffer;

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);
	GUI::Result isSuccessLoad;
};

#endif // !_MODEL_H_


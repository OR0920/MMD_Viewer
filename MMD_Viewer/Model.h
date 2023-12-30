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

	Model(GUI::Graphics::Device& device); ~Model();

	GUI::Result	Load(const char* const filepath);
	GUI::Result IsSuccessLoad() const;
	const GUI::Graphics::VertexBuffer& GetVB() const;
	const GUI::Graphics::IndexBuffer& GetIB() const;
private:
	GUI::Graphics::Device& mDevice;
	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);
	GUI::Result isSuccessLoad;
};

#endif // !_MODEL_H_


#ifndef _MODEL_H_
#define _MODEL_H_

#include"GUI_Util.h"

#include"MathUtil.h"
#include"MMDsdk.h"

#include"System.h"

class Model
{
public:
	// MMD���f���̒��_�\����
	struct ModelVertex
	{
		MathUtil::float3 position;
		MathUtil::float3 normal;
		MathUtil::float2 uv;
		float edgeRate;

		void Load(const MMDsdk::PmdFile::Vertex& data);
		void Load(const MMDsdk::PmxFile::Vertex& data);
	};

	// �ϊ��s��
	struct ModelTransform
	{
		MathUtil::Matrix world;
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
		MathUtil::float3 eye;
	};

	// �s�N�Z���V�F�[�_�[����Q�Ƃ����l
	struct PixelShaderData
	{
		MathUtil::float3 lightDir;
	};

	// �V�F�[�_�[����Q�Ƃ����}�e���A���̃f�[�^
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

	// �f�o�C�X��n���ď�����
	Model(GUI::Graphics::Device& device);

	~Model();

	// �ǂݍ���
	GUI::Result	Load(const char* const filepath);
	
	// �s����f�t�H���g�̒l�ɐݒ�
	GUI::Result SetDefaultSceneData(const float aspectRatio);

	// �`��
	void Draw(GUI::Graphics::GraphicsCommand& command);
	
private:
	GUI::Graphics::Device& mDevice;

	GUI::Graphics::VertexBuffer mVertexBuffer;
	GUI::Graphics::IndexBuffer mIndexBuffer;

	// �S���f�����ʂ̃f�B�X�N���v�^��
	static const int sDefaultTextureCount = 2;
	static const int sDefaultToonTextureCount = 10;
	static const int sSceneDataCount = 2;
	// ���f���ŗL�̂��̂��܂߂��f�B�X�N���v�^��
	int mDescriptorCount;

	// ���f���`�掞�Ƀo�C���h�����r���[�̃q�[�v
	GUI::Graphics::DescriptorHeap mHeap;

	// �萔�o�b�t�@
	GUI::Graphics::ConstantBuffer mTransformBuffer;
	GUI::Graphics::ConstantBuffer mPS_DataBuffer;
	GUI::Graphics::ConstantBuffer mMaterialBuffer;

	// �V�F�[�_�[����Q�Ƃ���Ȃ��}�e���A���̃f�[�^
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

	// �}�e���A�����̉ϒ��z��
	System::varray<MaterialInfo> mMaterialInfo;
	int mMaterialCount;

	// �p�C�v���C���̐ݒ�
	GUI::Graphics::InputElementDesc mInputLayout;
	GUI::Graphics::GraphicsPipeline mPipeline;
	GUI::Graphics::RootSignature mRootSignature;

	// �A�E�g���C���`�掞�̃p�C�v���C���ݒ�
	GUI::Graphics::GraphicsPipeline mOutlinePipeline;

	// �S���f�����L�̃g�D�[���e�N�X�`���t�@�C��
	static const wchar_t* mToonPath[10];

	// ���f���ŗL�̃e�N�X�`���f�[�^
	GUI::Graphics::Texture2D* mUniqueTexture;

	// ���f�����L�̃e�N�X�`���f�[�^
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


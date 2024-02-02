#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"OutlineVS.h"
#include"OutlinePS.h"

#include<memory>

#define ReturnIfFailed(func) if (func == GUI::Result::FAIL) return GUI::Result::FAIL;

void Model::ModelVertex::Load(const MMDsdk::PmdFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
	if (data.edgeFlag == MMDsdk::PmdFile::Vertex::EdgeEnable::VEE_ENABLE)
	{
		edgeRate = 1.f;
	}
	else
	{
		edgeRate = 0.f;
	}
}

void Model::ModelVertex::Load(const MMDsdk::PmxFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
	edgeRate = data.edgeRate;
}

void Model::Material::Load(const MMDsdk::PmdFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
	edgeColor = { 0.f, 0.f, 0.f, 1.f };
	edgeSize = 1.f;
}

void Model::Material::Load(const MMDsdk::PmxFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
	edgeColor = System::strong_cast<MathUtil::float4>(data.edgeColor);
	edgeSize = data.edgeSize;
}

const wchar_t* Model::mToonPath[] =
{
	L"DefaultTexture/toon01.bmp",
	L"DefaultTexture/toon02.bmp",
	L"DefaultTexture/toon03.bmp",
	L"DefaultTexture/toon04.bmp",
	L"DefaultTexture/toon05.bmp",
	L"DefaultTexture/toon06.bmp",
	L"DefaultTexture/toon07.bmp",
	L"DefaultTexture/toon08.bmp",
	L"DefaultTexture/toon09.bmp",
	L"DefaultTexture/toon10.bmp",
};

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mVertexBuffer(),
	mIndexBuffer(),
	mDescriptorCount(0),
	mHeap(),
	mTransformBuffer(),
	mPS_DataBuffer(),
	mMaterialBuffer(),
	mMaterialInfo(),
	mMaterialCount(0),
	mInputLayout({}),
	mPipeline({}),
	mRootSignature({}),
	mOutlinePipeline({}),
	mUniqueTexture(),
	mDefaultTextureWhite({}),
	mDefaultTextureBlack({}),
	mDefaultTextureToon()
{
	// ���_���C�A�E�g�̐ݒ�
	mInputLayout.SetElementCount(4);
	mInputLayout.SetDefaultPositionDesc();
	mInputLayout.SetDefaultNormalDesc();
	mInputLayout.SetDefaultUV_Desc();
	mInputLayout.SetFloatParam("EDGE_RATE");

	//�@���[�g�V�O�l�`���̍쐬
	mRootSignature.SetParameterCount(7);

	// param[0] : b0 : �s���
	// param[1] : b1 : �V�[���̃f�[�^
	// param[2] : b2 : �}�e���A���̃f�[�^
	mRootSignature.SetParamForCBV(0, 0);
	mRootSignature.SetParamForCBV(1, 1);
	mRootSignature.SetParamForCBV(2, 2);

	// param[3] : t0 : �ʏ�e�N�X�`��  
	// param[4] : t1 : ��Z�X�t�B�A�e�N�X�`��  
	// param[5] : t2 : ���Z�X�t�B�A�e�N�X�`��  
	// param[6] : t3 : Toon�e�N�X�`��  
	GUI::Graphics::DescriptorRange range;
	range.SetRangeCount(1);
	range.SetRangeForSRV(0, 0, 1);
	mRootSignature.SetParamForDescriptorTable(3, range);

	GUI::Graphics::DescriptorRange sphRange;
	sphRange.SetRangeCount(1);
	sphRange.SetRangeForSRV(0, 1, 1);
	mRootSignature.SetParamForDescriptorTable(4, sphRange);

	GUI::Graphics::DescriptorRange spaRange;
	spaRange.SetRangeCount(1);
	spaRange.SetRangeForSRV(0, 2, 1);
	mRootSignature.SetParamForDescriptorTable(5, spaRange);

	GUI::Graphics::DescriptorRange toonRange;
	toonRange.SetRangeCount(1);
	toonRange.SetRangeForSRV(0, 3, 1);
	mRootSignature.SetParamForDescriptorTable(6, toonRange);

	// s0 : �ʏ�A��Z�A���Z�e�N�X�`���p�̃T���v���[
	// s1 : Toon�p�T���v���[�@���W�����[�v���Ȃ�
	mRootSignature.SetStaticSamplerCount(2);
	mRootSignature.SetSamplerDefault(0, 0);
	mRootSignature.SetSamplerUV_Clamp(1, 1);

	mDevice.CreateRootSignature(mRootSignature);

	// �p�C�v���C���ݒ�
	mPipeline.SetRootSignature(mRootSignature);
	mPipeline.SetAlphaEnable(); // ����ON
	mPipeline.SetDepthEnable(); // �[�x�L��
	mPipeline.SetInputLayout(mInputLayout);
	mPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	mDevice.CreateGraphicsPipeline(mPipeline);

	// ���f���ɂ�炸�Œ肳�ꂽ�f�B�X�N���v�^�[�̐���ݒ�
	mDescriptorCount = sDefaultTextureCount + sDefaultToonTextureCount + sSceneDataCount;


	// �֊s���p�̐ݒ�

	mOutlinePipeline.SetInputLayout(mInputLayout);
	mOutlinePipeline.SetFrontCullEnable();
	mOutlinePipeline.SetRootSignature(mRootSignature);
	mOutlinePipeline.SetDepthEnable();
	mOutlinePipeline.SetAlphaEnable();
	mOutlinePipeline.SetVertexShader(SetShader(gOutlineVS));
	mOutlinePipeline.SetPixelShader(SetShader(gOutlinePS));
	mDevice.CreateGraphicsPipeline(mOutlinePipeline);
}

Model::~Model()
{
}

GUI::Result Model::Load(const char* const filepath)
{
	// ���ׂĂ̌`���œǂݍ���ł݂�
	if (LoadPMX(filepath) == GUI::Result::SUCCESS)
	{
		DebugMessage("Load pmx file !");
	}
	else if (LoadPMD(filepath) == GUI::Result::SUCCESS)
	{
		DebugMessage("Load pmd file !");
	}
	else
	{
		// �S�t�H�[�}�b�g�œǂݍ��ݎ��s�����烊�^�[��
		return GUI::Result::FAIL;
	}

	DebugMessage("Model Load Succeeded !");

	ReturnIfFailed(this->CreateDefaultBufferData());

	DebugMessage("Default Buffer Created !");

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateDefaultBufferData()
{
	// �V�[�����̏�����
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform))
	);
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData))
	);

	// �f�t�H���g�̃e�N�X�`�������
	ReturnIfFailed(mDefaultTextureWhite.LoadFromFile(L"DefaultTexture/White.png"));
	ReturnIfFailed(mDevice.CreateTexture2D(mDefaultTextureWhite, mHeap));

	ReturnIfFailed(mDefaultTextureBlack.LoadFromFile(L"DefaultTexture/Black.png"));
	ReturnIfFailed(mDevice.CreateTexture2D(mDefaultTextureBlack, mHeap));

	for (int i = 0; i < 10; ++i)
	{
		auto& t = mDefaultTextureToon[i];

		ReturnIfFailed(t.LoadFromFile(mToonPath[i]));

		ReturnIfFailed(mDevice.CreateTexture2D(t, mHeap));
	}
}

void Model::Update()
{
	// ��]�p�v�Z
	static int frameCount = 0;
	static float rotUnit = 1.f;
	frameCount++;
	frameCount %= static_cast<int>(3600.f / (rotUnit * 10.f));
	float rotation = rotUnit * frameCount;

	// ���f������]������s�����������
	ModelTransform* transform = nullptr;
	mTransformBuffer.Map(reinterpret_cast<void**>(&transform));
	transform->world = MathUtil::Matrix::GenerateMatrixRotationY(MathUtil::DegreeToRadian(rotation));
	mTransformBuffer.Unmap();
}

void Model::Draw(GUI::Graphics::GraphicsCommand& command)
{
	// ���f���`��p�̃p�C�v���C���ƃ��[�g�V�O�l�`�����Z�b�g
	command.SetGraphicsPipeline(mPipeline);
	command.SetGraphicsRootSignature(mRootSignature);

	// �r���[��p���ăV�[���̃f�[�^���o�C���h
	command.SetDescriptorHeap(mHeap);
	command.SetConstantBuffer(mTransformBuffer, 0);
	command.SetConstantBuffer(mPS_DataBuffer, 1);

	// ���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@���Z�b�g
	command.SetVertexBuffer(mVertexBuffer, mIndexBuffer);

	
	// �}�e���A�����Ƀ��b�V����`��
	int indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		auto& info = mMaterialInfo[i];
		auto indexCount = info.materialIndexCount;

		// �}�e���A���Z�b�g
		command.SetConstantBuffer(mMaterialBuffer, 2, i);

		// �e�e�N�X�`�����Z�b�g
		// �ŗL�e�N�X�`���������Ȃ��ꍇ�f�t�H���g�̃e�N�X�`����n��
		if (info.texID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.texID], 3);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 3);
		}

		if (info.sphID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.sphID], 4);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 4);
		}

		if (info.spaID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.spaID], 5);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureBlack, 5);
		}

		// �g�D�[����
		// �E�ŗL�e�N�X�`����������(PMX�݂̂̋@�\)
		// �E���L�e�N�X�`����������
		// �E�g�D�[���e�N�X�`���������Ȃ�����
		// ������
		if (info.toonID != -1)
		{
			if (info.isShared == true)
			{
				command.SetDescriptorTable(mDefaultTextureToon[info.toonID], 6);
			}
			else
			{
				command.SetDescriptorTable(mUniqueTexture[info.toonID], 6);
			}
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 6);
		}

		// �O�̃}�e���A���̑�������A���b�V����`��
		command.DrawTriangleList(indexCount, indexOffs);
		indexOffs += indexCount;
	}


	// �֊s���`��
	command.SetGraphicsPipeline(mOutlinePipeline);

	indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		if (mMaterialInfo[i].isEdgeEnable == false)
		{
			indexOffs += mMaterialInfo[i].materialIndexCount;
			continue;
		}

		command.SetConstantBuffer(mMaterialBuffer, 2, i);

		command.DrawTriangleList(mMaterialInfo[i].materialIndexCount, indexOffs);
		indexOffs += mMaterialInfo[i].materialIndexCount;
	}
}

// PMD����ǂ݂���
void Model::MaterialInfo::Load(const MMDsdk::PmdFile::Material& data)
{
	materialIndexCount = data.vertexCount;
	toonID = static_cast<int>(data.toonIndex);
	isShared = true;
	if (data.edgeFlag == MMDsdk::PmdFile::Material::EdgeEnable::MEE_ENABLE)
	{
		isEdgeEnable = true;
	}
	else
	{
		isEdgeEnable = false;
	}
}

// PMX����ǂ݂���
void Model::MaterialInfo::Load(const MMDsdk::PmxFile::Material& data)
{
	texID = data.textureID;
	if (data.sphereMode == MMDsdk::PmxFile::Material::SphereMode::SM_SPH)
	{
		sphID = data.sphereTextureID;
	}
	else if (data.sphereMode == MMDsdk::PmxFile::Material::SphereMode::SM_SPA)
	{
		spaID = data.sphereTextureID;
	}

	toonID = data.toonTextureID;
	if (data.toonMode == MMDsdk::PmxFile::Material::ToonMode::TM_SHARED)
	{
		isShared = true;
	}
	else
	{
		isShared = false;
	}

	materialIndexCount = data.vertexCount;

	isEdgeEnable = data.GetDrawConfig(MMDsdk::PmxFile::Material::DC_DRAW_EDGE);
}

GUI::Result Model::LoadPMD(const char* const filepath)
{
	// PMD�Ƃ��ĊJ���Ȃ���Ύ��s
	MMDsdk::PmdFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}

	// ���_�f�[�^��ǂݍ��݁A�o�b�t�@�����
	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		System::varray<ModelVertex> mesh(vCount);
		//ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		ReturnIfFailed(CreateVertexBuffer(mesh.GetStart(), vCount));
	}

	// �C���f�b�N�X�f�[�^��ǂݍ��݁A�o�b�t�@�����
	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		System::varray<int> index(iCount);
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		ReturnIfFailed(CreateIndexBuffer(index.GetStart(), iCount));
	}

	// �}�e���A�������擾
	mMaterialCount = file.GetMaterialCount();

	// �}�e���A����0�Ȃ�A�ŗL�e�N�X�`����0�Ȃ̂ŁA�q�[�v����胊�^�[��
	if (mMaterialCount == 0)
	{
		ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));
		return GUI::Result::SUCCESS;
	}

	// PMD�̏ꍇ�A�e�N�X�`���t�@�C������
	// tex1.bmp*tex2.sph�@�̂悤�Ɍ������ꂽ��Ԃ�
	// �e�}�e���A���ɋL�^����Ă���B
	// 
	// ��ނ��Ƃɕ������L�^���Ă���
	// pmd�̃p�X�̒�����20�Œ�
	auto& pmdTexLength = MMDsdk::PmdFile::TexPathLength;

	struct TexPaths
	{
		char tex[pmdTexLength] = { '\0' };
		char sph[pmdTexLength] = { '\0' };
		char spa[pmdTexLength] = { '\0' };
	};


	// �e�N�X�`�����𕪊�����ۂɎg���\����

	// �e�N�X�`�����̒����ƊJ�n�ʒu
	struct SubString
	{
		const char* start = nullptr;
		int length = 0;
	};
	System::varray<TexPaths> texPathPerMaterial(mMaterialCount);

	// �e�N�X�`�����𕪊����ċL�^����\����
	// ���ꂼ��̃p�X���̊J�n�ʒu���L�^����
	struct TexPathSignature
	{
		SubString path[2];
		int pathCount = 0;
		void Load(const char* const str, const int length)
		{
			// �e�N�X�`����Ɖ��肵�ď�����
			path[0].start = &str[0];
			path[0].length = length;

			// 1�ڃJ�E���g
			pathCount++;

			// ��������'*'��T��
			for (int i = 0; i < length; ++i)
			{
				if (str[i] == '*')
				{
					// �e�p�X�̒����ƊJ�n�ʒu���L�^���Ă���
					path[1].start = &str[i + 1];

					path[0].length = i;
					path[1].length = length - i;

					// 2�ڃJ�E���g
					pathCount++;
					break;
				}
				if (str[i] == '\0')
				{
					// ���[�ɂ��ǂ蒅����������
					return;
				}
			}
		}
	};

	System::varray<TexPathSignature> pathSignature(mMaterialCount);

	// �e�N�X�`����
	int tCount = 0;

	if (mMaterialCount != 0)
	{
		// �V�F�[�_�[���W�X�^�ɒu���}�e���A�����
		System::varray<Material> material(mMaterialCount);
		// �V�F�[�_�[���W�X�^�ɒu���Ȃ��}�e���A�����
		mMaterialInfo.Init(mMaterialCount);

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);
			material[i].Load(m);
			mMaterialInfo[i].Load(m);

			// �e�N�X�`�����擾
			auto tp = m.texturePath.GetText();

			// �e�N�X�`���������Ȃ��}�e���A���Ȃ̂Ŏ�
			if (tp[0] == '\0') continue;

			// �p�X����́A����
			auto tpLength = m.texturePath.GetLength();
			pathSignature[i].Load(tp, tpLength);

			// �e�e�N�X�`������ނ��ƂɃR�s�[
			for (int j = 0; j < pathSignature[i].pathCount; ++j)
			{
				// Tex1.bmp*Tex2.sph�@�Ƃ����悤��
				// ��ڂɂ�NULL���������݂��Ȃ����߁A���o�b�t�@�ɃR�s�[����
				char path[pmdTexLength] = { '\0' };
				auto subStr = pathSignature[i].path[j];

				memcpy_s(&path[0], pmdTexLength, subStr.start, subStr.length);

				// �g���q�ŐU�蕪���@
				// PMD�̎d�l�ł́A�g���q�Ńe�N�X�`���̋@�\���ς��
				auto* ext = System::GetExt(path);

				auto& p = texPathPerMaterial[i];
				auto& mifo = mMaterialInfo[i];
				if (System::StringEqual(ext, ".sph") == true)
				{
					memcpy_s(p.sph, pmdTexLength, path, pmdTexLength);
					// �e�N�X�`����ǂݍ��񂾏��Ԃ�ID��Ή�������
					mifo.sphID = tCount;
					tCount++;// �e�N�X�`�����𐔂���
				}
				else if (System::StringEqual(ext, ".spa") == true)
				{
					memcpy_s(p.spa, pmdTexLength, path, pmdTexLength);
					mifo.spaID = tCount;
					tCount++;
				}
				else
				{
					//�@��L�ȊO�̊g���q�͂��ׂĒʏ�e�N�X�`���Ƃ���
					memcpy_s(p.tex, pmdTexLength, path, pmdTexLength);
					mifo.texID = tCount;
					tCount++;
				}
			}
		}

		// �f�B�X�N���v�^�̐����X�V���A�q�[�v�쐬
		mDescriptorCount += mMaterialCount + tCount;
		ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));

		// �}�e���A���̒萔�o�b�t�@���쐬
		ReturnIfFailed(CreateMaterialBuffer(material.GetStart(), mMaterialCount));
	}

	// �e�N�X�`����ǂݍ���
	// �Ƃ肠�����V���v���ɂ��Ă��������̂ŁA�����e�N�X�`�����낤�����̂܂ܓǂݍ���
	if (tCount != 0)
	{
		mUniqueTexture.Init(tCount);

		int texID = 0;
		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto p = texPathPerMaterial[i];

			if (p.tex[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.tex, texID));
				texID++;
			}

			if (p.sph[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.sph, texID));
				texID++;
			}

			if (p.spa[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.spa, texID));
				texID++;
			}
		}
	}

	return GUI::Result::SUCCESS;
}


GUI::Result Model::LoadPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}

	// ���_�f�[�^��ǂݍ��݃o�b�t�@���쐬
	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		System::varray<ModelVertex> mesh(vCount);
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		ReturnIfFailed(CreateVertexBuffer(mesh.GetStart(), vCount));
	}

	// �C���f�b�N�X�f�[�^��ǂݍ��݃o�b�t�@���쐬
	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		System::varray<int> index(iCount);
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		ReturnIfFailed(CreateIndexBuffer(index.GetStart(), iCount));
	}

	// �}�e���A���ƃe�N�X�`����ǂݍ���

	// PMD�ƈقȂ�A�e�N�X�`���ƃ}�e���A�����������Ă��邽��
	// ���̒i�K�Ńf�B�X�N���v�^�����m�肷��
	mMaterialCount = file.GetMaterialCount();
	auto tCount = file.GetTextureCount();

	mDescriptorCount += mMaterialCount + tCount;
	ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));

	// �}�e���A����ǂݍ��݃o�b�t�@���쐬
	if (mMaterialCount != 0)
	{
		System::varray<Material> material(mMaterialCount);
		mMaterialInfo.Init(mMaterialCount);

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);

			material[i].Load(m);
			mMaterialInfo[i].Load(m);
		}

		ReturnIfFailed(CreateMaterialBuffer(material.GetStart(), mMaterialCount));
	}

	// �e�N�X�`����ǂݍ��݃o�b�t�@���쐬
	if (tCount != 0)
	{
		mUniqueTexture.Init(tCount);

		for (int i = 0; i < tCount; ++i)
		{
			ReturnIfFailed
			(
				CreateTexture
				(
					file.GetDirectoryPath(),
					file.GetTexturePath(i).GetText(),
					i
				)
			);
		}
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount)
{
	// �o�b�t�@������ăR�s�[
	ReturnIfFailed(mDevice.CreateVertexBuffer(mVertexBuffer, sizeof(ModelVertex), vertexCount));
	ReturnIfFailed(mVertexBuffer.Copy(vertex));

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateIndexBuffer(const int index[], const int indexCount)
{
	// �o�b�t�@������ăR�s�[
	ReturnIfFailed(mDevice.CreateIndexBuffer(mIndexBuffer, sizeof(int), indexCount));
	ReturnIfFailed(mIndexBuffer.Copy(index));

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateMaterialBuffer(const Material material[], const int materialCount)
{
	// �o�b�t�@������ă}�b�v���ăR�s�[
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer
		(
			mMaterialBuffer,
			mHeap,
			sizeof(Material),
			materialCount
		)
	);

	unsigned char* mappedMaterial = nullptr;
	ReturnIfFailed(mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)));

	for (int i = 0; i < materialCount; ++i)
	{
		*reinterpret_cast<Material*>(mappedMaterial) = material[i];
		// 256�o�C�g�ɃA���C�������g����Ă��邽�߁Asizeof(Material)�ł͂Ȃ�
		mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();
	}

	mMaterialBuffer.Unmap();

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateTexture(const char* const dirPath, const char* const filename, const int texID)
{
	// �f�B���N�g����nullptr�͂��肦�Ȃ�
	if (dirPath == nullptr)	return GUI::Result::FAIL;

	// �e�N�X�`�����Ȃ��ꍇ�́A���s�ł͂Ȃ��̂Ő����Ƃ��ĕԂ�
	if (filename == nullptr) return GUI::Result::SUCCESS;
	if (texID == -1) return GUI::Result::SUCCESS;

	// �p�X������
	char* filepath = nullptr;
	System::newArray_CopyAssetPath(&filepath, dirPath, filename);

	// �e�N�X�`����ǂݍ���
	auto result = mUniqueTexture[texID].LoadFromFile(filepath);

	System::SafeDeleteArray(&filepath);

	ReturnIfFailed(result);

	// �e�N�X�`�����쐬
	ReturnIfFailed(mDevice.CreateTexture2D(mUniqueTexture[texID], mHeap))

		return GUI::Result::SUCCESS;
}


GUI::Result Model::SetDefaultSceneData(const float aspectRatio)
{
	// �s��f�[�^�A�V�[���f�[�^���}�b�v���R�s�[
	// �f�[�^�͈�����Ȃ̂ŁA���̂܂܃N���X�̃|�C���^���g�p����B
	ModelTransform* mappedTransform = nullptr;
	ReturnIfFailed(mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)));
	// ���_
	auto eye = MathUtil::Vector(0.f, 10.f, -30.f);
	mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
	mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
	(
		eye,
		MathUtil::Vector::basicZ,
		MathUtil::Vector::basicY
	);
	mappedTransform->proj = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
	(
		MathUtil::PI_DIV4,
		aspectRatio,
		0.1f,
		1000.f
	);
	mappedTransform->eye = eye.GetFloat3();
	mTransformBuffer.Unmap();

	PixelShaderData* mappedPS_Data = nullptr;
	ReturnIfFailed(mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)));

	// ���C�g�̕���
	mappedPS_Data->lightDir = MathUtil::Vector(-1.f, -1.f, 1.f).GetFloat3();
	mPS_DataBuffer.Unmap();


	return GUI::Result::SUCCESS;
}

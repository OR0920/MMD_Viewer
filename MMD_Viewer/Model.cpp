#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include<Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

void Model::ModelVertex::Load(const MMDsdk::PmdFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
}

void Model::ModelVertex::Load(const MMDsdk::PmxFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
}

void Model::Material::Load(const MMDsdk::PmdFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
}

void Model::Material::Load(const MMDsdk::PmxFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
}

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mVB(),
	mIB(),
	mHeap(),
	mTransformBuffer(),
	mPS_DataBuffer(),
	mMaterialBuffer(),
	mMaterialCount(0),
	inputLayout({}),
	mPipeline({}),
	mRootSignature({}),
	isSuccessLoad(GUI::Result::FAIL),
	mTexPath(),
	mToonPath(10)
{
	inputLayout.SetElementCount(3);
	inputLayout.SetDefaultPositionDesc();
	inputLayout.SetDefaultNormalDesc();
	inputLayout.SetDefaultUV_Desc();

	mRootSignature.SetParameterCount(7);
	mRootSignature.SetParamForCBV(0, 0);
	mRootSignature.SetParamForCBV(1, 1);
	mRootSignature.SetParamForCBV(2, 2);

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

	mRootSignature.SetStaticSamplerCount(2);
	mRootSignature.SetSamplerDefault(0, 0);
	mRootSignature.SetSamplerUV_Clamp(1, 1);

	mDevice.CreateRootSignature(mRootSignature);

	mPipeline.SetRootSignature(mRootSignature);
	mPipeline.SetAlphaEnable();
	mPipeline.SetCullDisable();
	mPipeline.SetDepthEnable();
	mPipeline.SetInputLayout(inputLayout);
	mPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	mDevice.CreateGraphicsPipeline(mPipeline);

	mToonPath =
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


}


Model::~Model()
{
	System::SafeDeleteArray(&mMaterialInfo);
	System::SafeDeleteArray(&mUniqueTexture);
}


GUI::Result Model::Load(const char* const filepath)
{
	isSuccessLoad = GUI::Result::FAIL;

	mDescriptorCount = sDefaultTextureCount + sDefaultToonTextureCount + sSceneDataCount;

	if (LoadPMD(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}
	else if (LoadPMX(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}

	if (isSuccessLoad == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	DebugMessage("Model Load Succeeded !");

	if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDefaultTextureWhite.LoadFromFile(L"DefaultTexture/White.png") == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDevice.CreateTexture2D(mDefaultTextureWhite, mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDefaultTextureBlack.LoadFromFile(L"DefaultTexture/Black.png") == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDevice.CreateTexture2D(mDefaultTextureBlack, mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	for (int i = 0; i < 10; ++i)
	{
		auto& t = mDefaultTextureToon[i];

		if (t.LoadFromFile(mToonPath[i].c_str()) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateTexture2D(t, mHeap) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	return isSuccessLoad;
}

GUI::Result Model::IsSuccessLoad() const
{
	return isSuccessLoad;
}

void Model::Draw(GUI::Graphics::GraphicsCommand& command)
{
	command.SetGraphicsPipeline(mPipeline);
	command.SetGraphicsRootSignature(mRootSignature);

	command.SetDescriptorHeap(mHeap);
	command.SetConstantBuffer(mTransformBuffer, 0);
	command.SetConstantBuffer(mPS_DataBuffer, 1);

	command.SetVertexBuffer(mVB, mIB);

	// ‰ñ“]ŠpŒvŽZ
	static int frameCount = 0;
	static float rotUnit = 1.f;
	frameCount++;
	frameCount %= static_cast<int>(3600.f / (rotUnit * 10.f));
	float rotation = rotUnit * frameCount;


	ModelTransform* transform = nullptr;
	mTransformBuffer.Map(reinterpret_cast<void**>(&transform));
	transform->world = MathUtil::Matrix::GenerateMatrixRotationY(MathUtil::DegreeToRadian(rotation));
	mTransformBuffer.Unmap();

	int indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		auto& info = mMaterialInfo[i];
		auto indexCount = info.materialIndexCount;

		command.SetConstantBuffer(mMaterialBuffer, 2, i);

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

		command.DrawTriangleList(indexCount, indexOffs);
		indexOffs += indexCount;
	}
}

void Model::MaterialInfo::Load(const MMDsdk::PmdFile::Material& data)
{
	materialIndexCount = data.vertexCount;
	toonID = static_cast<int>(data.toonIndex);
	isShared == true;
}

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
}

GUI::Result Model::LoadPMD(const char* const filepath)
{
	MMDsdk::PmdFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}

	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		if (CreateVertexBuffer(mesh, vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&mesh);
	}


	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (CreateIndexBuffer(index, iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);
	}


	mMaterialCount = file.GetMaterialCount();

	if (mMaterialCount == 0)
	{
		if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}


	struct TexPaths
	{
		const char* tex = nullptr;
		const char* sph = nullptr;
		const char* spa = nullptr;
	};

	TexPaths* paths = nullptr;
	
	int tCount = 0;

	if (mMaterialCount != 0)
	{
		Material* material = new Material[mMaterialCount];
		mMaterialInfo = new MaterialInfo[mMaterialCount];
		//texpath = new const char* [mMaterialCount] { nullptr };

		paths = new TexPaths[mMaterialCount];

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);
			material[i].Load(m);
			mMaterialInfo[i].Load(m);

			auto tp = m.texturePath.GetText();
			if (tp[0] != '\0')
			{
				wchar_t* wtp = nullptr;
				System::newArray_CreateWideCharStrFromMultiByteStr(&wtp, tp);
				wchar_t* ext = PathFindExtension(wtp);

				auto& p = paths[i];
				auto& mifo = mMaterialInfo[i];
				if (System::StringEqual(ext, L".sph") == true)
				{
					p.sph = tp;
					mifo.sphID = tCount;
					tCount++;
				}
				else if (System::StringEqual(ext, L".spa") == true)
				{
					p.spa = tp;
					mifo.spaID = tCount;
					tCount++;
				}
				else
				{
					p.tex = tp;
					mifo.texID = tCount;
					tCount++;
				}

				System::SafeDeleteArray(&wtp);
			}
		}

		mDescriptorCount += mMaterialCount + tCount;
		if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&material);
			System::SafeDeleteArray(&paths);
			return GUI::Result::FAIL;
		}

		if (CreateMaterialBuffer(material, mMaterialCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&material);
			System::SafeDeleteArray(&paths);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&material);
	}

	if (tCount != 0)
	{
		mUniqueTexture = new GUI::Graphics::Texture2D[tCount];


		int texID = 0;
		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto p = paths[i];
			
			if (p.tex != nullptr)
			{
				if (CreateTexture(file.GetDirectoryPath(), p.tex, texID) == GUI::Result::FAIL)
				{
					System::SafeDeleteArray(&paths);
					return GUI::Result::FAIL;
				}
				texID++;
			}
			
			if (p.sph != nullptr)
			{
				if (CreateTexture(file.GetDirectoryPath(), p.sph, texID) == GUI::Result::FAIL)
				{
					System::SafeDeleteArray(&paths);
					return GUI::Result::FAIL;
				}
				texID++;
			}

			if (p.spa != nullptr)
			{
				if (CreateTexture(file.GetDirectoryPath(), p.spa, texID) == GUI::Result::FAIL)
				{
					System::SafeDeleteArray(&paths);
					return GUI::Result::FAIL;
				}
				texID++;
			}
		}	
	}

	System::SafeDeleteArray(&paths);

	return GUI::Result::SUCCESS;
}


GUI::Result Model::LoadPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}


	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		if (CreateVertexBuffer(mesh, vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}


		System::SafeDeleteArray(&mesh);
	}

	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (CreateIndexBuffer(index, iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);
	}

	mMaterialCount = file.GetMaterialCount();
	auto tCount = file.GetTextureCount();

	mDescriptorCount += mMaterialCount + tCount;

	if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mMaterialCount != 0)
	{
		Material* material = new Material[mMaterialCount];
		mMaterialInfo = new MaterialInfo[mMaterialCount];

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);

			material[i].Load(m);
			mMaterialInfo[i].Load(m);
		}

		if (CreateMaterialBuffer(material, mMaterialCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&material);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&material);
	}

	if (tCount != 0)
	{
		mUniqueTexture = new GUI::Graphics::Texture2D[tCount];

		for (int i = 0; i < tCount; ++i)
		{
			if (CreateTexture(file.GetDirectoryPath(), file.GetTexturePath(i).GetText(), i) == GUI::Result::FAIL)
			{
				return GUI::Result::FAIL;
			}
		}
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount)
{
	if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vertexCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	};

	if (mVB.Copy(vertex) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateIndexBuffer(const int index[], const int indexCount)
{
	if (mDevice.CreateIndexBuffer(mIB, sizeof(int), indexCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mIB.Copy(index) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateMaterialBuffer(const Material material[], const int materialCount)
{
	if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), materialCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	unsigned char* mappedMaterial = nullptr;
	if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
	{
		for (int i = 0; i < materialCount; ++i)
		{
			*reinterpret_cast<Material*>(mappedMaterial) = material[i];
			mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();
		}

		return GUI::Result::SUCCESS;
	}

	return GUI::Result::FAIL;
}

GUI::Result Model::CreateTexture(const char* const dirPath, const char* const filename, const int texID)
{
	char* tFilePath = nullptr;
	System::newArray_CopyAssetPath(&tFilePath, dirPath, filename);

	wchar_t* filepath = nullptr;
	System::newArray_CreateWideCharStrFromMultiByteStr(&filepath, tFilePath);

	if (mUniqueTexture[texID].LoadFromFile(filepath) == GUI::Result::FAIL)
	{
		System::SafeDeleteArray(&tFilePath);
		System::SafeDeleteArray(&filepath);
		return GUI::Result::FAIL;
	}

	System::SafeDeleteArray(&tFilePath);
	System::SafeDeleteArray(&filepath);

	if (mDevice.CreateTexture2D(mUniqueTexture[texID], mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}
}


GUI::Result Model::SetDefaultSceneData()
{
	ModelTransform* mappedTransform = nullptr;
	if (mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
	{
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
			DirectX::XM_PIDIV4,
			static_cast<float>(1280) / static_cast<float>(720),
			0.1f,
			1000.f
		);
		mappedTransform->eye = eye.GetFloat3();
		mTransformBuffer.Unmap();
	}
	else
	{
		return GUI::Result::FAIL;
	}

	PixelShaderData* mappedPS_Data = nullptr;
	if (mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)) == GUI::Result::SUCCESS)
	{
		mappedPS_Data->lightDir = MathUtil::Vector(-1.f, -1.f, 1.f).GetFloat3();
		mPS_DataBuffer.Unmap();
	}
	else
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;

}

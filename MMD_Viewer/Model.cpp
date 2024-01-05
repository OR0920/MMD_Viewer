#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

void Model::ModelVertex::Load(const MMDsdk::PmdFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
}

void Model::ModelVertex::Load(const MMDsdk::PmxFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
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
	inputLayout.SetElementCount(2);
	inputLayout.SetDefaultPositionDesc();
	inputLayout.SetDefaultNormalDesc();

	mRootSignature.SetParameterCount(3);
	mRootSignature.SetParamForCBV(0, 0);
	mRootSignature.SetParamForCBV(1, 1);

	GUI::Graphics::DescriptorRange range;
	range.SetRangeCount(1);
	range.SetRangeForCBV(0, 2, 1);
	mRootSignature.SetParamForDescriptorTable(2, range);

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
		"toon01.bmp",
		"toon02.bmp",
		"toon03.bmp",
		"toon04.bmp",
		"toon05.bmp",

		"toon06.bmp",
		"toon07.bmp",
		"toon08.bmp",
		"toon09.bmp",
		"toon10.bmp",
	};
}


Model::~Model()
{
	System::SafeDeleteArray(&mMaterialInfo);
}


GUI::Result Model::Load(const char* const filepath)
{
	isSuccessLoad = GUI::Result::FAIL;

	if (LoadPMD(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}
	else if (LoadPMX(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}

	return isSuccessLoad;
}

GUI::Result Model::IsSuccessLoad() const
{
	return isSuccessLoad;
}

const GUI::Graphics::VertexBuffer& Model::GetVB() const
{
	return mVB;
}

const GUI::Graphics::IndexBuffer& Model::GetIB() const
{
	return mIB;
}

void Model::Draw(GUI::Graphics::GraphicsCommand& command) const
{
	command.SetGraphicsPipeline(mPipeline);
	command.SetGraphicsRootSignature(mRootSignature);

	command.SetDescriptorHeap(mHeap);
	command.SetConstantBuffer(mTransformBuffer, 0);
	command.SetConstantBuffer(mPS_DataBuffer, 1);

	command.SetVertexBuffer(mVB, mIB);

	int indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		auto indexCount = mMaterialInfo[i].materialIndexCount;
		command.SetDescriptorTable(mMaterialBuffer, i, 2);
		command.DrawTriangleList(indexCount, indexOffs);
		indexOffs += indexCount;
	}
}

void Model::DebugOut() const
{
	for (auto& tp : mTexPath)
	{
		DebugOutString(tp.c_str());
	}
}

void Model::MaterialInfo::Load(const MMDsdk::PmdFile::Material& data)
{
	materialIndexCount = data.vertexCount;
	toonID = data.toonIndex;
	isShared = true;
}

void Model::MaterialInfo::Load(const MMDsdk::PmxFile::Material& data)
{
	materialIndexCount = data.vertexCount;
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
}

GUI::Result Model::LoadPMD(const char* const filepath)
{
	MMDsdk::PmdFile file(filepath);
	if (file.IsSuccessLoad() == true)
	{
		auto vCount = file.GetVertexCount();
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		};

		if (mVB.Copy(mesh) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&mesh);

		auto iCount = file.GetIndexCount();
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (mDevice.CreateIndexBuffer(mIB, sizeof(int), iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		if (mIB.Copy(index) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);



		mMaterialCount = file.GetMaterialCount();
		auto descriptorCount = 1 + 1 + mMaterialCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), mMaterialCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		};

		unsigned char* mappedMaterial = nullptr;
		System::SafeDeleteArray(&mMaterialInfo);
		if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
		{
			mMaterialInfo = new MaterialInfo[mMaterialCount]{};
			for (int i = 0; i < mMaterialCount; ++i)
			{
				auto& m = file.GetMaterial(i);

				reinterpret_cast<Material*>(mappedMaterial)->Load(m);
				mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();

				mMaterialInfo[i].Load(m);
			}
			mMaterialBuffer.Unmap();
		}
		else
		{
			return GUI::Result::FAIL;
		}

		return GUI::Result::SUCCESS;
	}
	else
	{
		return GUI::Result::FAIL;
	}
}


GUI::Result Model::LoadPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);
	if (file.IsSuccessLoad() == true)
	{
		auto vCount = file.GetVertexCount();
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		};

		if (mVB.Copy(mesh) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&mesh);

		auto iCount = file.GetIndexCount();
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (mDevice.CreateIndexBuffer(mIB, sizeof(int), iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		if (mIB.Copy(index) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);


		mMaterialCount = file.GetMaterialCount();
		auto descriptorCount = 1 + 1 + mMaterialCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), mMaterialCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		};


		unsigned char* mappedMaterial = nullptr;
		System::SafeDeleteArray(&mMaterialInfo);
		if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
		{
			mMaterialInfo = new MaterialInfo[mMaterialCount]{};
			for (int i = 0; i < mMaterialCount; ++i)
			{
				auto& m = file.GetMaterial(i);

				reinterpret_cast<Material*>(mappedMaterial)->Load(m);
				mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();

				mMaterialInfo[i].Load(m);
			}
			mMaterialBuffer.Unmap();
		}
		else
		{
			return GUI::Result::FAIL;
		}

		std::string dirPath = file.GetDirectoryPath();

		mTexPath.resize(file.GetTextureCount());
		for (int i = 0; i < mTexPath.size(); ++i)
		{
			std::string path = file.GetTexturePath(i).GetText();
			mTexPath[i] = dirPath + path;
		}

		return GUI::Result::SUCCESS;

	}
	else
	{
		return GUI::Result::FAIL;
	}


}

GUI::Result Model::SetDefaultSceneData()
{
	ModelTransform* mappedTransform = nullptr;
	if (mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
	{
		auto eye = MathUtil::Vector(0.f, 10.f, -50.f);
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

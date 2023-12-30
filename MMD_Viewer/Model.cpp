#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mHeap(),
	mVB(),
	mIB(),
	mMaterialBuffer(),
	isSuccessLoad(GUI::Result::FAIL)
{
	inputLayout.SetElementCount(2);
	inputLayout.SetDefaultPositionDesc();
	inputLayout.SetDefaultNormalDesc();

	mRootSignature.SetParameterCount(3);
	mRootSignature.SetParamForCBV(0, 0);
	mRootSignature.SetParamForCBV(1, 1);
	mRootSignature.SetParamForCBV(2, 2);
	mDevice.CreateRootSignature(mRootSignature);

	mPipeline.SetRootSignature(mRootSignature);
	mPipeline.SetAlphaEnable();
	mPipeline.SetDepthEnable();
	mPipeline.SetInputLayout(inputLayout);
	mPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	mDevice.CreateGraphicsPipeline(mPipeline) == GUI::Result::FAIL;
}


Model::~Model() {}


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
	command.SetConstantBuffer(mMaterialBuffer, 2);

	command.DrawTriangleList(mVB, mIB);
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
			auto& v = file.GetVertex(i);
			mesh[i].position = System::strong_cast<MathUtil::float3>(v.position);
			mesh[i].normal = System::strong_cast<MathUtil::float3>(v.normal);
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



		auto mtCount = file.GetMaterialCount();

		auto descriptorCount = 1 + 1 + mtCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		ModelTransform* mappedTransform = nullptr;
		if (mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
		{
			mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
			mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
			(
				MathUtil::Vector(0.f, 10.f, -50.f),
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
		}
		
		if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		PixelShaderData* mappedPS_Data = nullptr;
		if (mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)) == GUI::Result::SUCCESS)
		{
			mappedPS_Data->lightDir = { -1.f, -1.f, 1.f };
		}

		if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), mtCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		};

		Material* mappedMaterial = nullptr;
		if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
		{
			for (int i = 0; i < mtCount; ++i)
			{
				auto& mt = mappedMaterial[i];
				auto& mtf = file.GetMaterial(i);
				mt.diffuse = System::strong_cast<MathUtil::float4>(mtf.diffuse);
				mt.specular = System::strong_cast<MathUtil::float3>(mtf.specular);
				mt.specularity = mtf.specularity;
				mt.ambient = System::strong_cast<MathUtil::float3>(mtf.ambient);
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
			auto& v = file.GetVertex(i);
			mesh[i].position = System::strong_cast<MathUtil::float3>(v.position);
			mesh[i].normal = System::strong_cast<MathUtil::float3>(v.normal);
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

		auto mtCount = file.GetMaterialCount();

		auto descriptorCount = 1 + 1 + mtCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		ModelTransform* mappedTransform = nullptr;
		if (mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
		{
			mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
			mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
			(
				MathUtil::Vector(0.f, 10.f, -50.f),
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
		}

		if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		PixelShaderData* mappedPS_Data = nullptr;
		if (mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)) == GUI::Result::SUCCESS)
		{
			mappedPS_Data->lightDir = { -1.f, -1.f, 1.f };
		}

		if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), mtCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		};

		Material* mappedMaterial = nullptr;
		if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
		{
			for (int i = 0; i < mtCount; ++i)
			{
				auto& mt = mappedMaterial[i];
				auto& mtf = file.GetMaterial(i);
				mt.diffuse = System::strong_cast<MathUtil::float4>(mtf.diffuse);
				mt.specular = System::strong_cast<MathUtil::float3>(mtf.specular);
				mt.specularity = mtf.specularity;
				mt.ambient = System::strong_cast<MathUtil::float3>(mtf.ambient);
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

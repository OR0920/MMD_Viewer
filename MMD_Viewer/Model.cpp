#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mHeap(),
	mVB(),
	mIB(),
	mMaterialBuffer(),
	isSuccessLoad(GUI::Result::FAIL)
{

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

		auto descriptorCount = 1 + mtCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform), 1) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
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

		auto descriptorCount = 1 + mtCount;
		if (mDevice.CreateDescriptorHeap(mHeap, descriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform), 1) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
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

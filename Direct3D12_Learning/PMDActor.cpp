// header
#include"PMDActor.h"
#include"PMDRenderer.h"
#include"Dx12Wrapper.h"

// std
#include<cassert>

// windows

// directx
#include"d3dx12.h"

// my lib
#include"System.h"
#include"MMDsdk.h"
#include"MathUtil.h"

#include"StringUtil.h"

#define CallInitFunctionWithAssert(func)\
if (FAILED(func))\
{\
	DebugMessageFunctionError(func, PMDActor::PMDActor());\
	assert(false);\
	return;\
}

static const char* const toonDirPath = "../x64/Debug/Test/Model/SharedToonTexture/";

PMDActor::PMDActor(const std::string argFilepath, PMDRenderer& argRenderer)
	:
	mRenderer(argRenderer),
	mDx12(argRenderer.mDx12),
	mAngle(0.f)
{
	mTransform.world = DirectX::XMMatrixIdentity();
	CallInitFunctionWithAssert(LoadPMDFile(argFilepath));
	CallInitFunctionWithAssert(CreateTransformView());
	CallInitFunctionWithAssert(CreateMaterialData());
	CallInitFunctionWithAssert(CreateMaterialAndTextureView());
}


inline MathUtil::float4 GetFloat4FromPMD(const MMDsdk::float4& mf)
{
	MathUtil::float4 ret;

	ret.x = mf.x;
	ret.y = mf.y;
	ret.z = mf.z;
	ret.w = mf.w;

	return ret;
}

inline MathUtil::float3 GetFloat3FromPMD(const MMDsdk::float3& mf)
{
	MathUtil::float3 ret;

	ret.x = mf.x;
	ret.y = mf.y;
	ret.z = mf.z;

	return ret;
}

inline MathUtil::float2 GetFloat2FromPMD(const MMDsdk::float2& mf)
{
	MathUtil::float2 ret;

	ret.x = mf.x;
	ret.y = mf.y;

	return ret;
}

struct Vertex
{
	MathUtil::float3 pos;
	MathUtil::float3 normal;
	MathUtil::float2 uv;
	unsigned short boneNo[2];
	unsigned char boneWeight;
	unsigned char edgeFlg;
	uint16_t pad = 0;

	void GetDataFromPMD_Vertex(const MMDsdk::PmdFile::Vertex& v)
	{
		pos = GetFloat3FromPMD(v.position);
		normal = GetFloat3FromPMD(v.normal);
		uv = GetFloat2FromPMD(v.uv);
		boneNo[0] = v.GetBoneID(0);
		boneNo[1] = v.GetBoneID(1);
		boneWeight = v.weight;
		edgeFlg = v.edgeFlag;
	}
};

void PMDActor::MaterialOnShader::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);

	diffuse = GetFloat4FromPMD(data.diffuse);
	DebugOutFloat4(diffuse);
	specularity = data.specularity;

	specular = GetFloat3FromPMD(data.specular);
	ambient = GetFloat3FromPMD(data.ambient);
}

void PMDActor::MaterialOnCPU::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);

	toonIndex = data.toonIndex;
	edgeFlag = data.edgeFlag;
}


void PMDActor::MaterialOnCPU::LoadTexturePath(const char* const dirPath, const char* const texFileName)
{
	if (newArray_SplitFileName(nullptr, nullptr, texFileName) == false)
	{
		SetTexturePath(dirPath, texFileName);
	}
	else
	{
		char* texNameBuff1 = nullptr;
		char* texNameBuff2 = nullptr;
		newArray_SplitFileName(&texNameBuff1, &texNameBuff2, texFileName);

		SetTexturePath(dirPath, texNameBuff1);
		SetTexturePath(dirPath, texNameBuff2);

		System::SafeDeleteArray(&texNameBuff1);
		System::SafeDeleteArray(&texNameBuff2);
	}

}

void PMDActor::MaterialOnCPU::SetTexturePath(const char* const dirPath, const char* const texname)
{
	char* ext = nullptr;

	newArray_GetExtention(&ext, texname);

	char* pathBuff = nullptr;
	System::newArray_CopyAssetPath(&pathBuff, dirPath, texname);


	if (System::StringEqual(ext, "sph"))
	{
		sphPath = pathBuff;
	}
	else if (System::StringEqual(ext, "spa"))
	{
		spaPath = pathBuff;
	}
	else
	{
		texPath = pathBuff;
	}

	System::SafeDeleteArray(&pathBuff);
	System::SafeDeleteArray(&ext);
}

void PMDActor::Material::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	onShader.GetMaterialDataFromPMD(materialDataFromFile);
	onCPU.GetMaterialDataFromPMD(materialDataFromFile);

	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);
	vertexCount = data.vertexCount;
}



class MMD_Model
{
public:
	MMD_Model() {}
	void Load(const char* const filepath)
	{
		MMDsdk::PmdFile pmd(filepath);
		if (pmd.IsSuccessLoad() == true)
		{
			LoadModel(pmd);
			DebugMessage("load as pmd");
			return;
		}
		MMDsdk::PmxFile pmx(filepath);
		if (pmx.IsSuccessLoad() == true)
		{
			LoadModel(pmx);
			DebugMessage("load as pmx");
			return;
		}
	}

	bool IsSuccessLoad() const
	{
		return isSuccess;
	}

	const int GetVertexCount() const
	{
		return mesh.size();
	}
	const Vertex& GetVertex(const int i) const
	{
		return mesh[i];
	}
	const int GetIndexCount() const
	{
		return index.size();
	}
	const std::vector<int>& GetIndex() const
	{
		return index;
	}

	const int GetMaterialCount() const
	{
		return mMaterials.size();
	}

	const PMDActor::Material& GetMaterial(const int i) const
	{
		return mMaterials[i];
	}
	const std::vector<PMDActor::Material>& GetMaterial() const
	{
		return mMaterials;
	}
private:
	void LoadModel(const MMDsdk::PmdFile& pmd)
	{
		mesh.assign(pmd.GetVertexCount(), {});
		for (int i = 0; i < pmd.GetVertexCount(); ++i)
		{
			mesh[i].GetDataFromPMD_Vertex(pmd.GetVertex(i));
		}

		index.assign(pmd.GetIndexCount(), 0);
		for (int i = 0; i < pmd.GetIndexCount(); ++i)
		{
			index[i] = pmd.GetIndex(i);
		}


		mMaterials.assign(pmd.GetMaterialCount(), {});
		for (int i = 0; i < pmd.GetMaterialCount(); ++i)
		{
			auto& fileM = pmd.GetMaterial(i);
			mMaterials[i].GetMaterialDataFromPMD(&fileM);
			if (fileM.texturePath.GetText()[0] != '\0')
			{
				mMaterials[i].onCPU.LoadTexturePath(pmd.GetDirectoryPath(), fileM.texturePath.GetText());
			}
		}

		isSuccess = true;
	}
	void LoadModel(const MMDsdk::PmxFile& pmx)
	{
		mesh.assign(pmx.GetVertexCount(), {});
		for (int i = 0; i < pmx.GetVertexCount(); ++i)
		{
			auto& v = mesh[i];
			auto& xv = pmx.GetVertex(i);
			v.pos = GetFloat3FromPMD(xv.position);
			v.normal = GetFloat3FromPMD(xv.normal);
			v.uv = GetFloat2FromPMD(xv.uv);
			v.boneNo[0] = xv.GetBoneID(0);
			v.boneNo[1] = xv.GetBoneID(1);
			v.boneWeight = xv.GetWeight(0);
			v.edgeFlg = xv.edgeRate;
		}

		index.assign(pmx.GetIndexCount(), 0);
		for (int i = 0; i < pmx.GetIndexCount(); ++i)
		{
			index[i] = pmx.GetIndex(i);
		}

		auto materialCount = pmx.GetMaterialCount();
		mMaterials.assign(materialCount, {});
		for (int i = 0; i < materialCount; ++i)
		{
			auto& xm = pmx.GetMaterial(i);
			auto& ms = mMaterials[i].onShader;
			ms.diffuse = GetFloat4FromPMD(xm.diffuse);
			ms.specular = GetFloat3FromPMD(xm.specular);
			ms.specularity = xm.specularity;
			ms.ambient = GetFloat3FromPMD(xm.ambient);
			auto& mc = mMaterials[i].onCPU;
			mc.edgeFlag = xm.edgeSize;

			if (xm.toonMode == MMDsdk::PmxFile::Material::TM_SHARED)
			{
				mc.toonIndex = xm.toonTextureID;
			}
			else if (xm.toonMode == MMDsdk::PmxFile::Material::TM_UNIQUE && xm.toonTextureID != -1)
			{
				char* toonPath = nullptr;
				System::newArray_CopyAssetPath(&toonPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.toonTextureID).GetText());
				mc.uniqueToonPath = toonPath;
				System::SafeDeleteArray(&toonPath);
			}
			else
			{
				mc.toonIndex = -1;
			}

			mMaterials[i].vertexCount = xm.vertexCount;
			char* texPath = nullptr;
			if (xm.textureID != -1)
			{
				System::newArray_CopyAssetPath(&texPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.textureID).GetText());
				mc.texPath = texPath;
				System::SafeDeleteArray(&texPath);
			}
			else if (xm.sphereTextureID != -1) 
			{
				System::newArray_CopyAssetPath(&texPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.sphereTextureID).GetText());
				switch (xm.sphereMode)
				{
				case MMDsdk::PmxFile::Material::SphereMode::SM_SPH:
					mc.sphPath = texPath;
				case MMDsdk::PmxFile::Material::SphereMode::SM_SPA:
					mc.spaPath = texPath;
				default:
					break;
				}
				System::SafeDeleteArray(&texPath);
			}
		}

		isSuccess = true;
	}

	bool isSuccess = false;

	std::vector<Vertex> mesh;
	std::vector<int> index;
	std::vector<PMDActor::Material> mMaterials;
};

HRESULT PMDActor::LoadPMDFile(const std::string argFilepath)
{
	model = new MMD_Model();
	model->Load(argFilepath.c_str());

	if (model->IsSuccessLoad() == false)
	{
		return S_FALSE;
	}


	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	DebugOutParam(model->GetVertexCount());
	DebugOutParam(sizeof(Vertex));
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(model->GetVertexCount() * sizeof(Vertex));

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mVertexBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(mVertexBuffer), PMDActor::LoadPMDFile());
		DebugOutParamHex(result);
		assert(SUCCEEDED(result));
		return result;
	}

	char* vertexBufferMap = nullptr;
	result = mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferMap));

	if (FAILED(result))
	{
		DebugMessageFunctionError(mVertexBuffer->Map(), PMDActor::LoadPMDFile);
		assert(SUCCEEDED(result));
		return result;
	}

	for (int i = 0; i < model->GetVertexCount(); ++i)
	{
		*reinterpret_cast<Vertex*>(vertexBufferMap) = model->GetVertex(i);
		vertexBufferMap += sizeof(Vertex);
	}
	mVertexBuffer->Unmap(0, nullptr);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = sizeof(Vertex) * model->GetVertexCount();
	mVertexBufferView.StrideInBytes = sizeof(Vertex);


	auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(model->GetIndexCount() * sizeof(model->GetIndex()[0]));

	result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mIndexBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(mIndexBuffer), PMDActor::LoadPMDFile());
		assert(SUCCEEDED(result));
		return result;
	}

	unsigned short* mappedIndex = nullptr;
	result = mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mIndexBuffer->Map(), PMDActor::LoadPMDFile());
		assert(SUCCEEDED(result));
		return result;
	}
	std::copy(model->GetIndex().begin(), model->GetIndex().end(), mappedIndex);
	mIndexBuffer->Unmap(0, nullptr);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	mIndexBufferView.SizeInBytes = model->GetIndex().size() * sizeof(model->GetIndex()[0]);


	std::vector<std::string> toonTexPath(10);
	std::vector<std::string> toonTexName =
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
		"toon10.bmp"
	};

	for (int i = 0; i < 10; ++i)
	{
		char* texPath = nullptr;
		System::newArray_CopyAssetPath(&texPath, toonDirPath, toonTexName[i].c_str());

		toonTexPath[i] = texPath;
		System::SafeDeleteArray(&texPath);
	}

	mTextureResources.assign(model->GetMaterialCount(), nullptr);
	mSPH_TextureResources.assign(model->GetMaterialCount(), nullptr);
	mSPA_TextureResources.assign(model->GetMaterialCount(), nullptr);
	mToonTextureResources.assign(model->GetMaterialCount(), nullptr);

	for (int i = 0; i < model->GetMaterialCount(); ++i)
	{
		DebugOutParamI(i);
		auto& m = model->GetMaterial(i).onCPU;
		if (0 <= m.toonIndex && m.toonIndex < 10)
		{
			mToonTextureResources[i] = mDx12.GetTextureByPath(toonTexPath[m.toonIndex]);
		}
		else if (m.uniqueToonPath.size() != 0)
		{
			mToonTextureResources[i] = mDx12.GetTextureByPath(m.uniqueToonPath);
		}

		if (m.texPath.size() != 0)
		{
			mTextureResources[i] = mDx12.GetTextureByPath(m.texPath);
		}

		if (m.sphPath.size() != 0)
		{
			mSPH_TextureResources[i] = mDx12.GetTextureByPath(m.sphPath);
		}

		if (m.spaPath.size() != 0)
		{
			mSPA_TextureResources[i] = mDx12.GetTextureByPath(m.spaPath);
		}
	}
}

HRESULT PMDActor::CreateTransformView()
{
	auto buffSize = sizeof(Transform);
	buffSize = (buffSize + 0xff) & ~0xff;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);


	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mTransformBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateCommittedResource(mTransformBuffer),
			PMDActor::CreateTransformView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	result = mTransformBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedTransform));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mTransformBuffer->Map(), PMDActor::CreateTransformView());
		assert(SUCCEEDED(result));
		return result;
	}

	*mMappedTransform = mTransform;

	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = mDx12.GetDevice()->CreateDescriptorHeap
	(
		&transformDescHeapDesc,
		IID_PPV_ARGS(mTransformHeap.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateDescriptorHeap(mTransformHeap),
			PMDActor::CreateTransformView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mTransformBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	mDx12.GetDevice()->CreateConstantBufferView(&cbvDesc, mTransformHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT PMDActor::CreateMaterialData()
{
	auto& mMaterials = model->GetMaterial();
	auto materialBuffSize = sizeof(MaterialOnShader);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	DebugOutParamI(materialBuffSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * mMaterials.size());

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mMaterialBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(), PMDActor::CreateMaterialData());
		assert(SUCCEEDED(result));
		return result;
	}

	unsigned char* mapMaterial = nullptr;
	result = mMaterialBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterial));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mMaterialBuffer->Map(), PMDActor::CreateMaterialData());
		assert(SUCCEEDED(result));
		return result;
	}

	for (int i = 0; i < mMaterials.size(); ++i)
	{
		//auto& m = mMaterials[i].onShader;
		//auto& mm = *reinterpret_cast<MaterialOnShader*>(mapMaterial);
		//mm.diffuse = m.diffuse;
		//mm.specular = m.specular;
		//mm.specularity = m.specularity;
		//mm.ambient = m.ambient;
		*reinterpret_cast<MaterialOnShader*>(mapMaterial) = mMaterials[i].onShader;
		mapMaterial += materialBuffSize;
	}

	mMaterialBuffer->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDActor::CreateMaterialAndTextureView()
{
	auto& mMaterials = model->GetMaterial();
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = mMaterials.size() * 5;
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;
	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	auto result = mDx12.GetDevice()->CreateDescriptorHeap
	(
		&materialDescHeapDesc,
		IID_PPV_ARGS(mMaterialHeap.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateDescriptorHeap(mMaterialHeap),
			PMDActor::CreateMaterialAndTextureView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	auto materialBuffSize = sizeof(MaterialOnShader);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBV_Desc = {};
	matCBV_Desc.BufferLocation = mMaterialBuffer->GetGPUVirtualAddress();
	matCBV_Desc.SizeInBytes = materialBuffSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapHandle(mMaterialHeap->GetCPUDescriptorHandleForHeapStart());

	auto incSize = mDx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < mMaterials.size(); ++i)
	{
		mDx12.GetDevice()->CreateConstantBufferView(&matCBV_Desc, matDescHeapHandle);
		matDescHeapHandle.ptr += incSize;
		matCBV_Desc.BufferLocation += materialBuffSize;

		if (mTextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureWhite->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureWhite.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mTextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mTextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);

		if (mSPH_TextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureWhite->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureWhite.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mSPH_TextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mSPH_TextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);


		if (mSPA_TextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureBlack->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureBlack.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mSPA_TextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mSPA_TextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);

		
		

		if (mToonTextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureWhite->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureWhite.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mToonTextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mToonTextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);
	}
}


void PMDActor::Update()
{
	mAngle += 0.03f;
	mMappedTransform->world = MathUtil::Matrix::GenerateMatrixRotationY(mAngle);
}

void PMDActor::Draw()
{
	const auto& cmdl = mDx12.GetCommandList();

	cmdl->IASetVertexBuffers(0, 1, &mVertexBufferView);
	cmdl->IASetIndexBuffer(&mIndexBufferView);

	ID3D12DescriptorHeap* transHeaps[] = { mTransformHeap.Get() };
	cmdl->SetDescriptorHeaps(1, transHeaps);
	cmdl->SetGraphicsRootDescriptorTable(1, mTransformHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* materialHeaps[] = { mMaterialHeap.Get() };
	cmdl->SetDescriptorHeaps(1, materialHeaps);

	auto materialHandle = mMaterialHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int indexOffs = 0;
	auto cbv_srv_incSize = mDx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

	for (auto& m : model->GetMaterial())
	{
		cmdl->SetGraphicsRootDescriptorTable(2, materialHandle);
		cmdl->DrawIndexedInstanced(m.vertexCount, 1, indexOffs, 0, 0);
		materialHandle.ptr += cbv_srv_incSize;
		indexOffs += m.vertexCount;
	}
}

PMDActor::~PMDActor()
{
	System::SafeDelete(&model);
}


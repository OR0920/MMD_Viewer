#pragma once

#include<string>
#include<vector>

#include<D3D12.h>


#include"MathUtil.h"
#include"UsingComPtr.h"


class Dx12Wrapper;
class PMDRenderer;
class MMD_Model;

class PMDActor
{
	friend PMDRenderer;
public:
	PMDActor(const std::string argPath , PMDRenderer& argRenderer);
	~PMDActor();

	//PMDActor* Clone();
	void Update();
	void Draw();


	struct MaterialOnShader
	{
		MathUtil::float4 diffuse;
		MathUtil::float3 specular;
		float specularity;
		MathUtil::float3 ambient;

		void GetMaterialDataFromPMD(const void* materialDataFromFile);
	};
	struct MaterialOnCPU
	{
		std::string texPath;
		std::string sphPath;
		std::string spaPath;
		std::string uniqueToonPath;

		int toonIndex;
		bool edgeFlag;

		void GetMaterialDataFromPMD(const void* materialDataFromFile);
		void LoadTexturePath(const char* const dirPath, const char* const texFileName);
	private:
		void SetTexturePath(const char* const dirPath, const char* const texname);
	};
	struct Material
	{
		unsigned int vertexCount;
		MaterialOnShader onShader;
		MaterialOnCPU onCPU;

		void GetMaterialDataFromPMD(const void* materialDataFromFile);
	};

private:
	MMD_Model* model;

	PMDRenderer& mRenderer;
	Dx12Wrapper& mDx12;

	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	ComPtr<ID3D12Resource> mTransformMat;
	ComPtr<ID3D12DescriptorHeap> mTransformHeap;


	struct Transform
	{
		MathUtil::Matrix world;
	} mTransform;
	MathUtil::Matrix *mMappedMatrices;

	ComPtr<ID3D12Resource> mTransformBuffer;

	//std::vector<Material> mMaterials;
	ComPtr<ID3D12Resource> mMaterialBuffer;
	std::vector<ComPtr<ID3D12Resource>> mTextureResources;
	std::vector<ComPtr<ID3D12Resource>> mSPH_TextureResources;
	std::vector<ComPtr<ID3D12Resource>> mSPA_TextureResources;
	std::vector<ComPtr<ID3D12Resource>> mToonTextureResources;

	HRESULT CreateMaterialData();
	ComPtr<ID3D12DescriptorHeap> mMaterialHeap;

	HRESULT CreateMaterialAndTextureView();

	HRESULT CreateTransformView();
	HRESULT LoadPMDFile(const std::string argPath);

	float mAngle;
};


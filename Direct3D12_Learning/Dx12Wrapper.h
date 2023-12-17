#pragma once

// std
#include<memory>
#include<functional>
#include<string>
#include<map>
#include<unordered_map>
#include<vector>

// windows
#include<Windows.h>
#include<WRL.h>

// DirextX
#include<DXGI1_6.h>
#include<D3D12.h>
//#include<DirectXMath.h>
#include<DirectXTex.h>

#include"UsingComPtr.h"

#include"MathUtil.h"

class Dx12Wrapper
{
public:
	Dx12Wrapper(HWND argWindowHandle);
	~Dx12Wrapper();

	void Update();
	void BeginDraw();
	void EndDraw();

	ComPtr<ID3D12Resource> GetTextureByPath(const std::string argTexpath);
	ComPtr<ID3D12Device> GetDevice();
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	ComPtr<IDXGISwapChain4> GetSwapChain();

	void MoveCamera(const MathUtil::Vector& velocity);
	void ResetCamera();

	void SetScene();

private:
	SIZE mWindowSize;

	ComPtr<IDXGIFactory4> mDxgiFactory;
	ComPtr<IDXGISwapChain4> mSwapChain;

	// デバイス毎
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12CommandQueue> mCommandQueue;

	// 描画先毎
	std::vector<ComPtr<ID3D12Resource>> mBackBuffer; // 可変長にしておこう
	ComPtr<ID3D12Resource> mDepthBuffer;
	ComPtr<ID3D12DescriptorHeap> mRTV_Heaps;
	ComPtr<ID3D12DescriptorHeap> mDSV_Heaps;
	std::unique_ptr<D3D12_VIEWPORT> mViewPort;
	std::unique_ptr<D3D12_RECT> mScissorRect;

	// シーン毎
	ComPtr<ID3D12Resource> mSceneConstantBuffer;
	struct SceneData
	{
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
		MathUtil::float3 eye;
	}*mMappedSceneData;
	ComPtr<ID3D12DescriptorHeap> mSceneDescriptorHeap;

	const MathUtil::float3 c_eye = { 0, 10, -40 };
	const MathUtil::float3 c_eyeDir = {0.f, 0.f, 1.f};
	const MathUtil::float3 c_target = { 0, 10, 0 };
	const MathUtil::float3 c_up = { 0, 1, 0 };

	MathUtil::Vector eye = c_eye;
	MathUtil::Vector eyeDir = c_eyeDir;
	MathUtil::Vector target = c_target;
	MathUtil::Vector up = c_up;

	ComPtr<ID3D12Fence> mFence;
	UINT64 mFenceValue;

	HRESULT CreateFinalRenderTarget();
	HRESULT CreateDepthStensilView();
	HRESULT CreateSwapChain(const HWND& argWindowHandle);


	HRESULT InitializeDevices();

	HRESULT InitializeCommandObjects();
	HRESULT CreateSceneView();


	using LoadLambda_t =
		std::function
		<HRESULT(
			const std::wstring& argPath,
			DirectX::TexMetadata* argMetaData,
			DirectX::ScratchImage& argScratchImage)>;

	std::map <std::string, LoadLambda_t> mLoadLambdaTable;
	std::unordered_map<std::string, ComPtr<ID3D12Resource>> mTextureTable;
	void CreateTextureLoaderTable();
	ComPtr<ID3D12Resource> CreateTextureFromFile(const std::string argTexPath);
};



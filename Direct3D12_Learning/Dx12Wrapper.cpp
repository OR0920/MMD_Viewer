#include"Dx12Wrapper.h"
#include"Application.h"
#include"System.h"

#include<cassert>

#include"d3dx12.h"

#include"StringUtil.h"
#include"MathUtil.h"


#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#define CallInitFunctionWithAssert(func)\
if (FAILED(func))\
{\
	DebugMessageFunctionError(func, Dx12Wrapper::Dx12Wrapper);\
	assert(false);\
	return;\
}

void EnableDebugLayer()
{
	ComPtr<ID3D12Debug> debugLayer = nullptr;
	auto result(D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf())));
	if (FAILED(result))
	{
		DebugMessageFunctionError(D3D12GetDebugInterface, EnableDebugLayer);
		return;
	}
	debugLayer->EnableDebugLayer();
}

Dx12Wrapper::Dx12Wrapper(HWND argWindowHandle)
	:
	mWindowSize({}),
	mDxgiFactory(nullptr),
	mSwapChain(nullptr),
	mDevice(nullptr),
	mCommandAllocator(nullptr),
	mCommandList(nullptr),
	mCommandQueue(nullptr),
	mDepthBuffer(nullptr),
	mBackBuffer(),
	mRTV_Heaps(nullptr),
	mDSV_Heaps(nullptr),
	mViewPort(nullptr),
	mScissorRect(nullptr),
	mSceneConstantBuffer(nullptr),
	mMappedSceneData(nullptr),
	mSceneDescriptorHeap(nullptr),
	mFence(nullptr),
	mFenceValue(0),
	mTextureTable()
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif // _DEBUG
	auto& app = Application::Instance();
	mWindowSize = app.GetWindowSize();

	CallInitFunctionWithAssert(InitializeDevices());
	CallInitFunctionWithAssert(InitializeCommandObjects());
	CallInitFunctionWithAssert(CreateSwapChain(argWindowHandle));
	CallInitFunctionWithAssert(CreateFinalRenderTarget());
	CallInitFunctionWithAssert(CreateSceneView());

	CreateTextureLoaderTable();

	CallInitFunctionWithAssert(CreateDepthStensilView());
	CallInitFunctionWithAssert
	(
		mDevice->CreateFence
		(
			mFenceValue,
			D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(mFence.ReleaseAndGetAddressOf()
			)
		)
	);
}

HRESULT Dx12Wrapper::InitializeDevices()
{
	UINT flagsDXGI = 0;
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;

	ComPtr<IDXGIFactory> tFactory = nullptr;

	auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(tFactory.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		DebugMessageFunctionError(CreateDXGIFactory2, Dx12Wrapper::InitializeDevices)
			return result;
	}

	result = tFactory->QueryInterface(IID_PPV_ARGS(mDxgiFactory.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		DebugMessageFunctionError(tFactory->QueryInterface, Dx12Wrapper::InitializeDevices);
		return result;
	}

	// アダプターを列挙しNVIDIAのものを見つける
	std::vector<ComPtr<IDXGIAdapter>> adapters;
	ComPtr<IDXGIAdapter> tmpAdapter = nullptr;
	bool forContinueFlag = true;
	for (int i = 0; forContinueFlag; ++i)
	{
		forContinueFlag =
			mDxgiFactory->EnumAdapters(i, tmpAdapter.ReleaseAndGetAddressOf())
			!= DXGI_ERROR_NOT_FOUND;
		adapters.push_back(tmpAdapter);
	}
	for (auto adapter : adapters)
	{
		DXGI_ADAPTER_DESC adapterDesc = {};
		adapter->GetDesc(&adapterDesc);
		std::wstring strDesc = adapterDesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adapter;
			break;
		}
	}
	result = S_FALSE;


	// デバイスの初期化
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL featureLevel = {};

	for (auto level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter.Get(), level, IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf()))))
		{
			featureLevel = level;
			result = S_OK;
			break;
		}
	}
	if (FAILED(result))
	{
		DebugMessageFunctionError(D3D12CreateDevice, Dx12Wrapper::InitializeDevices);
		return result;
	}
	return result;
}

HRESULT Dx12Wrapper::InitializeCommandObjects()
{
	auto result =
		mDevice->CreateCommandAllocator
		(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(mCommandAllocator.ReleaseAndGetAddressOf())
		);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommandAllocator, Dx12Wrapper::InitializeCommandObjects);
		assert(false);
		return result;
	}
	result =
		mDevice->CreateCommandList
		(
			0,
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			mCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(mCommandList.ReleaseAndGetAddressOf())
		);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommandList, InitializeCommandObjects);
		assert(false);
		return result;
	}

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = mDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(mCommandQueue.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommandQueue, InitializeCommandObjects);
		assert(false);
		return result;
	}
	return result;
}


HRESULT Dx12Wrapper::CreateSwapChain(const HWND& argWindowHandle)
{
	RECT rect = {};
	bool boolResult = GetWindowRect(argWindowHandle, &rect);
	if (boolResult == 0)
	{
		DebugMessageFunctionError(GetWindowRect, Dx12Wrapper::CreateSwapChain);
		return S_FALSE;
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = mWindowSize.cx;
	swapChainDesc.Height = mWindowSize.cy;
	swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = mDxgiFactory->CreateSwapChainForHwnd
	(
		mCommandQueue.Get(),
		argWindowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(mSwapChain.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDxgiFactory->CreateSwapChainForHwnd(), Dx12Wrapper::CreateSwapChain());
		DebugOutParamHex(result);
		assert(SUCCEEDED(result));
		return result;
	}
	//result = swapchain->QueryInterface(IID_PPV_ARGS(mSwapChain.ReleaseAndGetAddressOf()));
	//if (FAILED(result))
	//{
	//	DebugMessageFunctionError(swapchain->QueryInterface(), Dx12Wrapper::CreateSwapChain());
	//	assert(SUCCEEDED(result));
	//	return result;
	//}
	return result;
}

HRESULT Dx12Wrapper::CreateFinalRenderTarget()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto result = mSwapChain->GetDesc1(&desc);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mSwapChain->GetDesc1, Dx12Wrapper::CreateFinalRenderTarget);
		return result;
	}

	// レンダーターゲットビューのヒープを作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = mDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mRTV_Heaps.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateDescriptorHeap(), Dx12Wrapper::CreateFinalRenderTarget());
		assert(SUCCEEDED(result));
		return result;
	}

	// スワップチェインの情報を取得し、バックバッファを初期化
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = mSwapChain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mSwapChain->GetDesc(), Dx12Wrapper::CreateFinalRenderTarget());
		assert(SUCCEEDED(result));
		return result;
	}
	mBackBuffer.assign(swcDesc.BufferCount, nullptr);

	// レンダーターゲットビューを作成する場所を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < swcDesc.BufferCount; ++i)
	{
		// スワップチェインからレンダーターゲットのリソースを取得
		result = mSwapChain->GetBuffer(i, IID_PPV_ARGS(mBackBuffer[i].ReleaseAndGetAddressOf()));

		if (FAILED(result))
		{
			DebugMessageFunctionError(mSwapChain->GetBuffer(), Dx12Wrapper::CreateFinalRenderTarget());
		}
		assert(SUCCEEDED(result));

		// リソースの情報からフォーマット取得
		rtvDesc.Format = mBackBuffer[i]->GetDesc().Format;

		// 取得したリソースのビューを作成する
		mDevice->CreateRenderTargetView(mBackBuffer[i].Get(), &rtvDesc, rtvHandle);

		// 次のバッファへ
		rtvHandle.ptr +=
			mDevice->GetDescriptorHandleIncrementSize
			(
				D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV
			);
	}

	mViewPort.reset(new CD3DX12_VIEWPORT(mBackBuffer[0].Get()));
	mScissorRect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));

	return result;
}

HRESULT Dx12Wrapper::CreateSceneView()
{
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	auto result = mSwapChain->GetDesc1(&desc);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mSwapChain->GetDesc1(), Dx12Wrapper::CreateSceneView());
		return result;
	}

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);

	result = mDevice->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mSceneConstantBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommittedResource(), Dx12Wrapper::CreateSceneView());
		assert(SUCCEEDED(result));
		return result;
	}

	result = mSceneConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedSceneData));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mSceneConstantBuffer->Map(), Dx12Wrapper::CreateSceneView);
		assert(SUCCEEDED(result));
		return result;
	}

	mMappedSceneData->view = MathUtil::Matrix::GenerateMatrixLookAtLH(eye, target, up);

	mMappedSceneData->proj = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
	(
		DirectX::XM_PIDIV4,
		static_cast<float>(desc.Width) / static_cast<float>(desc.Height),
		0.1f,
		1000.f
	);

	mMappedSceneData->eye = eye.GetFloat3();

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = mDevice->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(mSceneDescriptorHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateDescriptorHeap(), Dx12Wrapper::CreateSceneView());
		return result;
	}

	auto heapHandle = mSceneDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mSceneConstantBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = mSceneConstantBuffer->GetDesc().Width;

	mDevice->CreateConstantBufferView(&cbvDesc, heapHandle);

	return result;
}

void Dx12Wrapper::MoveCamera(MathUtil::Vector& velocity)
{
	eye = eye + velocity;
	target = target + velocity;

	mMappedSceneData->view = MathUtil::Matrix::GenerateMatrixLookAtLH(eye, target, up);
}

void Dx12Wrapper::ResetCamera()
{
	eye = c_eye;
	target = c_target;
	up = c_up;

	mMappedSceneData->view = MathUtil::Matrix::GenerateMatrixLookAtLH(eye, target, up);
}

void Dx12Wrapper::CreateTextureLoaderTable()
{
	mLoadLambdaTable["sph"]
		= mLoadLambdaTable["spa"]
		= mLoadLambdaTable["bmp"]
		= mLoadLambdaTable["png"]
		= mLoadLambdaTable["jpg"]
		= [](
			const std::wstring& argPath,
			DirectX::TexMetadata* argMetadata,
			DirectX::ScratchImage& argImg
			)->HRESULT
		{
			return
				DirectX::LoadFromWICFile
				(
					argPath.c_str(),
					DirectX::WIC_FLAGS::WIC_FLAGS_NONE,
					argMetadata,
					argImg
				);
		};

	mLoadLambdaTable["tga"]
		= [](
			const std::wstring& argPath,
			DirectX::TexMetadata* argMetadata,
			DirectX::ScratchImage& argImg
			)->HRESULT
		{
			return
				DirectX::LoadFromTGAFile
				(
					argPath.c_str(),
					argMetadata,
					argImg
				);
		};

	mLoadLambdaTable["dds"]
		= [](
			const std::wstring& argPath,
			DirectX::TexMetadata* argMetadata,
			DirectX::ScratchImage& argImg
			)->HRESULT
		{
			return
				DirectX::LoadFromDDSFile
				(
					argPath.c_str(),
					DirectX::DDS_FLAGS::DDS_FLAGS_NONE,
					argMetadata,
					argImg
				);
		};
}

HRESULT Dx12Wrapper::CreateDepthStensilView()
{
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	auto result = mSwapChain->GetDesc1(&scDesc);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mSwapChain->GetDesc1(), Dx12Wrapper::CreateDepthStensilView());
		return result;
	}

	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D
	(
		DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
		scDesc.Width,
		scDesc.Height
	);
	resDesc.MipLevels = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT, 1.f, 0);

	result = mDevice->CreateCommittedResource
	(
		&depthHeapProp,
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(mDepthBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommittedResource(), Dx12Wrapper::CreateDepthStensilView());
		return result;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDSV_Heaps.ReleaseAndGetAddressOf()));

	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateDescriptorHeap, Dx12Wrapper::CreateDepthStensilView());
		return result;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;

	mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvDesc, mDSV_Heaps->GetCPUDescriptorHandleForHeapStart());
	return result;
}


ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const std::string texPath)
{
	auto it = mTextureTable.find(texPath);
	if (it != mTextureTable.end())
	{
		return mTextureTable[texPath];
	}
	else
	{
		mTextureTable[texPath] = CreateTextureFromFile(texPath);
		return mTextureTable[texPath];
	}
}

ComPtr<ID3D12Resource> Dx12Wrapper::CreateTextureFromFile(const std::string texpath)
{
	DebugOutString(texpath.c_str());
	DirectX::TexMetadata metadata = {};
	DirectX::ScratchImage scratchImage = {};

	wchar_t* wTexPath = nullptr;
	System::newArray_CreateWideCharStrFromMultiByteStr(&wTexPath, texpath.c_str());

	char* ext = nullptr;
	newArray_GetExtention(&ext, texpath.c_str());

	auto result = mLoadLambdaTable[ext](wTexPath, &metadata, scratchImage);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mLoadLambdaTable[ext](), Dx12Wrapper::CreateTextureFromFile());
		return nullptr;
	}
	auto img = scratchImage.GetImage(0, 0, 0);

	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

	ComPtr<ID3D12Resource> texBuff = nullptr;
	result = mDevice->CreateCommittedResource
	(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(texBuff.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError(mDevice->CreateCommittedResource(texBuff), Dx12Wrapper::CreateTextureFromFile());
		return nullptr;
	}

	result = texBuff->WriteToSubresource
	(
		0,
		nullptr,
		img->pixels,
		img->rowPitch,
		img->slicePitch
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(texBuff->WriteToSubresource(), Dx12Wrapper::CreateTextureFromFile());
		return nullptr;
	}

	System::SafeDeleteArray(&wTexPath);
	System::SafeDeleteArray(&ext);

	return texBuff;
}

void Dx12Wrapper::BeginDraw()
{
	auto backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		mBackBuffer[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier(1, &barrier);

	auto rtvHandle = mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * backBufferIndex;

	auto dsvHandle = mDSV_Heaps->GetCPUDescriptorHandleForHeapStart();
	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

	float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.f };
	mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	mCommandList->RSSetViewports(1, mViewPort.get());
	mCommandList->RSSetScissorRects(1, mScissorRect.get());
}

void Dx12Wrapper::SetScene()
{
	ID3D12DescriptorHeap* sceneheaps[] = { mSceneDescriptorHeap.Get() };
	mCommandList->SetDescriptorHeaps(1, sceneheaps);
	mCommandList->SetGraphicsRootDescriptorTable(0, mSceneDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void Dx12Wrapper::EndDraw()
{
	auto backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		mBackBuffer[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->Close();

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(1, cmdLists);
	mCommandQueue->Signal(mFence.Get(), ++mFenceValue);

	if (mFence->GetCompletedValue() < mFenceValue)
	{
		auto ev = CreateEvent(nullptr, false, false, nullptr);
		mFence->SetEventOnCompletion(mFenceValue, ev);
		WaitForSingleObject(ev, INFINITE);
		CloseHandle(ev);
	}

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);
}

ComPtr<IDXGISwapChain4> Dx12Wrapper::GetSwapChain()
{
	return mSwapChain;
}

ComPtr<ID3D12Device> Dx12Wrapper::GetDevice()
{
	return mDevice;
}


void Dx12Wrapper::Update()
{

}

ComPtr<ID3D12GraphicsCommandList> Dx12Wrapper::GetCommandList()
{
	return mCommandList;
}


Dx12Wrapper::~Dx12Wrapper()
{
}



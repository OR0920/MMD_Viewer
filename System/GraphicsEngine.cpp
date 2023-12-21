// 実装が長くなりそうなので分離
#include"GUI_Util.h"


// windows
#include"d3dx12.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


// my lib
#include"System.h"

using namespace System;
using namespace System::GUI;


GraphicsEngine::GraphicsEngine() {}
GraphicsEngine::~GraphicsEngine() {}


#define ReturnIfFailed(func, at)\
{\
	auto hresult = func;\
	if (FAILED(hresult))\
	{\
		DebugMessageFunctionError(func, at); \
		DebugOutParamHex(hresult);\
		return FAIL; \
	}\
}

Result GraphicsEngine::Init(const ParentWindow& parent)
{
	// Direct3D12 必携　より借用 
	//DX12デバイス作成
	{
		ReturnIfFailed
		(
			D3D12CreateDevice
			(
				0,
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);
	}

	//コマンドキュー作成
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ReturnIfFailed
		(
			mDevice->CreateCommandQueue
			(
				&queueDesc,
				IID_PPV_ARGS(mCommandQueue.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);
	}

	//コマンドリスト作成
	{
		ReturnIfFailed
		(
			mDevice->CreateCommandAllocator
			(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS
				(
					mCommandAllocator.ReleaseAndGetAddressOf()
				)
			),
			GraphicsEngine::Init()
		);
		ReturnIfFailed
		(
			mDevice->CreateCommandList
			(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				mCommandAllocator.Get(),
				NULL,
				IID_PPV_ARGS(mCommandList.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);
		mCommandList->Close();
	}

	//スワップチェーン作成
	{
		ComPtr<IDXGIFactory4> factory;
		ReturnIfFailed
		(
			CreateDXGIFactory1
			(
				IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = gFrameCount;
		swapChainDesc.Width = parent.GetWindowWidth();
		swapChainDesc.Height = parent.GetWindowHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapchain1 = nullptr;
		ReturnIfFailed
		(
			factory->CreateSwapChainForHwnd
			(
				mCommandQueue.Get(),
				parent.GetHandle(),
				&swapChainDesc,
				NULL, NULL,
				swapchain1.ReleaseAndGetAddressOf()
			),
			GraphicsEngine::Init()
		);

		ReturnIfFailed
		(
			swapchain1->QueryInterface<IDXGISwapChain3>
			(
				m_swapChain.ReleaseAndGetAddressOf()
			),
			GraphicsEngine::Init()
		);
	}

	//レンダーターゲット作成
	{
		//レンダーターゲットビューのヒープ作成
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = gFrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ReturnIfFailed
		(
			mDevice->CreateDescriptorHeap
			(
				&rtvHeapDesc,
				IID_PPV_ARGS(mRTV_Heap.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);

		//レンダーターゲットビュー作成
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTV_Heap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < gFrameCount; n++)
		{
			ReturnIfFailed(
				m_swapChain->GetBuffer
				(
					n,
					IID_PPV_ARGS
					(
						mRenderTargets[n].ReleaseAndGetAddressOf()
					)
				),
				GraphicsEngine::Init()
			);

			mDevice->CreateRenderTargetView
			(
				mRenderTargets[n].Get(), NULL, rtvHandle
			);

			rtvHandle.Offset
			(
				1,
				mDevice->GetDescriptorHandleIncrementSize
				(
					D3D12_DESCRIPTOR_HEAP_TYPE_RTV
				)
			);
		}
	}
	//深度バッファ
	{
		//深度バッファ自体の作成
		D3D12_HEAP_PROPERTIES dsHeapProp;
		ZeroMemory(&dsHeapProp, sizeof(&dsHeapProp));
		dsHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsHeapProp.CreationNodeMask = NULL;
		dsHeapProp.VisibleNodeMask = NULL;

		D3D12_RESOURCE_DESC dsResDesc;
		ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
		dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsResDesc.Alignment = 0;
		dsResDesc.Width = parent.GetWindowWidth();
		dsResDesc.Height = parent.GetWindowHeight();
		dsResDesc.DepthOrArraySize = 1;
		dsResDesc.MipLevels = 1;
		dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsResDesc.SampleDesc.Count = 1;
		dsResDesc.SampleDesc.Quality = 0;
		dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue;
		ZeroMemory(&clearValue, sizeof(D3D12_CLEAR_VALUE));
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		ReturnIfFailed
		(
			mDevice->CreateCommittedResource
			(
				&dsHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&dsResDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(mDepthBuffer.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);

		//深度バッファのビュー用のヒープ
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		ZeroMemory(&dsvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = NULL;

		ReturnIfFailed
		(
			mDevice->CreateDescriptorHeap
			(
				&dsvHeapDesc,
				IID_PPV_ARGS(mDSV_Heap.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);

		//深度バッファのビュー
		D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesk;
		ZeroMemory(&dsViewDesk, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
		dsViewDesk.Format = DXGI_FORMAT_D32_FLOAT;
		dsViewDesk.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsViewDesk.Flags = D3D12_DSV_FLAG_NONE;
		dsViewDesk.Texture2D.MipSlice = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDSV_Heap->GetCPUDescriptorHandleForHeapStart();

		mDevice->CreateDepthStencilView
		(
			mDepthBuffer.Get(),
			&dsViewDesk,
			dsvHandle
		);
	}

	//フェンス作成
	{
		ReturnIfFailed
		(
			mDevice->CreateFence
			(
				0,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(mFence.ReleaseAndGetAddressOf())
			),
			GraphicsEngine::Init()
		);
		mFenceValue = 1;

	}
	// 借用ここまで

	return Result::SUCCESS;
}

void GraphicsEngine::Draw
(
	const float clearR,
	const float clearG,
	const float clearB,
	const float clearA
)
{

}

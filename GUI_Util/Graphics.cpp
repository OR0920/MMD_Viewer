#include"GUI_Util.h"

// std

// windows
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#include"d3dx12.h"
#include<d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#define D3D12Allignment(buffSize) (buffSize + 0xff) & ~0xff;

// my lib
#include "System.h"

#define ReturnIfFailed(func, at)\
{\
	auto result = func;\
	if(FAILED(result))\
	{\
		DebugMessageFunctionError(func, at);\
		DebugOutParamHex(result);\
		return GUI::Result::FAIL;\
	}\
}

using namespace GUI;
using namespace GUI::Graphics;

template <class ComInterface>
void SafeRelease(ComInterface** ptr)
{
	if (*ptr != nullptr)
	{
		(*ptr)->Release();
		*ptr = nullptr;
	}
}


// デバッグレイヤ

Result Graphics::EnalbleDebugLayer()
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;
	ReturnIfFailed
	(
		D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf())),
		Graphics::EnalbleDebugLayer()
	);

	debug->EnableDebugLayer();

#endif // _DEBUG
	return SUCCESS;
}

// デバイス
Device::Device()
	:
	mDevice(nullptr)
{

}

Device::~Device()
{

}

Result Device::Create()
{
	ReturnIfFailed
	(
		D3D12CreateDevice
		(
			nullptr,
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())
		),
		Device::Create()
	);

	return SUCCESS;
}

Result Device::CreateGraphicsCommand(GraphicsCommand& command)
{
	auto type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Type = type;

	ReturnIfFailed
	(
		mDevice->CreateCommandQueue
		(
			&commandQueueDesc,
			IID_PPV_ARGS(command.mCommandQueue.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	ReturnIfFailed
	(
		mDevice->CreateCommandAllocator
		(
			type,
			IID_PPV_ARGS(command.mCommandAllocator.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	ReturnIfFailed
	(
		mDevice->CreateCommandList
		(
			0,
			type,
			command.mCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(command.mCommandList.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	return SUCCESS;
}

Result Device::CreateRenderTarget(RenderTarget& renderTarget, const SwapChain& swapchain)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	ReturnIfFailed
	(
		swapchain.GetDesc(&desc),
		Device::CreateRenderTarget()
	);

	auto& bufferCount = renderTarget.mBufferCount = desc.BufferCount;
	
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = bufferCount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ReturnIfFailed
	(
		mDevice->CreateDescriptorHeap
		(
			&heapDesc,
			IID_PPV_ARGS(renderTarget.mRTV_Heaps.ReleaseAndGetAddressOf())
		),
		Device::CreateRenderTarget()
	);

	renderTarget.mRT_Resource = new ComPtr<ID3D12Resource>[bufferCount] {nullptr};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle 
		= renderTarget.mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < renderTarget.mBufferCount; ++i)
	{
		auto result = swapchain.GetBuffer
		(
			i, 
			reinterpret_cast<void**>
			(renderTarget.mRT_Resource[i].ReleaseAndGetAddressOf())
		);

		if (result == Result::FAIL)
		{
			return FAIL;
		}

		rtvDesc.Format = renderTarget.mRT_Resource[i]->GetDesc().Format;
		mDevice->CreateRenderTargetView(renderTarget.mRT_Resource[i].Get(), &rtvDesc, rtvHandle);
		rtvHandle.ptr +=
			mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	renderTarget.mViewPort = CD3DX12_VIEWPORT(renderTarget.mRT_Resource[0].Get());
	renderTarget.mScissorRect
		= CD3DX12_RECT(0, 0, desc.BufferDesc.Width, desc.BufferDesc.Width);

	return SUCCESS;
}


// コマンド
GraphicsCommand::GraphicsCommand()
	:
	mCommandQueue(nullptr),
	mCommandAllocator(nullptr),
	mCommandList()
{

}

GraphicsCommand::~GraphicsCommand()
{

}

// スワップチェイン

SwapChain::SwapChain()
	:
	mSwapChain(nullptr)
{

}

SwapChain::~SwapChain()
{

}

Result SwapChain::Create
(
	const GraphicsCommand& device,
	const ParentWindow& targetWindow,
	const int frameCount
)
{
	if (targetWindow.GetHandle() == 0)
	{
		DebugMessage("The Target Window Is not Exist !");

		return FAIL;
	}

	ComPtr<IDXGIFactory> tFactory;
	ReturnIfFailed
	(
		CreateDXGIFactory2
		(
			DXGI_CREATE_FACTORY_DEBUG,
			IID_PPV_ARGS(tFactory.ReleaseAndGetAddressOf())
		),
		SwapChain::Create();
	);

	ComPtr<IDXGIFactory4> factory4;

	ReturnIfFailed
	(
		tFactory->QueryInterface(IID_PPV_ARGS(factory4.ReleaseAndGetAddressOf())),
		SwapChain::Create()
	);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = targetWindow.GetWindowWidth();
	swapChainDesc.Height = targetWindow.GetWindowHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = frameCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> tSwCh1;

	ReturnIfFailed
	(
		factory4->CreateSwapChainForHwnd
		(
			device.mCommandQueue.Get(),
			targetWindow.GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			tSwCh1.ReleaseAndGetAddressOf()
		),
		SwapChain::Create()
	);

	ReturnIfFailed
	(
		tSwCh1->QueryInterface(mSwapChain.ReleaseAndGetAddressOf()),
		SwapChain::Create()
	);
	return SUCCESS;
}

Result SwapChain::GetDesc(void* desc) const
{
	ReturnIfFailed
	(
		mSwapChain->GetDesc(reinterpret_cast<DXGI_SWAP_CHAIN_DESC*>(desc)),
		SwapChain::GetDesc()
	);

	return SUCCESS;
}

Result SwapChain::GetBuffer(const unsigned int bufferID, void** resource) const
{
	ReturnIfFailed
	(
		mSwapChain->GetBuffer(bufferID, IID_PPV_ARGS(reinterpret_cast<ID3D12Resource**>(resource))),
		SwapChain::GetBuffer()
	);
	return SUCCESS;
}

//　レンダーターゲット

RenderTarget::RenderTarget()
	:
	mRTV_Heaps(nullptr)
{

}

RenderTarget::~RenderTarget()
{
	for (int i = 0; i < mBufferCount; ++i)
	{
		SafeRelease(mRT_Resource[i].GetAddressOf());
	}

	System::SafeDeleteArray(&mRT_Resource);
}

// 深度ステンシルバッファ
DepthStencilBuffer::DepthStencilBuffer()
{

}

DepthStencilBuffer::~DepthStencilBuffer()
{

}

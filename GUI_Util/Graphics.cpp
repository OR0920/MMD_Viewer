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

	command.mCommandList->Close();

	command.mDevice = mDevice.Get();

	return SUCCESS;
}

Result Device::CreateRenderTarget(RenderTarget& renderTarget, const SwapChain& swapChain)
{
	DXGI_SWAP_CHAIN_DESC desc = {};
	if (swapChain.GetDesc(&desc) == Result::FAIL)
	{
		return FAIL;
	}

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
		auto result = swapChain.GetBuffer
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

	renderTarget.mIncrementSize 
		= mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	return SUCCESS;
}

Result Device::CreateDepthBuffer
(
	DepthStencilBuffer& depthStencilBuffer,
	const SwapChain& swapChain
)
{
	DXGI_SWAP_CHAIN_DESC swDesc = {};
	if (swapChain.GetDesc(&swDesc) == Result::FAIL)
	{
		return FAIL;
	}

	auto depthFormat = DXGI_FORMAT_D32_FLOAT;

	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D
	(
		depthFormat,
		swDesc.BufferDesc.Width,
		swDesc.BufferDesc.Height
	);
	resDesc.MipLevels = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	auto clearValue = CD3DX12_CLEAR_VALUE(depthFormat, 1.f, 0);

	ReturnIfFailed
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(depthStencilBuffer.mDSB_Resource.ReleaseAndGetAddressOf())
		),
		Device::CreateDepthBuffer()
	);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ReturnIfFailed
	(
		mDevice->CreateDescriptorHeap
		(
			&heapDesc,
			IID_PPV_ARGS(depthStencilBuffer.mDSV_Heap.ReleaseAndGetAddressOf())
		),
		Device::CreateDepthBuffer()
	);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	mDevice->CreateDepthStencilView
	(
		depthStencilBuffer.mDSB_Resource.Get(),
		&dsvDesc,
		depthStencilBuffer.mDSV_Heap->GetCPUDescriptorHandleForHeapStart()
	);


	return SUCCESS;
}

Result Device::CreateFence(Fence& fence)
{
	ReturnIfFailed
	(
		mDevice->CreateFence
		(
			fence.mFenceValue,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(fence.mFence.ReleaseAndGetAddressOf())
		),
		Device::CreateFence()
	);
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

void GraphicsCommand::BeginDraw()
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);
}

void GraphicsCommand::UnlockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetResource(mSwapChain->GetCurrentBackBufferIndex()).Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCommand::SetRenderTarget
(
	const RenderTarget* const renderTarget,
	const DepthStencilBuffer* const depthStencilBuffer
)
{
	if (renderTarget == nullptr)
	{
		DebugMessage
		(
			"ERROR: Render Target is nullptr ! \n at: "
			<< ToString(GraphicsCommand::SetRenderTarget())
		);
		return;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {}, dsvHandle = {};

	renderTarget->GetDescriptorHandle(&rtvHandle, mSwapChain->GetCurrentBackBufferIndex());

	if (depthStencilBuffer == nullptr)
	{
		mCommandList->OMSetRenderTargets(1, &rtvHandle, 0, nullptr);
		return;
	}

	depthStencilBuffer->GetDescriptorHandle(&dsvHandle);
	mCommandList->OMSetRenderTargets(1, &rtvHandle, 1, &dsvHandle);
}

void GraphicsCommand::LockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetResource(mSwapChain->GetCurrentBackBufferIndex()).Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCommand::EndDraw()
{
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(1, cmdLists);
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
	GraphicsCommand& device,
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

	device.mSwapChain = mSwapChain.Get();

	return SUCCESS;
}

int SwapChain::GetCurrentBackBufferIndex() const
{
	return mSwapChain->GetCurrentBackBufferIndex();
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
	mRTV_Heaps(nullptr),
	mRT_Resource(nullptr),
	mBufferCount(0),
	mViewPort({}),
	mScissorRect({})
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

void RenderTarget::GetDescriptorHandle(void* handlePtr, const int bufferID) const
{
	if (bufferID < 0 || mBufferCount <= bufferID)
	{
		DebugMessage("ERROR: The ID is Out of Range !");
		return ;
	}
	auto ptr = reinterpret_cast<D3D12_CPU_DESCRIPTOR_HANDLE*>(handlePtr);
	*ptr = mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();
	(*ptr).ptr += mIncrementSize * bufferID;
}

const ComPtr<ID3D12Resource> RenderTarget::GetResource(const int bufferID) const
{
	if (bufferID < 0 || mBufferCount <= bufferID)
	{
		DebugMessage("ERROR: The ID is Out of Range !");
		return nullptr;
	}
	return mRT_Resource[bufferID];
}

// 深度ステンシルバッファ
DepthStencilBuffer::DepthStencilBuffer()
	:
	mDSB_Resource(nullptr),
	mDSV_Heap(nullptr)
{

}

DepthStencilBuffer::~DepthStencilBuffer()
{

}

void DepthStencilBuffer::GetDescriptorHandle(void* handlePtr) const
{
	auto ptr = reinterpret_cast<D3D12_CPU_DESCRIPTOR_HANDLE*>(handlePtr);
	*ptr = mDSV_Heap->GetCPUDescriptorHandleForHeapStart();
}

// フェンス

Fence::Fence()
	:
	mFence(nullptr),
	mFenceValue(0)
{

}

Fence::~Fence()
{

}

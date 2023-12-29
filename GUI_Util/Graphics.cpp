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
#include"MathUtil.h"

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

Color::Color
(
	float _r,
	float _g,
	float _b,
	float _a
)
	:
	r(_r),
	g(_g),
	b(_b),
	a(_a)
{

}

Color::Color()
	:
	r(0.f),
	g(0.f),
	b(0.f),
	a(1.f)
{

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

	ReturnIfFailed
	(
		mDevice->CreateFence
		(
			command.mFenceValue,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(command.mFence.ReleaseAndGetAddressOf())
		),
		Device::CreateFence()
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
			i, renderTarget.mRT_Resource[i].ReleaseAndGetAddressOf()
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

Result Device::CreateRootSignature(RootSignature& rootSignature)
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = nullptr;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	ReturnIfFailed
	(
		D3D12SerializeRootSignature
		(
			&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			signature.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()
		),
		Device::CreateRootSignature()
	);

	ReturnIfFailed
	(
		mDevice->CreateRootSignature
		(
			0, signature->GetBufferPointer(), signature->GetBufferSize(),
			IID_PPV_ARGS(rootSignature.mRootSignature.ReleaseAndGetAddressOf())
		),
		Device::CreateRootSignature()
	);

	return SUCCESS;
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

void SwapChain::Present()
{
	mSwapChain->Present(1, 0);
}

Result SwapChain::GetDesc(DXGI_SWAP_CHAIN_DESC* desc) const
{
	ReturnIfFailed
	(
		mSwapChain->GetDesc(desc),
		SwapChain::GetDesc()
	);

	return SUCCESS;
}

Result SwapChain::GetBuffer(const unsigned int bufferID, ID3D12Resource** resource) const
{
	ReturnIfFailed
	(
		mSwapChain->GetBuffer(bufferID, IID_PPV_ARGS(resource)),
		SwapChain::GetBuffer()
	);
	return SUCCESS;
}


// コマンド
GraphicsCommand::GraphicsCommand()
	:
	mDevice(nullptr),
	mSwapChain(nullptr),
	mCommandQueue(nullptr),
	mCommandAllocator(nullptr),
	mCommandList(),
	rtvHandle({}),
	dsvHandle({}),
	mFence(nullptr),
	mFenceValue(0)
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

	auto viewport = renderTarget->GetViewPort();
	auto rect = renderTarget->GetRect();
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &rect);

	renderTarget->GetDescriptorHandle(rtvHandle, mSwapChain->GetCurrentBackBufferIndex());

	if (depthStencilBuffer == nullptr)
	{
		mCommandList->OMSetRenderTargets(1, &rtvHandle, 0, nullptr);
		return;
	}

	depthStencilBuffer->GetDescriptorHandle(dsvHandle);
	mCommandList->OMSetRenderTargets(1, &rtvHandle, 1, &dsvHandle);
}



void GraphicsCommand::ClearRenderTarget(const Color& color)
{
	float col[] = { color.r, color.g, color.b, color.a };
	mCommandList->ClearRenderTargetView(rtvHandle, col, 0, nullptr);
}



void GraphicsCommand::ClearDepthBuffer()
{
	mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
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
	mCommandList->Close();

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(1, cmdLists);

	mCommandQueue->Signal(mFence.Get(), ++mFenceValue);

	do
	{

	} while (mFence->GetCompletedValue() < mFenceValue);


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

void RenderTarget::GetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE& handle, const int bufferID) const
{
	if (bufferID < 0 || mBufferCount <= bufferID)
	{
		DebugMessage("ERROR: The ID is Out of Range !");
		return;
	}
	handle = mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += mIncrementSize * bufferID;
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

D3D12_VIEWPORT RenderTarget::GetViewPort() const 
{
	return mViewPort;
}

D3D12_RECT RenderTarget::GetRect() const
{
	return mScissorRect;
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

void DepthStencilBuffer::GetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE& handle) const
{
	handle = mDSV_Heap->GetCPUDescriptorHandleForHeapStart();
}


// ルートシグネチャ
RootSignature::RootSignature()
	:
	mRootSignature(nullptr)
{

}

RootSignature::~RootSignature()
{

}

// 入力レイアウト

InputElementDesc::InputElementDesc()
	:
	mInputElementDesc(nullptr)
{

}

InputElementDesc::~InputElementDesc()
{
	System::SafeDeleteArray(&mInputElementDesc);
}

void InputElementDesc::SetElementCount(const int count)
{
	System::SafeDeleteArray(&mInputElementDesc);

	mCount = count;
	mLastID = 0;
	mInputElementDesc = new D3D12_INPUT_ELEMENT_DESC[count]{};
}

void InputElementDesc::DefaultPosition(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	mInputElementDesc[mLastID] = 
	{ 
		semantics, 
		0, 
		DXGI_FORMAT_R32G32B32_FLOAT, 
		0, 
		0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 
	};

	mLastID++;
}

void InputElementDesc::DefaultColor(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	mInputElementDesc[mLastID] =
	{
		semantics,
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};
	
	mLastID++;
}

void InputElementDesc::DebugOutLayout() const
{
	for (int i = 0; i < mCount; ++i)
	{
		auto& ied = mInputElementDesc[i];
		DebugMessage(" { " << ied.SemanticName << " } ");
	}
}

bool InputElementDesc::IsSizeOver() const
{
	if (mCount <= mLastID)
	{
		DebugMessage("ERROR: It Cannot add Desc over the Desc Size. AT : " << ToString(InputElementDesc::DefaultPosition()));
		return true;
	}

	return false;
}

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

// �f�o�b�O���C��

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

// �f�o�C�X
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

	D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle
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
		mDevice->CreateRenderTargetView(renderTarget.mRT_Resource[i].Get(), &rtvDesc, mRTV_Handle);
		mRTV_Handle.ptr +=
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
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	ReturnIfFailed
	(
		D3D12SerializeRootSignature
		(
			&rootSignature.mDesc, D3D_ROOT_SIGNATURE_VERSION_1,
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

Result Device::CreateGraphicsPipeline(GraphicsPipeline& pipeline)
{
	ReturnIfFailed
	(
		mDevice->CreateGraphicsPipelineState
		(
			&pipeline.psoDesc,
			IID_PPV_ARGS(pipeline.mPipelineState.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsPipeline()
	);

	return SUCCESS;
}

Result Device::CreateVertexBuffer
(
	VertexBuffer& vertexBuffer,
	const unsigned int elementSize,
	const unsigned int elementCount
)
{
	DebugOutParam(elementSize);
	DebugOutParam(elementCount);

	auto bufferSize = elementSize * elementCount;
	DebugOutParam(bufferSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ReturnIfFailed
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(vertexBuffer.mResource.ReleaseAndGetAddressOf())
		),
		Device::CreateVertexBuffer()
	);


	vertexBuffer.mView.BufferLocation = vertexBuffer.mResource->GetGPUVirtualAddress();
	vertexBuffer.mView.SizeInBytes = bufferSize;
	vertexBuffer.mView.StrideInBytes = elementSize;

	vertexBuffer.mVertexCount = elementCount;

	return SUCCESS;
}

Result Device::CreateIndexBuffer
(
	IndexBuffer& indexBuffer,
	const unsigned int indexTypeSize,
	const unsigned int indexCount
)
{
	DebugOutParam(indexTypeSize);
	DebugOutParam(indexCount);

	auto bufferSize = indexTypeSize * indexCount;
	DebugOutParam(bufferSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ReturnIfFailed
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(indexBuffer.mResource.ReleaseAndGetAddressOf())
		),
		Device::CreateIndexBuffer()
	);

	indexBuffer.mView.BufferLocation = indexBuffer.mResource->GetGPUVirtualAddress();
	indexBuffer.mView.Format = DXGI_FORMAT_R32_UINT;
	indexBuffer.mView.SizeInBytes = bufferSize;

	indexBuffer.mIndexCount = indexCount;

	return SUCCESS;
}

Result Device::CreateConstantBuffer
(
	ConstantBuffer& constantBuffer,
	DescriptorHeapForShaderData& viewHeap,
	unsigned int bufferStructSize
)
{
		DebugMessage(ToString(Device::CreateConstantBuffer()));
	DebugMessage(bufferStructSize);
	auto bufferSize = D3D12Allignment(bufferStructSize);
	DebugMessage(bufferSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ReturnIfFailed
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constantBuffer.mResource.ReleaseAndGetAddressOf())
		),
		Device::CreateConstantBuffer()
	);

	auto& viewDesc = constantBuffer.mViewDesc;
	viewDesc.BufferLocation = constantBuffer.mResource->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = constantBuffer.mResource->GetDesc().Width;

	mDevice->CreateConstantBufferView
	(
		&viewDesc, viewHeap.GetCPU_Handle()
	);

	constantBuffer.mCPU_View = viewHeap.GetCPU_Handle();
	constantBuffer.mGPU_View = viewHeap.GetGPU_Handle();

	viewHeap.MoveToNextHeap();

	return SUCCESS;
}

Result Device::CreateDescriptorHeap
(
	DescriptorHeapForShaderData& heap,
	const unsigned int descriptorCount
)
{
	auto type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = descriptorCount;
	desc.Type = type;
	ReturnIfFailed
	(
		mDevice->CreateDescriptorHeap
		(
			&desc,
			IID_PPV_ARGS(heap.mDescriptorHeap.ReleaseAndGetAddressOf())
		),
		Device::CreateDescriptorHeap()
	);

	heap.mIncrementSize = mDevice->GetDescriptorHandleIncrementSize(type);
	heap.mDescriptorCount = descriptorCount;

	return SUCCESS;
}

// �X���b�v�`�F�C��

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
	if (targetWindow.GetCPU_Handle() == 0)
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
			targetWindow.GetCPU_Handle(),
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


// �R�}���h
GraphicsCommand::GraphicsCommand()
	:
	mDevice(nullptr),
	mSwapChain(nullptr),
	mCommandQueue(nullptr),
	mCommandAllocator(nullptr),
	mCommandList(),
	mRTV_Handle({}),
	mDSV_Handle({}),
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

void GraphicsCommand::BeginDraw(const GraphicsPipeline& pipeline)
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), pipeline.GetPipelineState().Get());
}

void GraphicsCommand::SetGraphicsPipeline(const GraphicsPipeline& pipeline)
{
	mCommandList->SetPipelineState(pipeline.GetPipelineState().Get());
}

void GraphicsCommand::UnlockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetGPU_Address(mSwapChain->GetCurrentBackBufferIndex()).Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCommand::SetRenderTarget
(
	const RenderTarget& renderTarget
)
{
	this->SetViewportAndRect(renderTarget);
	mCommandList->OMSetRenderTargets(1, &mRTV_Handle, 0, nullptr);
}

void GraphicsCommand::SetRenderTarget
(
	const RenderTarget& renderTarget,
	const DepthStencilBuffer& depthStencilBuffer
)
{
	this->SetViewportAndRect(renderTarget);
	depthStencilBuffer.GetDescriptorHandle(mDSV_Handle);
	mCommandList->OMSetRenderTargets(1, &mRTV_Handle, 1, &mDSV_Handle);
}

void GraphicsCommand::ClearRenderTarget(const Color& color)
{
	float col[] = { color.r, color.g, color.b, color.a };
	mCommandList->ClearRenderTargetView(mRTV_Handle, col, 0, nullptr);
}

void GraphicsCommand::ClearDepthBuffer()
{
	mCommandList->ClearDepthStencilView(mDSV_Handle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void GraphicsCommand::SetGraphicsRootSignature(const RootSignature& rootSignature)
{
	mCommandList->SetGraphicsRootSignature(rootSignature.GetRootSignature().Get());
}

void GraphicsCommand::SetDescriptorHeap(const DescriptorHeapForShaderData& descHeap)
{
	mCommandList->SetDescriptorHeaps(1, descHeap.GetDescriptorHeap().GetAddressOf());
}

void GraphicsCommand::SetConstantBuffer
(
	const ConstantBuffer& constBuffer,
	const int paramID
)
{
	mCommandList->SetGraphicsRootConstantBufferView
	(
		paramID,
		constBuffer.GetGPU_Address()
	);
}

void GraphicsCommand::DrawTriangle(const VertexBuffer& vertex)
{
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetVertexBuffers(0, 1, vertex.GetView());
	mCommandList->DrawInstanced(vertex.GetVertexCount(), 1, 0, 0);
}

void GraphicsCommand::DrawTriangleList
(
	const VertexBuffer& vertex,
	const IndexBuffer& index
)
{
	if (vertex.GetVertexCount() == 0 || index.GetIndexCount() == 0) return;

	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetVertexBuffers(0, 1, vertex.GetView());
	mCommandList->IASetIndexBuffer(index.GetView());
	mCommandList->DrawIndexedInstanced(index.GetIndexCount(), 1, 0, 0, 0);
}


void GraphicsCommand::LockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetGPU_Address(mSwapChain->GetCurrentBackBufferIndex()).Get(),
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

void GraphicsCommand::SetViewportAndRect(const RenderTarget& renderTarget)
{
	auto viewport = renderTarget.GetViewPort();
	mCommandList->RSSetViewports(1, &viewport);
	auto rect = renderTarget.GetRect();
	mCommandList->RSSetScissorRects(1, &rect);

	renderTarget.GetDescriptorHandle(mRTV_Handle, mSwapChain->GetCurrentBackBufferIndex());
}


//�@�����_�[�^�[�Q�b�g

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

const ComPtr<ID3D12Resource> RenderTarget::GetGPU_Address(const int bufferID) const
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

// �[�x�X�e���V���o�b�t�@
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


// ���[�g�V�O�l�`��
RootSignature::RootSignature()
	:
	mRootSignature(nullptr),
	mDesc({})
{
	mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	mDesc.NumParameters = 0;
	mDesc.pParameters = nullptr;
	mDesc.NumStaticSamplers = 0;
	mDesc.pStaticSamplers = nullptr;
}

RootSignature::~RootSignature()
{
	System::SafeDeleteArray(&mRootParamter);
}

void RootSignature::SetParameterCount(const int count)
{
	mDesc.NumParameters = count;

	System::SafeDeleteArray(&mRootParamter);
	mRootParamter = new D3D12_ROOT_PARAMETER[count]{};
	mDesc.NumParameters = count;
	mDesc.pParameters = mRootParamter;
}

void RootSignature::SetParamForCBV(const int paramID, const int registerID)
{
	auto& p = mRootParamter[paramID];
	p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	p.Descriptor.RegisterSpace = 0;
	p.Descriptor.ShaderRegister = registerID;
}

const ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature() const
{
	return mRootSignature;
}

// ���̓��C�A�E�g

InputElementDesc::InputElementDesc()
	:
	mCount(0),
	mLastID(0),
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

void InputElementDesc::SetDefaultPositionDesc(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	auto& desc = mInputElementDesc[mLastID];
	desc.SemanticName = semantics;
	desc.SemanticIndex = 0;
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.InputSlot = 0;
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	desc.InstanceDataStepRate = 0;

	mLastID++;
}

void InputElementDesc::SetDefaultColorDesc(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	auto& desc = mInputElementDesc[mLastID];
	desc.SemanticName = semantics;
	desc.SemanticIndex = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.InputSlot = 0;
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	desc.InstanceDataStepRate = 0;

	mLastID++;
}

void InputElementDesc::SetDefaultNormalDesc(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	auto& desc = mInputElementDesc[mLastID];
	desc.SemanticName = semantics;
	desc.SemanticIndex = 0;
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.InputSlot = 0;
	desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	desc.InstanceDataStepRate = 0;

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

int InputElementDesc::GetDescCount() const
{
	return mCount;
}

const D3D12_INPUT_ELEMENT_DESC* const InputElementDesc::GetElementDesc() const
{
	return mInputElementDesc;
}


bool InputElementDesc::IsSizeOver() const
{
	if (mCount <= mLastID)
	{
		DebugMessage("ERROR: The Desc has Not Space. AT : " << ToString(InputElementDesc::SetDefaultPositionDesc()));
		return true;
	}

	return false;
}

// �p�C�v���C���X�e�[�g

GraphicsPipeline::GraphicsPipeline()
	:
	mPipelineState(nullptr),
	psoDesc({})
{
	//psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	//psoDesc.pRootSignature = m_rootSignature.Get();
	//psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	//psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

}

GraphicsPipeline::~GraphicsPipeline()
{

}

void GraphicsPipeline::SetDepthEnable()
{
	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
}

void GraphicsPipeline::SetInputLayout(const InputElementDesc& inputElementDesc)
{
	psoDesc.InputLayout.NumElements = inputElementDesc.GetDescCount();
	psoDesc.InputLayout.pInputElementDescs = inputElementDesc.GetElementDesc();
}

void GraphicsPipeline::SetRootSignature(const RootSignature& rootSignatue)
{
	psoDesc.pRootSignature = rootSignatue.GetRootSignature().Get();
}

void GraphicsPipeline::SetVertexShader(const unsigned char* const vertexShader, const int length)
{
	psoDesc.VS.BytecodeLength = length;
	psoDesc.VS.pShaderBytecode = vertexShader;
}

void GraphicsPipeline::SetPixelShader(const unsigned char* const pixelShader, const int length)
{
	psoDesc.PS.BytecodeLength = length;
	psoDesc.PS.pShaderBytecode = pixelShader;
}

const ComPtr<ID3D12PipelineState> GraphicsPipeline::GetPipelineState() const
{
	return mPipelineState;
}

// ���_�o�b�t�@

VertexBuffer::VertexBuffer()
	:
	mResource(nullptr),
	mView({})
{

}

VertexBuffer::~VertexBuffer()
{

}

Result VertexBuffer::Copy(const void* const data)
{
	unsigned char* mappedVertex = nullptr;
	auto range = CD3DX12_RANGE(0, 0);
	ReturnIfFailed
	(
		mResource->Map
		(
			0, &range, reinterpret_cast<void**>(&mappedVertex)
		),
		VertexBuffer::Copy()
	);

	std::memcpy(mappedVertex, data, mView.SizeInBytes);

	mResource->Unmap(0, nullptr);

	return SUCCESS;
}

const D3D12_VERTEX_BUFFER_VIEW* const VertexBuffer::GetView() const
{
	return &mView;
}

const int VertexBuffer::GetVertexCount() const
{
	return mVertexCount;
}

// �C���f�b�N�X�o�b�t�@

IndexBuffer::IndexBuffer()
	:
	mResource(nullptr),
	mView({}),
	mIndexCount(0)
{

}

IndexBuffer::~IndexBuffer()
{

}

Result IndexBuffer::Copy(const void* const data)
{
	unsigned char* mappedIndex = nullptr;

	auto range = CD3DX12_RANGE(0, 0);
	ReturnIfFailed
	(
		mResource->Map(0, &range, reinterpret_cast<void**>(&mappedIndex)),
		IndexBuffer::Copy()
	);

	std::memcpy(mappedIndex, data, mView.SizeInBytes);

	mResource->Unmap(0, nullptr);

	return SUCCESS;
}

const D3D12_INDEX_BUFFER_VIEW* const IndexBuffer::GetView() const
{
	return &mView;
}

const int IndexBuffer::GetIndexCount() const
{
	return mIndexCount;
}

//�@�萔�o�b�t�@

ConstantBuffer::ConstantBuffer()
	:
	mResource(nullptr)
{

}

ConstantBuffer::~ConstantBuffer()
{

}

Result ConstantBuffer::Map(void** ptr)
{
	ReturnIfFailed
	(
		mResource->Map
		(
			0, nullptr, ptr
		),
		ConstantBuffer::Map()
	);

	return SUCCESS;
}

const D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPU_Address() const
{
	return mViewDesc.BufferLocation;
}

// �f�B�X�N���v�^�q�[�v

DescriptorHeapForShaderData::DescriptorHeapForShaderData()
	:
	mDescriptorHeap(nullptr),
	mDescriptorCount(0),
	mIncrementSize(0),
	mLastID(0)
{
}

DescriptorHeapForShaderData::~DescriptorHeapForShaderData()
{

}

const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapForShaderData::GetCPU_Handle()
{
	if (mDescriptorCount <= mLastID)
	{
		return {};
	}

	auto ret = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	ret.ptr += mLastID * mIncrementSize;
	return ret;
}

const D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapForShaderData::GetGPU_Handle()
{
	if (mDescriptorCount <= mLastID)
	{
		return {};
	}

	auto ret = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	ret.ptr += mLastID * mIncrementSize;
	return ret;
}

void DescriptorHeapForShaderData::MoveToNextHeap()
{
	mLastID++;
}

const ComPtr<ID3D12DescriptorHeap> DescriptorHeapForShaderData::GetDescriptorHeap() const
{
	return mDescriptorHeap;
}
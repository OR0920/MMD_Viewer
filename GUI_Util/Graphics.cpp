// �����������Ȃ�̂Ŋu��

#include"GUI_Util.h"

// std
#include<cassert>

// windows
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#include"d3dx12.h"
#include<d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include<directxtex.h>

// �o�b�t�@�̃T�C�Y���A���C�������g����
#define D3D12Allignment(buffSize) (buffSize + 0xff) & ~0xff;

// my lib
#include "System.h"
#include"MathUtil.h"

// API�����s�����ꍇ�Ɏ��s��Ԃ�
#define ReturnIfFailed(func, at)\
{\
	auto result = func;\
	if(FAILED(result))\
	{\
		DebugMessageFunctionError(func, at);\
		DebugMessageNewLine();\
		DebugOutParamHex(result);\
		DebugMessageNewLine();\
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


#define ReturnIfSuccess(func)\
{\
	auto result = func;\
	if(SUCCEEDED(result))\
	{\
		return SUCCESS;\
	}\
}

// �e�N�X�`���f�[�^�̎����N���X
// DirectX Tex�����b�s���O
namespace GUI
{
	namespace Graphics
	{
		class TextureData
		{
		public:
			TextureData() : mMetaData({}), mImage({}) {}

			const DirectX::TexMetadata& GetMetaData() const
			{
				return mMetaData;
			}
			const DirectX::ScratchImage& GetImage() const
			{
				return mImage;
			}

			Result LoadFromFile(const wchar_t* const filepath)
			{
				// �e�t�H�[�}�b�g�ŕЂ��[����ǂݍ���
				// ���������烊�^�[��
				ReturnIfSuccess
				(
					DirectX::LoadFromWICFile
					(
						filepath, DirectX::WIC_FLAGS_NONE,
						&mMetaData, mImage
					)
				);

				ReturnIfSuccess
				(
					DirectX::LoadFromDDSFile
					(
						filepath, DirectX::DDS_FLAGS_NONE,
						&mMetaData, mImage
					)
				);

				ReturnIfSuccess
				(
					DirectX::LoadFromTGAFile
					(
						filepath, DirectX::TGA_FLAGS_NONE,
						&mMetaData, mImage
					)
				);

				return FAIL;
			}

		private:
			DirectX::TexMetadata mMetaData;
			DirectX::ScratchImage mImage;
		};
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

static const DXGI_FORMAT gDxgiFormat[Format::FORMAT_COUNT] =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R32G32B32A32_FLOAT
};

// �f�o�b�O���C���L����
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
	// �t�B�[�`���[���x���͂Ƃ肠����12_0�ō쐬
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
	// �O���t�B�b�N�X�p�̃R�}���h���X�g
	auto type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// �R�}���h�L���[�@�R�}���h���s�̏��Ԃ��Ǘ�
	// �Ƃ肠����������@�����̃R�}���h�L���[���Ǘ�����ꍇ�ʃN���X��������
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

	// �R�}���h�A���P�[�^�[�@�R�}���h���L�^���郁�������Ǘ�
	ReturnIfFailed
	(
		mDevice->CreateCommandAllocator
		(
			type,
			IID_PPV_ARGS(command.mCommandAllocator.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	// �R�}���h���X�g�@�������ăR�}���h���L�^����
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

	// �t�F���X�A���̂Ƃ���ʃN���X�ɂ���K�v�����Ȃ��̂ŁA�����ɉB�؂�
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

	// ���X�g����Ă���
	command.mCommandList->Close();

	// �f�o�C�X���擾���Ă���
	command.mDevice = mDevice.Get();

	return SUCCESS;
}

Result Device::CreateRenderTarget(RenderTarget& renderTarget, const SwapChain& swapChain)
{
	// �X���b�v�`�F�C���̏��擾
	DXGI_SWAP_CHAIN_DESC desc = {};
	if (swapChain.GetDesc(&desc) == Result::FAIL)
	{
		return FAIL;
	}

	// �o�b�N�o�b�t�@�̏����擾
	auto& bufferCount = renderTarget.mBufferCount = desc.BufferCount;

	// �����_�[�^�[�Q�b�g�̃r���[��u���������̈�����
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
			IID_PPV_ARGS(renderTarget.mHeaps.ReleaseAndGetAddressOf())
		),
		Device::CreateRenderTarget()
	);

	// �X���b�v�`�F�C������A�`�������炢�A�r���[���쐬����
	renderTarget.mResource = new ComPtr<ID3D12Resource>[bufferCount] {nullptr};

	D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle
		= renderTarget.mHeaps->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < renderTarget.mBufferCount; ++i)
	{
		auto result = swapChain.GetBuffer
		(
			i, renderTarget.mResource[i].ReleaseAndGetAddressOf()
		);

		if (result == Result::FAIL)
		{
			return FAIL;
		}

		rtvDesc.Format = renderTarget.mResource[i]->GetDesc().Format;
		mDevice->CreateRenderTargetView(renderTarget.mResource[i].Get(), &rtvDesc, mRTV_Handle);
		mRTV_Handle.ptr +=
			mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// �`�掞�ɕK�v�ɂȂ�������炤
	renderTarget.mViewPort = CD3DX12_VIEWPORT(renderTarget.mResource[0].Get());
	renderTarget.mScissorRect
		= CD3DX12_RECT(0, 0, desc.BufferDesc.Width, desc.BufferDesc.Width);

	renderTarget.mViewIncrementSize
		= mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	renderTarget.mAspectRatio = swapChain.GetAspectRatio();

	return SUCCESS;
}

Result Device::CreateSubRenderTarget
(
	SubRenderTarget& subRenderTarget,
	const RenderTarget& mainRenderTarget,
	const Format format[],
	const int count
)
{
	subRenderTarget.mTargetCount = count;

	// �f�B�X�N���v�^�q�[�v�쐬(�����_�[�^�[�Q�b�g�p)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = count;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ReturnIfFailed
	(
		mDevice->CreateDescriptorHeap
		(
			&heapDesc,
			IID_PPV_ARGS(subRenderTarget.mRTV_Heaps.ReleaseAndGetAddressOf())
		),
		Device::CreateSubRenderTarget()
	);

	// �f�B�X�N���v�^�q�[�v�쐬(�e�N�X�`���p)
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ReturnIfFailed
	(
		mDevice->CreateDescriptorHeap
		(
			&heapDesc,
			IID_PPV_ARGS(subRenderTarget.mSRV_Heaps.ReleaseAndGetAddressOf())
		),
		Device::CreateSubRenderTarget()
	);

	subRenderTarget.mResource = new ComPtr<ID3D12Resource>[count];

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.MipLevels = 1;
	resDesc.Width = mainRenderTarget.GetWidth();
	resDesc.Height = mainRenderTarget.GetHeight();
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.DepthOrArraySize = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	// �����l�̐ݒ�
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Color[0] = 0.5f;
	clearValue.Color[1] = 0.5f;
	clearValue.Color[2] = 0.5f;
	clearValue.Color[3] = 1.f;
	clearValue.DepthStencil.Depth = 1.f;
	clearValue.DepthStencil.Stencil = 0;

	auto rtvHeapHandle =
		subRenderTarget.mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();


	// �e�N�X�`���Ƃ��ė��p����ۂ̃r���[�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto srvHeapHandle =
		subRenderTarget.mSRV_Heaps->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < count; ++i)
	{
		resDesc.Format
			= clearValue.Format
			= srvDesc.Format
			= gDxgiFormat[format[i]];

		ReturnIfFailed
		(
			mDevice->CreateCommittedResource
			(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				&clearValue,
				IID_PPV_ARGS(subRenderTarget.mResource[i].ReleaseAndGetAddressOf())
			),
			Device::CreateSubRenderTarget()
		);

		mDevice->CreateRenderTargetView
		(
			subRenderTarget.mResource[i].Get(),
			NULL,
			rtvHeapHandle
		);

		rtvHeapHandle.ptr +=
			mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		mDevice->CreateShaderResourceView
		(
			subRenderTarget.mResource[i].Get(),
			&srvDesc,
			srvHeapHandle
		);

		srvHeapHandle.ptr +=
			mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	return SUCCESS;
}


Result Device::CreateDepthBuffer
(
	DepthStencilBuffer& depthStencilBuffer,
	const SwapChain& swapChain
)
{
	// �X���b�v�`�F�[���̏����擾
	DXGI_SWAP_CHAIN_DESC swDesc = {};
	if (swapChain.GetDesc(&swDesc) == Result::FAIL)
	{
		return FAIL;
	}

	// 4�o�C�g�S���A�[�x�l�Ƃ��Ďg��
	auto depthFormat = DXGI_FORMAT_D32_FLOAT;

	// �[�x�o�b�t�@�̐ݒ�A�ʂɐݒ肵�����Ȃ����烁�\�b�h��
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D
	(
		depthFormat,
		swDesc.BufferDesc.Width,
		swDesc.BufferDesc.Height
	);
	resDesc.MipLevels = 1;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// �N���A�Ɏg���l
	auto clearValue = CD3DX12_CLEAR_VALUE(depthFormat, 1.f, 0);

	// �[�x�o�b�t�@�쐬
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

	// �r���[�����ꏊ��p��
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

	// �r���[�����
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

	// ���[�g�V�O�l�`���̃o�C�i�����쐬
	ReturnIfFailed
	(
		D3D12SerializeRootSignature
		(
			&rootSignature.mDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			signature.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()
		),
		Device::CreateRootSignature()
	);

	// �o�C�i�������ƂɃV�O�l�`���쐬
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
	if (elementCount == 0) return FAIL;

	// �o�b�t�@�S�̂̃T�C�Y
	auto bufferSize = elementSize * elementCount;

	// ���\�[�X���쐬
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

	// �r���[��ݒ�
	vertexBuffer.mView.BufferLocation = vertexBuffer.mResource->GetGPUVirtualAddress();
	vertexBuffer.mView.SizeInBytes = bufferSize;
	vertexBuffer.mView.StrideInBytes = elementSize;

	// ���_�����L�^���Ă���
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
	if (indexCount == 0) return FAIL;

	// �o�b�t�@�S�̂̃T�C�Y
	auto bufferSize = indexTypeSize * indexCount;

	// ���\�[�X���쐬
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

	// �r���[��ݒ�
	indexBuffer.mView.BufferLocation = indexBuffer.mResource->GetGPUVirtualAddress();
	indexBuffer.mView.Format = DXGI_FORMAT_R32_UINT;
	indexBuffer.mView.SizeInBytes = bufferSize;

	// ���_�����L�^���Ă���
	indexBuffer.mIndexCount = indexCount;

	return SUCCESS;
}

Result Device::CreateConstantBuffer
(
	ConstantBuffer& constantBuffer,
	DescriptorHeap& viewHeap,
	const unsigned int bufferStructSize,
	const unsigned int bufferCount
)
{
	// �A���C�������g���ꂽ�v�f�������̃T�C�Y
	auto bufferStructSizeAllignmented = D3D12Allignment(bufferStructSize);

	// �o�b�t�@�S�̂̃T�C�Y
	auto bufferSize = bufferStructSizeAllignmented * bufferCount;

	// ���\�[�X���쐬
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

	// �f�B�X�N���v�^�q�[�v�Ƀr���[���쐬����
	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
	viewDesc.BufferLocation = constantBuffer.mResource->GetGPUVirtualAddress();
	viewDesc.SizeInBytes = bufferStructSizeAllignmented;

	constantBuffer.mViewDesc = viewDesc;
	constantBuffer.mCPU_Handle = viewHeap.GetCurrentCPU_Handle();
	constantBuffer.mGPU_Handle = viewHeap.GetCurrentGPU_Handle();

	for (unsigned int i = 0; i < bufferCount; ++i)
	{
		mDevice->CreateConstantBufferView
		(
			&viewDesc, viewHeap.GetCurrentCPU_Handle()
		);

		viewDesc.BufferLocation += bufferStructSizeAllignmented;
		viewHeap.MoveToNextHeapPos();
	}

	constantBuffer.mViewIncrementSize =
		mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return SUCCESS;
}

Result Device::CreateTexture2D
(
	Texture2D& texture,
	DescriptorHeap& viewHeap
)
{
	// ���\�[�X���쐬
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	auto& metadata = texture.mData->GetMetaData();
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = metadata.format;
	resDesc.Width = metadata.width;
	resDesc.Height = metadata.height;
	resDesc.DepthOrArraySize = metadata.arraySize;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.MipLevels = metadata.mipLevels;
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;

	ReturnIfFailed
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(texture.mResource.GetAddressOf())
		),
		Device::CreateTexture2D();
	);

	// �e�N�X�`���̃f�[�^���R�s�[����
	if (texture.WriteToSubresource() == FAIL) return FAIL;

	// �r���[���쐬
	auto& viewDesc = texture.mViewDesc;
	viewDesc.Format = texture.mResource->GetDesc().Format;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;

	mDevice->CreateShaderResourceView
	(
		texture.mResource.Get(),
		&viewDesc,
		viewHeap.GetCurrentCPU_Handle()
	);

	texture.mGPU_Handle = viewHeap.GetCurrentGPU_Handle();
	texture.mViewIncrementSize = viewHeap.mViewIncrementSize;

	viewHeap.MoveToNextHeapPos();

	return SUCCESS;
}

Result Device::CreateDescriptorHeap
(
	DescriptorHeap& heap,
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

	heap.mViewIncrementSize = mDevice->GetDescriptorHandleIncrementSize(type);
	heap.mDescriptorCount = descriptorCount;
	heap.mLastID = 0;

	return SUCCESS;
}

// �X���b�v�`�F�C��

SwapChain::SwapChain()
	:
	mSwapChain(nullptr),
	mAspectRatio(0.f)
{

}

SwapChain::~SwapChain()
{

}

Result SwapChain::Create
(
	GraphicsCommand& command,
	const ParentWindow& targetWindow,
	const int frameCount
)
{
	// �E�B���h�E���Ȃ��ƍ��Ȃ�
	if (targetWindow.GetHandle() == 0)
	{
		DebugMessage("The Target Window Is not Exist !");

		return FAIL;
	}

	mAspectRatio =
		static_cast<float>(targetWindow.GetClientWidth())
		/ static_cast<float>(targetWindow.GetClientHeight());

	// DXGI�̃C���^�[�t�F�[�X�𐶐�����C���^�[�t�F�[�X
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

	// IDXGIFactory����AIDXGIFactory4�����炤
	ComPtr<IDXGIFactory4> factory4;
	ReturnIfFailed
	(
		tFactory->QueryInterface(IID_PPV_ARGS(factory4.ReleaseAndGetAddressOf())),
		SwapChain::Create()
	);

	// �X���b�v�`�F�C���̐ݒ�A���ڕʂɐݒ肵�����Ȃ����烁�\�b�h��
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = targetWindow.GetClientWidth();
	swapChainDesc.Height = targetWindow.GetClientHeight();
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

	// SwapChain1�����ASwapChain4�����炤
	ComPtr<IDXGISwapChain1> tSwCh1;
	ReturnIfFailed
	(
		factory4->CreateSwapChainForHwnd
		(
			command.mCommandQueue.Get(),
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

	// �`�掞�ɕK�v�ɂȂ邽�߁A�R�}���h�ɍ��񂵂��Ă���
	command.mSwapChain = this;

	return SUCCESS;
}

const float SwapChain::GetAspectRatio() const
{
	return mAspectRatio;
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
	// �R�}���h�����Z�b�g
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);
}

void GraphicsCommand::SetGraphicsPipeline(const GraphicsPipeline& pipeline)
{
	mCommandList->SetPipelineState(pipeline.GetPipelineState().Get());
}

void GraphicsCommand::UnlockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetRenderTargetResource(mSwapChain->GetCurrentBackBufferIndex()).Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCommand::UnlockRenderTarget(const SubRenderTarget& renderTarget)
{
	for (int i = 0; i < renderTarget.GetTargetCount(); ++i)
	{
		auto barreir = CD3DX12_RESOURCE_BARRIER::Transition
		(
			renderTarget.GetRenderTargetResource(i).Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		mCommandList->ResourceBarrier(1, &barreir);
	}
}

void GraphicsCommand::SetRenderTarget
(
	const RenderTarget& renderTarget
)
{
	mRTV_Handle = renderTarget.GetDescriptorHandle(mSwapChain->GetCurrentBackBufferIndex());
	mCommandList->OMSetRenderTargets(1, &mRTV_Handle, 0, nullptr);
}

void GraphicsCommand::SetRenderTarget
(
	const SubRenderTarget& renderTarget
)
{
	mRTV_Handle = renderTarget.GetRTV_DescriptorHandle();
	mCommandList->OMSetRenderTargets(renderTarget.GetTargetCount(), &mRTV_Handle, 0, nullptr);
}

void GraphicsCommand::SetRenderTarget
(
	const RenderTarget& renderTarget,
	const DepthStencilBuffer& depthStencilBuffer
)
{
	mRTV_Handle = renderTarget.GetDescriptorHandle(mSwapChain->GetCurrentBackBufferIndex());
	mDSV_Handle = depthStencilBuffer.GetDescriptorHandle();
	mCommandList->OMSetRenderTargets(1, &mRTV_Handle, 1, &mDSV_Handle);
}

void GraphicsCommand::SetRenderTarget
(
	const SubRenderTarget& renderTarget,
	const DepthStencilBuffer& depthStencilBuffer
)
{
	mRTV_Handle = renderTarget.GetRTV_DescriptorHandle();
	mDSV_Handle = depthStencilBuffer.GetDescriptorHandle();
	mCommandList->OMSetRenderTargets(renderTarget.GetTargetCount(), &mRTV_Handle, 1, &mDSV_Handle);
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

void GraphicsCommand::SetDescriptorHeap(const DescriptorHeap& descHeap)
{
	mCommandList->SetDescriptorHeaps(1, descHeap.GetDescriptorHeap().GetAddressOf());
}

void GraphicsCommand::SetConstantBuffer
(
	const ConstantBuffer& constBuffer,
	const int paramID,
	const int bufferID
)
{
	mCommandList->SetGraphicsRootConstantBufferView
	(
		paramID,
		constBuffer.GetGPU_Address(bufferID)
	);
}


void GraphicsCommand::SetDescriptorTable
(
	const SignateBuffer& buffer,
	const int paramID,
	const int bufferID
)
{
	mCommandList->SetGraphicsRootDescriptorTable
	(
		paramID, buffer.GetGPU_Handle(bufferID)
	);
}

void GraphicsCommand::SetVertexBuffer
(
	const VertexBuffer& vertex
)
{
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetVertexBuffers(0, 1, vertex.GetView());
}

void GraphicsCommand::SetVertexBuffer
(
	const VertexBuffer& vertex,
	const IndexBuffer& index
)
{
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->IASetVertexBuffers(0, 1, vertex.GetView());
	mCommandList->IASetIndexBuffer(index.GetView());
}

void GraphicsCommand::DrawTriangle(const int vertexCount)
{
	mCommandList->DrawInstanced(vertexCount, 1, 0, 0);
}

void GraphicsCommand::DrawTriangleList
(
	const int indexCount,
	const int offs
)
{
	mCommandList->DrawIndexedInstanced(indexCount, 1, offs, 0, 0);
}


void GraphicsCommand::LockRenderTarget(const RenderTarget& renderTarget)
{
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		renderTarget.GetRenderTargetResource(mSwapChain->GetCurrentBackBufferIndex()).Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier(1, &barrier);
}

void GraphicsCommand::LockRenderTarget(const SubRenderTarget& renderTarget)
{
	for (int i = 0; i < renderTarget.GetTargetCount(); ++i)
	{
		auto barreir = CD3DX12_RESOURCE_BARRIER::Transition
		(
			renderTarget.GetRenderTargetResource(i).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);
		
		mCommandList->ResourceBarrier(1, &barreir);
	}
}

void GraphicsCommand::EndDraw()
{
	mCommandList->Close();

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(1, cmdLists);

	mCommandQueue->Signal(mFence.Get(), ++mFenceValue);

	// �Ƃ肠�����V���v���Ƀr�W�[���[�v�ő҂�
	do
	{

	} while (mFence->GetCompletedValue() < mFenceValue);
}

void GraphicsCommand::Flip()
{
	mSwapChain->Present();
}


void GraphicsCommand::SetViewportAndRect(const RenderTarget& renderTarget)
{
	auto viewport = renderTarget.GetViewPort();
	mCommandList->RSSetViewports(1, &viewport);
	auto rect = renderTarget.GetRect();
	mCommandList->RSSetScissorRects(1, &rect);
}


//�@�����_�[�^�[�Q�b�g

RenderTarget::RenderTarget()
	:
	mHeaps(nullptr),
	mResource(nullptr),
	mBufferCount(0),
	mViewIncrementSize(0),
	mViewPort({}),
	mScissorRect({}),
	mAspectRatio(0.f)
{

}

RenderTarget::~RenderTarget()
{
	for (int i = 0; i < mBufferCount; ++i)
	{
		SafeRelease(mResource[i].GetAddressOf());
	}

	System::SafeDeleteArray(&mResource);
}

const float RenderTarget::GetAspectRatio() const
{
	return mAspectRatio;
}

const D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDescriptorHandle(const int bufferID) const
{
	if (bufferID < 0 || mBufferCount <= bufferID)
	{
		DebugMessage("ERROR: The ID is Out of Range !");
		assert(false);
	}
	auto h = mHeaps->GetCPUDescriptorHandleForHeapStart();
	h.ptr += mViewIncrementSize * bufferID;
	return h;
}

const ComPtr<ID3D12Resource> RenderTarget::GetRenderTargetResource(const int bufferID) const
{
	IS_OUT_OF_RANGE(mResource, bufferID, mBufferCount);

	return mResource[bufferID];
}

const DXGI_FORMAT RenderTarget::GetFormat() const
{
	return mResource[0]->GetDesc().Format;
}

const int RenderTarget::GetWidth() const
{
	return mResource[0]->GetDesc().Width;
}

const int RenderTarget::GetHeight() const
{
	return mResource[0]->GetDesc().Height;
}

const D3D12_VIEWPORT& RenderTarget::GetViewPort() const
{
	return mViewPort;
}

const D3D12_RECT& RenderTarget::GetRect() const
{
	return mScissorRect;
}

// �}���`�p�X�p�����_�[�^�[�Q�b�g
SubRenderTarget::SubRenderTarget()
	:
	mRTV_Heaps(nullptr),
	mSRV_Heaps(nullptr),
	mResource(nullptr),
	mTargetCount(0)
{

}

SubRenderTarget::~SubRenderTarget()
{
	System::SafeDeleteArray(&mResource);
}

const int SubRenderTarget::GetTargetCount() const
{
	return mTargetCount;
}

const ComPtr<ID3D12Resource> SubRenderTarget::GetRenderTargetResource(const int i) const
{
	IS_OUT_OF_RANGE(mResource, i, mTargetCount);

	return mResource[i];
}

const D3D12_CPU_DESCRIPTOR_HANDLE& SubRenderTarget::GetRTV_DescriptorHandle() const
{
	return mRTV_Heaps->GetCPUDescriptorHandleForHeapStart();
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

const D3D12_CPU_DESCRIPTOR_HANDLE& DepthStencilBuffer::GetDescriptorHandle() const
{
	return mDSV_Heap->GetCPUDescriptorHandleForHeapStart();
}


// �f�B�X�N���v�^�����W
DescriptorRange::DescriptorRange()
	:
	mRange(nullptr),
	mRangeCount(0)
{

}

DescriptorRange::~DescriptorRange()
{
	System::SafeDeleteArray(&mRange);
}


void DescriptorRange::SetRangeCount(const int rangeCount)
{
	mRangeCount = rangeCount;
	mRange = new D3D12_DESCRIPTOR_RANGE[rangeCount];

}

void DescriptorRange::SetRangeForCBV
(
	const int rangeID,
	const int registerID,
	const int descriptorCount
)
{
	if (mRangeCount <= rangeID)
	{
		assert(false);
	}

	// �萔�o�b�t�@�r���[�p�Ƀ����W��ݒ�
	auto& r = mRange[rangeID];
	r.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	r.BaseShaderRegister = registerID;
	r.NumDescriptors = descriptorCount;
	r.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	r.RegisterSpace = 0;
}

void DescriptorRange::SetRangeForSRV
(
	const int rangeID,
	const int registerID,
	const int descriptorCount
)
{
	if (mRangeCount < rangeID)
	{
		assert(false);
	}

	// �V�F�[�_�[���\�[�X�r���[�p�Ƀ����W��ݒ�
	auto& r = mRange[rangeID];
	r = {};
	r.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	r.BaseShaderRegister = registerID;
	r.NumDescriptors = descriptorCount;
	r.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	r.RegisterSpace = 0;
}

int DescriptorRange::GetRangeCount() const
{
	return mRangeCount;
}

const D3D12_DESCRIPTOR_RANGE* const DescriptorRange::GetRange() const
{
	return mRange;
}



// ���[�g�V�O�l�`��
RootSignature::RootSignature()
	:
	mRootSignature(nullptr),
	mDesc({}),
	mRootParamter(nullptr),
	mSamplerDesc(nullptr)
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
	System::SafeDeleteArray(&mSamplerDesc);
}

void RootSignature::SetParameterCount(const int count)
{
	System::SafeDeleteArray(&mRootParamter);
	mRootParamter = new D3D12_ROOT_PARAMETER[count]{};
	mDesc.NumParameters = count;
	mDesc.pParameters = mRootParamter;
}

void RootSignature::SetParamForCBV(const int paramID, const int registerID)
{
	if (IsParamSizeOver(paramID)) return;

	// �w�肳�ꂽ���[�g�p�����[�^��萔�o�b�t�@�r���[�p�ɐݒ�
	auto& p = mRootParamter[paramID];
	p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	p.Descriptor.RegisterSpace = 0;
	p.Descriptor.ShaderRegister = registerID;
}

void RootSignature::SetParamForSRV(const int paramID, const int registerID)
{
	if (IsParamSizeOver(paramID)) return;

	//�w�肳�ꂽ���[�g�p�����[�^��SRV�p�ɐݒ�
	auto& p = mRootParamter[paramID];
	p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	p.Descriptor.RegisterSpace = 0;
	p.Descriptor.ShaderRegister = registerID;
}

void RootSignature::SetParamForDescriptorTable
(
	const int paramID,
	const DescriptorRange& range
)
{
	if (IsParamSizeOver(paramID)) return;

	// �w�肳�ꂽ�p�����[�^���f�B�X�N���v�^�e�[�u���p�ɐݒ肵�A�����W���Z�b�g
	auto& p = mRootParamter[paramID];
	p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	p.DescriptorTable.NumDescriptorRanges = range.GetRangeCount();
	p.DescriptorTable.pDescriptorRanges = range.GetRange();

}

void RootSignature::SetStaticSamplerCount(const int count)
{
	System::SafeDeleteArray(&mSamplerDesc);
	mSamplerDesc = new D3D12_STATIC_SAMPLER_DESC[count]{};
	mDesc.NumStaticSamplers = count;
	mDesc.pStaticSamplers = mSamplerDesc;
}

void RootSignature::SetSamplerDefault(const int samplerID, const int registerID)
{
	if (IsSamplerSizeOver(samplerID)) return;

	// �f�t�H���g�̃T���v���[
	CD3DX12_STATIC_SAMPLER_DESC desc = {};
	desc.Init(registerID);

	mSamplerDesc[samplerID] = desc;
}

void RootSignature::SetSamplerUV_Clamp(const int samplerID, const int registerID)
{
	if (IsSamplerSizeOver(samplerID)) return;

	// UV�����[�v�����Ȃ�
	CD3DX12_STATIC_SAMPLER_DESC desc = {};
	desc.Init
	(
		registerID,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);
	mSamplerDesc[samplerID] = desc;
}

const ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature() const
{
	return mRootSignature;
}

bool RootSignature::IsParamSizeOver(const int i) const
{
	if (i < 0 || mDesc.NumParameters <= i)
	{
		DebugMessage("The id " << i << " is over Size !");
		assert(false);
		return true;
	}
	return false;
}

bool RootSignature::IsSamplerSizeOver(const int i) const
{
	if (i < 0 || mDesc.NumStaticSamplers <= i)
	{
		DebugMessage("The id " << i << " is over Size !");
		assert(false);
		return true;
	}
	return false;
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

void InputElementDesc::SetDefaultUV_Desc(const char* const semantics)
{
	if (IsSizeOver() == true) return;

	auto& desc = mInputElementDesc[mLastID];
	desc.SemanticName = semantics;
	desc.SemanticIndex = 0;
	desc.Format = DXGI_FORMAT_R32G32_FLOAT;
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
	// �f�t�H���g�̐ݒ�
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.StencilEnable = false;

	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	psoDesc.NumRenderTargets = 1;

	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
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

void GraphicsPipeline::SetAlphaEnable()
{
	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void GraphicsPipeline::SetCullDisable()
{
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
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
	if (mResource == nullptr) return FAIL;

	// �}�b�v���ăR�s�[
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
	if (mResource == nullptr) return FAIL;

	// �}�b�v���ăR�s�[
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

// ���[�g�V�O�l�`���Ńo�C���h����郊�\�[�X

SignateBuffer::~SignateBuffer() {}

//�@�萔�o�b�t�@

ConstantBuffer::ConstantBuffer()
	:
	mResource(nullptr),
	mViewDesc({}),
	mCPU_Handle({}),
	mGPU_Handle({}),
	mViewIncrementSize(0)
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

void ConstantBuffer::Unmap()
{
	mResource->Unmap(0, nullptr);
}

const int ConstantBuffer::GetBufferIncrementSize() const
{
	return mViewDesc.SizeInBytes;
}

const D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPU_Address(const int i) const
{
	return mViewDesc.BufferLocation + (i * mViewDesc.SizeInBytes);
}

const D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::GetGPU_Handle(const int i) const
{
	auto ret = mGPU_Handle;
	ret.ptr += i * mViewIncrementSize;
	return ret;
}

// �e�N�X�`��2D


Texture2D::Texture2D()
	:
	mResource(nullptr),
	mViewDesc({}),
	mGPU_Handle({}),
	mData(nullptr),
	mViewIncrementSize()
{

}

Texture2D::~Texture2D()
{
	System::SafeDelete(&mData);
}


Result Texture2D::LoadFromFile(const wchar_t* const filepath)
{
	System::SafeDelete(&mData);
	mData = new TextureData();
	return mData->LoadFromFile(filepath);
}

Result Texture2D::LoadFromFile(const char* const filepath)
{
	System::SafeDelete(&mData);

	mData = new TextureData();

	TCHAR* wPath = nullptr;
	System::newArray_CreateWideCharStrFromMultiByteStr(&wPath, filepath);

	auto result = mData->LoadFromFile(wPath);
	System::SafeDeleteArray(&wPath);

	return result;
}

const D3D12_GPU_DESCRIPTOR_HANDLE Texture2D::GetGPU_Handle(const int i) const
{
	auto ret = mGPU_Handle;
	ret.ptr += i * mViewIncrementSize;
	return ret;
}

Result Texture2D::WriteToSubresource()
{
	// ���\�[�X�ɏ�������
	auto img = mData->GetImage().GetImage(0, 0, 0);
	ReturnIfFailed
	(
		mResource->WriteToSubresource
		(
			0,
			nullptr,
			img->pixels,
			img->rowPitch,
			img->slicePitch
		),
		Texture2D::WriteToSubresource()
	);

	return SUCCESS;
}

// �f�B�X�N���v�^�q�[�v

DescriptorHeap::DescriptorHeap()
	:
	mDescriptorHeap(nullptr),
	mDescriptorCount(0),
	mViewIncrementSize(0),
	mLastID(0)
{
}

DescriptorHeap::~DescriptorHeap()
{

}

const D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCurrentCPU_Handle()
{
	if (mDescriptorCount <= mLastID)
	{
		assert(false);
	}

	auto ret = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	ret.ptr += mLastID * mViewIncrementSize;
	return ret;
}

const D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCurrentGPU_Handle()
{
	if (mDescriptorCount <= mLastID)
	{
		assert(false);
	}

	auto ret = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	ret.ptr += mLastID * mViewIncrementSize;
	return ret;
}

void DescriptorHeap::MoveToNextHeapPos()
{
	mLastID++;
}

const ComPtr<ID3D12DescriptorHeap> DescriptorHeap::GetDescriptorHeap() const
{
	return mDescriptorHeap;
}

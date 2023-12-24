// �����������Ȃ肻���Ȃ̂ŕ���
#include"GraphicsEngine.h"

// std
#include<vector>
#include<cassert>

// windows
#include"d3dx12.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


// my lib
#include"System.h"
#include"MMDsdk.h"
#include"MathUtil.h"

using namespace GUI::Graphics;
using namespace GUI;

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

GraphicsEngine::GraphicsEngine()
	:
	mParentWidth(0),
	mParentHeight(0),
	mFenceValue(0)
{}
GraphicsEngine::~GraphicsEngine() {}

Result GraphicsEngine::Init(const ParentWindow& parent)
{
	mParentWidth = parent.GetWindowWidth();
	mParentHeight = parent.GetWindowHeight();

	// Direct3D12 �K�g�@���ؗp 
	//DX12�f�o�C�X�쐬
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

	//�R�}���h�L���[�쐬
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

	//�R�}���h���X�g�쐬
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

	//�X���b�v�`�F�[���쐬
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
		swapChainDesc.Width = mParentWidth;
		swapChainDesc.Height = mParentHeight;
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
				mSwapChain.ReleaseAndGetAddressOf()
			),
			GraphicsEngine::Init()
		);
	}

	//�����_�[�^�[�Q�b�g�쐬
	{
		//�����_�[�^�[�Q�b�g�r���[�̃q�[�v�쐬
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

		//�����_�[�^�[�Q�b�g�r���[�쐬
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTV_Heap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < gFrameCount; n++)
		{
			ReturnIfFailed(
				mSwapChain->GetBuffer
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
	//�[�x�o�b�t�@
	{
		//�[�x�o�b�t�@���̂̍쐬
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
		dsResDesc.Width = mParentWidth;
		dsResDesc.Height = mParentHeight;
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

		//�[�x�o�b�t�@�̃r���[�p�̃q�[�v
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

		//�[�x�o�b�t�@�̃r���[
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

	//�t�F���X�쐬
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
	// �ؗp�����܂�

	return Result::SUCCESS;
}

const Color Color::Black = Color(0.f, 0.f, 0.f, 1.f);
const Color Color::White = Color(1.f, 1.f, 1.f, 1.f);
const Color Color::Gray = Color(0.5f, 0.5f, 0.5f, 1.f);

Color::Color(float _r, float _g, float _b, float _a)
	:
	r(_r), g(_g), b(_b), a(_a)
{

}

Color::Color() {}

// �|�C���^�ɂ�鋭���ȃL���X�g
// To��From����������œ����\�������Ă��邱�Ƃ�
// �m���ȏꍇ�ɂ̂ݎg�p�\�@
// ���ł͐�΂Ɏg�p���Ȃ�����
template <class To, class From>
To strong_cast(const From& from)
{
	assert(sizeof(To) == sizeof(From));
	return *reinterpret_cast<const To*>(reinterpret_cast<const void*>(&from));
}

static const int gMaxBoneCount = 4;

struct Vertex
{
public:
	MathUtil::float3 position = {};
	MathUtil::float3 normal = {};
	MathUtil::float2 uv = {};

	int32_t boneID[gMaxBoneCount] = { -1 };
	float weight[gMaxBoneCount] = { 0.f };

	void LoadFromPMD(const MMDsdk::PmdFile::Vertex& other)
	{
		position = strong_cast<MathUtil::float3>(other.position);
		normal = strong_cast<MathUtil::float3>(other.normal);
		uv = strong_cast<MathUtil::float2>(other.uv);

		boneID[0] = other.GetBoneID(0);
		boneID[1] = other.GetBoneID(1);

		weight[0] = static_cast<float>(other.weight) / 100.f;
		weight[1] = 1 - weight[0];
	}

	void LoadFromPMX(const MMDsdk::PmxFile::Vertex& other)
	{
		position = strong_cast<MathUtil::float3>(other.position);
		normal = strong_cast<MathUtil::float3>(other.normal);
		uv = strong_cast<MathUtil::float2>(other.uv);

		for (int i = 0; i < gMaxBoneCount; ++i)
		{
			boneID[i] = other.GetBoneID(i);
			weight[i] = other.GetWeight(i);
		}
	}


};


Model::Model() : mModelName(nullptr) {}
Model::~Model() 
{
	System::SafeDeleteArray(&mModelName);
}

Result Model::Load(const char* const filepath)
{
	if (LoadAsPMD(filepath) == SUCCESS)
	{
		return SUCCESS;
	}
	else if (LoadAsPMX(filepath) == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		DebugMessage("The Filepath \"" << filepath << "\" is not Supported File !");
		return FAIL;
	}
}

void Model::Draw() const
{
	DebugOutString(mModelName);
}

Result Model::LoadAsPMD(const char* const filepath)
{
	MMDsdk::PmdFile file(filepath);

	if (file.IsSuccessLoad() == false)
	{
		return FAIL;
	}

	Reset();

	file.DebugOutHeader();
	auto nameLength = file.GetHeader().modelInfoJP.modelName.GetLength();
	auto name = file.GetHeader().modelInfoJP.modelName.GetText();
	mModelName = new char[nameLength] {'\0'};

	for (int i = 0; i < nameLength; ++i)
	{
		mModelName[i] = name[i];
	}

	std::vector<Vertex> vertices(file.GetVertexCount());

	for (int i = 0; i < vertices.size(); ++i)
	{
		auto& v = vertices[i];

		v.LoadFromPMD(file.GetVertex(i));
	}

	DebugMessage(filepath << " is Loaded !");

	return SUCCESS;
}

Result Model::LoadAsPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);

	if (file.IsSuccessLoad() == false)
	{
		return FAIL;
	}

	Reset();

	file.DebugOutHeader();
	auto nameLength = file.GetHeader().modelInfoJP.modelName.GetLength();
	auto name = file.GetHeader().modelInfoJP.modelName.GetText();
	mModelName = new char[nameLength] { '\0' };

	for (int i = 0; i < nameLength; ++i)
	{
		mModelName[i] = name[i];
	}
	
	std::vector<Vertex> vertices(file.GetVertexCount());
	for (int i = 0; i < vertices.size(); ++i)
	{
		auto& v = vertices[i];
		v.LoadFromPMX(file.GetVertex(i));
	}

	DebugMessage(filepath << " is Loaded !");

	return SUCCESS;
}

void Model::Reset()
{
	// ���f���f�[�^����������
	System::SafeDeleteArray(&mModelName);
}

Scene::Scene()
	:
	mClearColor(),
	mModel(nullptr)
{

}

Scene::~Scene()
{

}

void Scene::SetBackGroundColor
(
	const Color clearColor
)
{
	mClearColor = clearColor;
}

void Scene::PutModel(const Model& model)
{
	mModel = &model;
}

void Scene::Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList, const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle) const
{
	auto clearCol = mClearColor;
	const float clearColor[] = { clearCol.r, clearCol.g, clearCol.b, clearCol.a };
	mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);

	if (mModel != nullptr)
	{
		mModel->Draw();
	}
}

void GraphicsEngine::Draw(const Scene& scene)
{
	// Direct3D12 �K�g����ؗp

	//�o�b�N�o�b�t�@�����݉����ڂ����擾

	UINT backBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	//�R�}���h���X�g�ɏ������ޑO�ɂ̓R�}���h�A���P�[�^�[�����Z�b�g����
	mCommandAllocator->Reset();
	//�R�}���h���X�g�����Z�b�g����
	mCommandList->Reset(mCommandAllocator.Get(), 0);

	//��������R�}���h���X�g�ɃR�}���h����������ł���

	//�o�b�N�o�b�t�@�̃g�����W�V�����������_�[�^�[�Q�b�g���[�h�ɂ���
	auto resBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		mRenderTargets[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	mCommandList->ResourceBarrier
	(
		1,
		&resBarrier
	);

	//�o�b�N�o�b�t�@�������_�[�^�[�Q�b�g�ɃZ�b�g
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle
	(
		mRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
		backBufferIndex,
		mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle
		= mDSV_Heap->GetCPUDescriptorHandleForHeapStart();

	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//�r���[�|�[�g���Z�b�g
	auto viewport = CD3DX12_VIEWPORT
	(
		0.0f, 0.0f,
		(float)mParentWidth,
		(float)mParentHeight
	);
	auto scissorRect = CD3DX12_RECT
	(
		0, 0,
		mParentWidth,
		mParentHeight
	);
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);

	// �V�[���N���X�ֈς˂�
	scene.Draw(mCommandList, rtvHandle);

	//�o�b�N�o�b�t�@�̃g�����W�V������Present���[�h�ɂ���
	resBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		mRenderTargets[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	mCommandList->ResourceBarrier
	(
		1,
		&resBarrier
	);

	//�R�}���h�̏������݂͂����ŏI���AClose����
	mCommandList->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//�o�b�N�o�b�t�@���t�����g�o�b�t�@�ɐ؂�ւ��ăV�[�������j�^�[�ɕ\��
	mSwapChain->Present(1, 0);

	//GPU�T�C�h���S�Ċ��������Ƃ���GPU�T�C�h����Ԃ��Ă���l�i�t�F���X�l�j���Z�b�g
	mCommandQueue->Signal(mFence.Get(), mFenceValue);

	//��ŃZ�b�g�����V�O�i����GPU����A���Ă���܂ŃX�g�[���i���̍s�őҋ@�j
	do
	{
		//GPU�̊�����҂ԁA�����ŉ����L�Ӌ`�Ȏ��iCPU��Ɓj�����قǌ������オ��

	} while (mFence->GetCompletedValue() < mFenceValue);

	//�����Ńt�F���X�l���X�V���� �O����傫�Ȓl�ł���΂ǂ�Ȓl�ł������킯�����A1�����̂��ȒP�Ȃ̂�1�𑫂�
	mFenceValue++;
}


GraphicsEngine& GraphicsEngine::Instance()
{
	static GraphicsEngine engine;
	return engine;
}

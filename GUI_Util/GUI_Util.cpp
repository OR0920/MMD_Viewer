// header
#include "GUI_Util.h"


// std
#include<cassert>

// windows
#include<windows.h>
#include<tchar.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#include"d3dx12.h"
#include<d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#define D3D12Allignment(buffSize) (buffSize + 0xff) & ~0xff;
// my lib
#include "DebugMessage.h"
#include "System.h"

#include"MathUtil.h"
#include"MMDsdk.h"


// ParentWindow
using namespace GUI;

ParentWindow::~ParentWindow()
{

}

// MainWindow

MainWindow& MainWindow::Instance()
{
	static MainWindow inst;
	return inst;
}

// �R�[���o�b�N�֐���
// �w�b�_�ɏ��������Ȃ��̂ŁA�����o�ɂ͂��Ȃ�
BOOL CALLBACK ParentResize(HWND hwnd, LPARAM lparam)
{
	// �q�E�B���h�E�ɃT�C�Y�ύX��ʒm
	// ���Ƃ͎q�ɔC����
	SendMessage(hwnd, WM_SIZE, NULL, NULL);
	return true;
}
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
		//case WM_CREATE:
		//	DragAcceptFiles(hwnd, true);
		//	break;
		//case WM_DROPFILES:
		//	DebugMessage("File Dropped");
		//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		EnumChildWindows(hwnd, ParentResize, lp);
		break;
	}
	default:

		break;
	}


	return DefWindowProc(hwnd, msg, wp, lp);
}


Result MainWindow::Create(int width, int height)
{
	SET_JAPANESE_ENABLE;

	if (mWindowClass.lpszClassName != nullptr)
	{
		DebugMessage("Error at " << ToString(MainWindow::Create()) " : The " << ToString(MainWindow) << " is already Created !");
		return FAIL;
	}

	mWidth = width;
	mHeight = height;

	auto& wc = mWindowClass;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T(ToString(MainWindow));
	wc.hIconSm = NULL;

	if (RegisterClassEx(&wc) == 0)
	{
		DebugMessageFunctionError(RegisterClassEx(), MainWindow::Create());

		auto le = GetLastError();
		wchar_t* messageBuff;
		FormatMessage
		(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			le,
			0,
			(LPWSTR)&messageBuff,
			0,
			NULL
		);

		DebugMessageWide(messageBuff);

		return Result::FAIL;
	}


	mWindowHandle = CreateWindowEx
	(
		NULL,
		wc.lpszClassName,
		_T("MMD Viewer"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (mWindowHandle == NULL)
	{
		DebugMessageFunctionError(CreateWindowEx(), MainWindow::Create());

		return Result::FAIL;
	}


	return Result::SUCCESS;
}

Result MainWindow::ProcessMessage()
{
	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return QUIT;
	}

	return CONTINUE;
}

const int MainWindow::GetWindowWidth() const
{
	return mWidth;
}

const int MainWindow::GetWindowHeight() const
{
	return mHeight;
}


const HWND MainWindow::GetHandle() const
{
	return mWindowHandle;
}

MainWindow::MainWindow()
	:
	mWidth(0),
	mHeight(0),
	mWindowHandle(NULL),
	mWindowClass({})
{

}

MainWindow::~MainWindow()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}

// ErrorBox
void GUI::ErrorBox(const TCHAR* const message)
{
	MessageBox(NULL, message, _T("ERROR"), MB_ICONERROR);
}

// FileCatcher

bool FileCatcher::sIsUpdated = false;
TCHAR FileCatcher::sFilePath[MAX_PATH] = {};
FileCatcher::DropPos FileCatcher::sDropPos = {};

FileCatcher& FileCatcher::Instance()
{
	static FileCatcher inst;
	return inst;
}

// �v���V�[�W���[
LRESULT CALLBACK FileCatcher::FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		DebugMessage("File Catcher Created");
		DragAcceptFiles(hwnd, true);
		break;
	}
	case WM_DROPFILES:
	{
		RECT rect = {};
		GetWindowRect(hwnd, &rect);
		GetCursorPos(reinterpret_cast<LPPOINT>(&sDropPos));
		sDropPos.x -= rect.left;
		sDropPos.y -= rect.top;

		DragQueryFile((HDROP)wp, 0, sFilePath, MAX_PATH);
		DragFinish((HDROP)wp);
		sIsUpdated = true;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		DebugMessage("test");
		break;
	}
	case WM_SIZE:
	{
		auto parentHwnd = GetParent(hwnd);

		RECT parentClientRect{};
		GetClientRect(parentHwnd, &parentClientRect);
		MoveWindow(hwnd, 0, 0, parentClientRect.right, parentClientRect.bottom, true);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}


Result FileCatcher::Create(const ParentWindow& parent)
{
	if (mWindowClass.lpszClassName != nullptr)
	{
		DebugMessage("Error at " << ToString(FileCatcher::Create()) " : The " << ToString(FileCatcher) << " is already Created !");
		return FAIL;
	}
	auto parentHwnd = parent.GetHandle();

	auto& wc = mWindowClass;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = FileCatcherProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T(ToString(FileCatcher));
	wc.hIconSm = NULL;
	wc.hbrBackground = (HBRUSH)0x00000000;

	if (RegisterClassEx(&wc) == 0)
	{
		DebugMessageFunctionError(RegisterClassEx(), FileCatcher::Create());
		return FAIL;
	}

	RECT parentRect = {};
	GetClientRect(parentHwnd, &parentRect);

	auto hwnd = CreateWindowEx
	(
		WS_EX_ACCEPTFILES,
		wc.lpszClassName,
		wc.lpszClassName,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0,
		0,
		parentRect.right - parentRect.left,
		parentRect.bottom - parentRect.top,
		parentHwnd,
		NULL,
		wc.hInstance,
		NULL
	);

	RECT child{};
	GetWindowRect(hwnd, &child);


	if (hwnd == NULL)
	{
		DebugMessageFunctionError(CreateWindowEx(), FileCatcher::Create());
		return FAIL;
	}

	return SUCCESS;
}

bool FileCatcher::Update()
{
	if (sIsUpdated == false)
	{
		return false;
	}

	char* filePath = nullptr;
	System::newArray_CreateMultiByteStrFromWideCharStr(&filePath, sFilePath);

	mFilePath = filePath;


	System::SafeDeleteArray(&filePath);

	sIsUpdated = false;
	return true;
}

int FileCatcher::GetLength() const
{
	return static_cast<int>(mFilePath.size());
}

const char* const FileCatcher::GetPath() const
{
	return mFilePath.c_str();
}

const FileCatcher::DropPos& FileCatcher::GetDropPos() const
{
	return sDropPos;
}


FileCatcher::FileCatcher()
	:
	mWindowClass({}),
	mFilePath()
{

}

FileCatcher::~FileCatcher()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}

// Color

Color::Color()
{

}

Color::Color(float _r, float _g, float _b, float _a)
	:
	r(_r), g(_g), b(_b), a(_a)
{

}

// WinAPI �� HRESULT ���m�F����}�N��
#define ReturnIfFiled(InitFunction, at)\
{\
	auto result = InitFunction;\
	if(FAILED(result))\
	{\
		DebugMessageFunctionError(InitFunction, at);\
		DebugOutParamHex(result);\
		return FAIL;\
	}\
}\

// From : https://learn.microsoft.com/ja-jp/windows/win32/medfound/saferelease
template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

// Model

Model::Model()
	:
	mVB_Resource(nullptr),
	mVB_View({}),
	mIB_Resource(nullptr),
	mIB_View({}),
	mIndexCount(0)
{
	DebugMessage("Model Created !");
}

Model::Model(const Model& other)
	:
	mVB_Resource(other.mVB_Resource),
	mVB_View(other.mVB_View),
	mIB_Resource(other.mIB_Resource),
	mIB_View(other.mIB_View),
	mIndexCount(other.mIndexCount)
{
	DebugMessage("Copy Model Created !");
}

const Model& Model::operator=(const Model& other)
{
	DebugMessage("Model Copyed !");

	mVB_Resource = other.mVB_Resource;
	mVB_View = other.mVB_View;
	mIB_Resource = other.mIB_Resource;
	mIB_View = other.mIB_View;
	mIndexCount = other.mIndexCount;

	return *this;
}

Result Model::Load(const char* const filepath)
{
	if (LoadAsPMD(filepath) == SUCCESS)
	{
		DebugMessage("Load PMD File !");
	}
	else if (LoadAsPMX(filepath) == SUCCESS)
	{
		DebugMessage("Load PMX File !");
	}
	else
	{
		DebugMessage("Not Supported File !");
		return FAIL;
	}

	return SUCCESS;
}

void Model::Reset()
{
	DebugMessage("Model Reseted !");
	SafeRelease(mVB_Resource.GetAddressOf());
	SafeRelease(mIB_Resource.GetAddressOf());
}

void Model::Draw()
{
	if (mVB_Resource == nullptr) return;
	if (mIB_Resource == nullptr) return;
	//DebugMessage("Model Draw !");

	auto commandList = GraphicsDevice::GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &mVB_View);
	commandList->IASetIndexBuffer(&mIB_View);

	commandList->DrawIndexedInstanced(mIndexCount, 1, 0, 0, 0);
}

#include"VertexShader.h"
#include"PixelShader.h"


struct Vertex
{
	MathUtil::float3 position = {};
	MathUtil::float3 normal = {};

	void LoadFromPMD_Vertex(const MMDsdk::PmdFile::Vertex& data)
	{
		position = System::strong_cast<MathUtil::float3>(data.position);
		normal = System::strong_cast<MathUtil::float3>(data.normal);
	}
};

const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

Result Model::LoadAsPMD(const char* const filepath)
{
	MMDsdk::PmdFile file(filepath);

	if (file.IsSuccessLoad() == false)
	{
		return FAIL;
	}

	auto device = GraphicsDevice::GetDevice();

	// ���_�o�b�t�@�쐬
	{
		// ���f�[�^
		std::vector<Vertex> vertex(file.GetVertexCount());

		// �t�@�C������f�[�^�R�s�[
		for (int i = 0; i < file.GetVertexCount(); ++i)
		{
			vertex[i].LoadFromPMD_Vertex(file.GetVertex(i));
		}

		// ���v�̃f�[�^�T�C�Y
		auto vertexBufferSize = sizeof(Vertex) * file.GetVertexCount();

		// �f�[�^�T�C�Y���A�b�v���[�h�p�̃��\�[�X���쐬
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

		ReturnIfFiled
		(
			device->CreateCommittedResource
			(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(mVB_Resource.ReleaseAndGetAddressOf())
			),
			Model::LoadAsPMD()
		);

		// �r���[�̍쐬
		mVB_View.BufferLocation = mVB_Resource->GetGPUVirtualAddress();
		mVB_View.StrideInBytes = sizeof(Vertex);
		mVB_View.SizeInBytes = vertexBufferSize;

		// ���f�[�^����AVRAM��ɃR�s�[
		Vertex* resource = nullptr;
		auto range = CD3DX12_RANGE(0, 0);
		ReturnIfFiled
		(
			mVB_Resource->Map(0, &range, reinterpret_cast<void**>(&resource)),
			Model::LoadAsPMD()
		);
		for (int i = 0; i < vertex.size(); ++i)
		{
			resource[i].position = vertex[i].position;
			resource[i].normal = vertex[i].normal;
		}
		mVB_Resource->Unmap(0, NULL);
	}

	// �C���f�b�N�X�f�[�^
	{
		mIndexCount = file.GetIndexCount();
		std::vector<int> index(file.GetIndexCount());
		for (int i = 0; i < file.GetIndexCount(); ++i)
		{
			index[i] = static_cast<int> (file.GetIndex(i));
		}

		auto indexBufferSize = index.size() * sizeof(int);

		// �f�[�^�T�C�Y���A�b�v���[�h�p�̃��\�[�X���쐬
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

		ReturnIfFiled
		(
			device->CreateCommittedResource
			(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(mIB_Resource.ReleaseAndGetAddressOf())
			),
			Model::LoadAsPMD()
		);

		// �r���[�̍쐬
		mIB_View.BufferLocation = mIB_Resource->GetGPUVirtualAddress();
		mIB_View.Format = DXGI_FORMAT_R32_UINT;
		mIB_View.SizeInBytes = indexBufferSize;

		// ���f�[�^����AVRAM��ɃR�s�[
		int* resource = nullptr;
		// CPU������͌��Ȃ�
		auto range = CD3DX12_RANGE(0, 0);
		ReturnIfFiled
		(
			mIB_Resource->Map(0, &range, reinterpret_cast<void**>(&resource)),
			Model::LoadAsPMD()
		);
		for (int i = 0; i < index.size(); ++i)
		{
			resource[i] = index[i];
		}
		mIB_Resource->Unmap(0, NULL);
	}

	// last
	return SUCCESS;
}

Result Model::LoadAsPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);

	if (file.IsSuccessLoad() == false)
	{
		return FAIL;
	}

	return SUCCESS;
}

// GraphicsDevice
GraphicsDevice* GraphicsDevice::sCanvas = nullptr;

Result GraphicsDevice::Init(const ParentWindow& window, const int frameCount)
{
	if (window.GetHandle() == 0)
	{
		return FAIL;
	}
	if (sCanvas == nullptr)
	{
		sCanvas = new GraphicsDevice(window, frameCount);

		if (sCanvas->InitDirect3D() == FAIL)
		{
			return FAIL;
		}
		if (sCanvas->InitConstantResource() == FAIL)
		{
			return FAIL;
		}


		return SUCCESS;
	}
	else
	{
		DebugMessage("ERROR : The " ToString(GraphicsDevice) " is already Initialized !");
		return FAIL;
	}
}

GraphicsDevice& GraphicsDevice::Instance()
{
	return *sCanvas;
}

void GraphicsDevice::Tern()
{
	if (sCanvas != nullptr)
	{
		delete sCanvas;
		sCanvas = nullptr;
	}
}


void GraphicsDevice::BeginDraw()
{
	mCurrentBufferID = mSwapChain->GetCurrentBackBufferIndex();

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.Get(), 0);

	auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		mRT_Resouces[mCurrentBufferID].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	mCommandList->ResourceBarrier
	(
		1,
		&resourceBarrier
	);

	mRTV_Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE
	(
		mRTV_Heap->GetCPUDescriptorHandleForHeapStart(),
		mCurrentBufferID,
		mDevice->GetDescriptorHandleIncrementSize
		(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV
		)
	);
	mDSV_Handle =
		mDSV_Heap->GetCPUDescriptorHandleForHeapStart();

	mCommandList->OMSetRenderTargets(1, &mRTV_Handle, false, &mDSV_Handle);

	auto viewport = CD3DX12_VIEWPORT
	(
		0.f, 0.f,
		static_cast<float>(mWidth),
		static_cast<float>(mHeight)
	);
	auto scissorRect = CD3DX12_RECT
	(
		0, 0, mWidth, mHeight
	);
	mCommandList->RSSetViewports(1, &viewport);
	mCommandList->RSSetScissorRects(1, &scissorRect);

	mCommandList->SetPipelineState(mPipelineState.Get());

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	mCommandList->SetDescriptorHeaps(1, mCB_Heap.GetAddressOf());

	mCommandList->SetGraphicsRootDescriptorTable(0, mCB_Heap->GetGPUDescriptorHandleForHeapStart());
}

void GraphicsDevice::Clear(const Color& clearColor)
{
	float color[] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
	mCommandList->ClearRenderTargetView(mRTV_Handle, color, 0, NULL);
}

void GraphicsDevice::SetCamera
(
	const MathUtil::float3 eye,
	const MathUtil::float3 target,
	const MathUtil::float3 up
)
{
	mappedCB->view = MathUtil::Matrix::GenerateMatrixLookAtLH(eye, target, up);
}

void GraphicsDevice::EndDraw()
{
	auto resourceBarrier =
		CD3DX12_RESOURCE_BARRIER::Transition
		(
			mRT_Resouces[mCurrentBufferID].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

	mCommandList->ResourceBarrier(1, &resourceBarrier);

	mCommandList->Close();
	ID3D12CommandList* ppCommandList[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	mSwapChain->Present(1, 0);

	mCommandQueue->Signal(mFence.Get(), mFenceValue);
	do
	{
		;
	} while (mFence->GetCompletedValue() < mFenceValue);

	mFenceValue++;
}



GraphicsDevice::GraphicsDevice(const ParentWindow& window, const int frameCount)
	:
	mIsSuccessInit(FAIL),
	mFrameCount(frameCount),
	mWindow(window),
	mWidth(window.GetWindowWidth()),
	mHeight(window.GetWindowHeight()),
	mDevice(nullptr),
	mCommandQueue(nullptr),
	mCommandAllocator(nullptr),
	mCommandList(nullptr),
	mSwapChain(nullptr),
	mRTV_Heap(nullptr),
	mRT_Resouces(),
	mCurrentBufferID(0),
	mRTV_Handle({}),
	mDSV_Heap(nullptr),
	mDSB_Resouce(nullptr),
	mDSV_Handle({}),
	mFence(nullptr),
	mFenceValue(0),
	mCB_Resource(nullptr),
	mCB_Heap(nullptr),
	mappedCB(nullptr),
	mRootSignature(nullptr),
	mPipelineState(nullptr)
{

}

GraphicsDevice::~GraphicsDevice()
{
}


Result GraphicsDevice::InitDirect3D()
{

#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> debug;
		ReturnIfFiled
		(
			D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf())),
			GraphicsDevice::InitDirect3D()
		);
		debug->EnableDebugLayer();
	}
#endif // _DEBUG

	// �f�o�C�X�쐬
	{
		ReturnIfFiled
		(
			D3D12CreateDevice
			(
				nullptr, D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		mDevice->SetName(_T(ToString(GraphicsDevice::mDevice)));
	}

	// �R�}���h�֘A�쐬
	{
		auto commandType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		D3D12_COMMAND_QUEUE_DESC cqd = {};
		cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cqd.Type = commandType;
		ReturnIfFiled
		(
			mDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(mCommandQueue.ReleaseAndGetAddressOf())),
			GraphicsDevice::InitDirect3D()
		);

		ReturnIfFiled
		(
			mDevice->CreateCommandAllocator
			(
				commandType,
				IID_PPV_ARGS(mCommandAllocator.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		ReturnIfFiled
		(
			mDevice->CreateCommandList
			(
				0, commandType, mCommandAllocator.Get(), NULL,
				IID_PPV_ARGS(mCommandList.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);
		mCommandList->Close();
	}


	// �X���b�v�`�F�C���쐬
	{
		ComPtr<IDXGIFactory1> factory1 = nullptr;
		ReturnIfFiled
		(
			CreateDXGIFactory1(IID_PPV_ARGS(factory1.ReleaseAndGetAddressOf())),
			GraphicsDevice::InitDirect3D()
		);

		ComPtr<IDXGIFactory4> factory4 = nullptr;
		ReturnIfFiled
		(
			factory1->QueryInterface(factory4.ReleaseAndGetAddressOf()),
			GraphicsDevice::InitDirect3D()
		);


		DXGI_SWAP_CHAIN_DESC1 scd = {};
		scd.BufferCount = mFrameCount;
		scd.Width = mWidth;
		scd.Height = mHeight;
		scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scd.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> sw1 = nullptr;

		ReturnIfFiled
		(
			factory4->CreateSwapChainForHwnd
			(
				mCommandQueue.Get(),
				mWindow.GetHandle(),
				&scd, NULL, NULL,
				sw1.ReleaseAndGetAddressOf()
			),
			GraphicsDevice::InitDirect3D()
		);

		ReturnIfFiled
		(
			sw1->QueryInterface(IID_PPV_ARGS(mSwapChain.ReleaseAndGetAddressOf())),
			GraphicsDevice::InitDirect3D()
		);
	}

	// �����_�[�^�[�Q�b�g�쐬
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDhd = {};
		rtvDhd.NumDescriptors = mFrameCount;
		rtvDhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ReturnIfFiled
		(
			mDevice->CreateDescriptorHeap
			(
				&rtvDhd, IID_PPV_ARGS(mRTV_Heap.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle
		(
			mRTV_Heap->GetCPUDescriptorHandleForHeapStart()
		);

		mRT_Resouces.assign(mFrameCount, nullptr);
		for (UINT i = 0; i < mFrameCount; ++i)
		{
			ReturnIfFiled
			(
				mSwapChain->GetBuffer
				(
					i,
					IID_PPV_ARGS(mRT_Resouces[i].ReleaseAndGetAddressOf())
				),
				GraphicsDevice::InitDirect3D()
			);

			mDevice->CreateRenderTargetView
			(
				mRT_Resouces[i].Get(),
				NULL, rtvHandle
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

	// �f�v�X�o�b�t�@
	{
		D3D12_HEAP_PROPERTIES dsbHp = {};
		dsbHp.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsbHp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsbHp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsbHp.CreationNodeMask = NULL;
		dsbHp.VisibleNodeMask = NULL;

		D3D12_RESOURCE_DESC dsbRd = {};
		dsbRd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsbRd.Alignment = 0;
		dsbRd.Width = mWidth;
		dsbRd.Height = mHeight;
		dsbRd.DepthOrArraySize = 1;
		dsbRd.MipLevels = 1;
		dsbRd.Format = DXGI_FORMAT_D32_FLOAT;
		dsbRd.SampleDesc = { 1, 0 };
		dsbRd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsbRd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.f;
		clearValue.DepthStencil.Stencil = 0;

		ReturnIfFiled
		(
			mDevice->CreateCommittedResource
			(
				&dsbHp,
				D3D12_HEAP_FLAG_NONE,
				&dsbRd,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(mDSB_Resouce.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHd = {};
		dsvHd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHd.NumDescriptors = 1;
		dsvHd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHd.NodeMask = NULL;

		ReturnIfFiled
		(
			mDevice->CreateDescriptorHeap
			(
				&dsvHd,
				IID_PPV_ARGS(mDSV_Heap.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_D32_FLOAT;
		dsvd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = D3D12_DSV_FLAG_NONE;
		dsvd.Texture2D.MipSlice = 0;
		auto dsvHandle =
			mDSV_Heap->GetCPUDescriptorHandleForHeapStart();

		mDevice->CreateDepthStencilView
		(
			mDSB_Resouce.Get(),
			&dsvd,
			dsvHandle
		);
	}

	{
		ReturnIfFiled
		(
			mDevice->CreateFence
			(
				0,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(mFence.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);

		mFenceValue = 1;
	}

	//�@���[�g�V�O�l�`���쐬
	{
		CD3DX12_DESCRIPTOR_RANGE range[1];
		range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER rootParameter[1];
		rootParameter[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_0
		(
			_countof(rootParameter),
			rootParameter,
			0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> sigunature;
		ComPtr<ID3DBlob> error;

		ReturnIfFiled
		(
			D3DX12SerializeVersionedRootSignature
			(
				&rootSignatureDesc,
				D3D_ROOT_SIGNATURE_VERSION_1,
				sigunature.ReleaseAndGetAddressOf(),
				error.ReleaseAndGetAddressOf()
			),
			GraphicsDevice::InitDirect3D()
		);

		ReturnIfFiled
		(
			mDevice->CreateRootSignature
			(
				0,
				sigunature->GetBufferPointer(),
				sigunature->GetBufferSize(),
				IID_PPV_ARGS(mRootSignature.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);
	}

	// �p�C�v���C���X�e�[�g�쐬
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout.pInputElementDescs = inputLayout;
		psoDesc.InputLayout.NumElements = _countof(inputLayout);
		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(gVertexShader, _countof(gVertexShader) * sizeof(BYTE));
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(gPixelShader, _countof(gPixelShader) * sizeof(BYTE));
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//psoDesc.BlendState.AlphaToCoverageEnable = true;
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		ReturnIfFiled
		(
			mDevice->CreateGraphicsPipelineState
			(
				&psoDesc, IID_PPV_ARGS(mPipelineState.ReleaseAndGetAddressOf())
			),
			GraphicsDevice::InitDirect3D()
		);
	}

	// last

	return SUCCESS;
}

Result GraphicsDevice::InitConstantResource()
{
	DebugOutParam(sizeof(ConstantBuffer));
	auto constantBufferSize = D3D12Allignment(sizeof(ConstantBuffer));
	DebugOutParam(constantBufferSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

	ReturnIfFiled
	(
		mDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mCB_Resource.ReleaseAndGetAddressOf())
		),
		GraphicsDevice::InitConstantResource()
	);

	D3D12_DESCRIPTOR_HEAP_DESC cbHeapDesc = {};
	cbHeapDesc.NumDescriptors = 1;
	cbHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ReturnIfFiled
	(
		mDevice->CreateDescriptorHeap
		(
			&cbHeapDesc,
			IID_PPV_ARGS(mCB_Heap.ReleaseAndGetAddressOf())
		),
		GraphicsDevice::InitConstantResource()
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbViewDesc = {};
	cbViewDesc.BufferLocation = mCB_Resource->GetGPUVirtualAddress();
	cbViewDesc.SizeInBytes = constantBufferSize;
	mDevice->CreateConstantBufferView(&cbViewDesc, mCB_Heap->GetCPUDescriptorHandleForHeapStart());

	auto range = CD3DX12_RANGE(0, 0);
	ReturnIfFiled
	(
		mCB_Resource->Map
		(
			0, &range, reinterpret_cast<void**>(&mappedCB)
		),
		GraphicsDevice::InitConstantResource()
	);

	mappedCB->world = MathUtil::Matrix::GenerateMatrixIdentity();

	mappedCB->projection = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
	(
		MathUtil::PI_DIV4,
		static_cast<float>(mWidth) / static_cast<float>(mHeight),
		0.1f,
		1000.f
	);

	return SUCCESS;
}

ComPtr<ID3D12Device>& GraphicsDevice::GetDevice()
{
	return Instance().mDevice;
}

ComPtr<ID3D12GraphicsCommandList>& GraphicsDevice::GetCommandList()
{
	return Instance().mCommandList;
}

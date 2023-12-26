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

// my lib
#include "DebugMessage.h"
#include "System.h"

#include"MathUtil.h"


// ParentWindow
using namespace GUI;

ParentWindow::~ParentWindow()
{

}

//// MainWindow
//static HWND gMainWindowHandle = NULL;
//static WNDCLASSEX gMainWindowClass = {};

BOOL CALLBACK ParentResize(HWND hwnd, LPARAM lparam)
{
	// 子ウィンドウにサイズ変更を通知
	// あとは子に任せる
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

bool MainWindow::IsClose()
{
	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		isClose = true;
	}

	return isClose;
}

const HWND MainWindow::GetHandle() const
{
	return mWindowHandle;
}

const int MainWindow::GetWindowWidth() const
{
	return mWidth;
}

const int MainWindow::GetWindowHeight() const
{
	return mHeight;
}

MainWindow& MainWindow::Instance()
{
	static MainWindow inst;
	return inst;
}

MainWindow::MainWindow()
	:
	mWindowHandle(NULL),
	mWindowClass({}),
	isClose(false),
	mWidth(0),
	mHeight(0)
{

}

MainWindow::~MainWindow()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}


// FileCatcher

bool FileCatcher::sIsUpdated = false;
TCHAR FileCatcher::sFilePath[MAX_PATH] = {};
FileCatcher::DropPos FileCatcher::sDropPos = {};

// プロシージャー
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

FileCatcher& FileCatcher::Instance()
{
	static FileCatcher inst;
	return inst;
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


// Canvas
ComPtr<ID3D12Device> Canvas::sDevice = nullptr;
ComPtr<ID3D12CommandQueue> Canvas::sCommandQueue = nullptr;

Canvas::Canvas(const ParentWindow& parent, const int frameCount)
	:
	mIsSuccessInit(FAIL),
	mFrameCount(frameCount),
	mWindow(parent),
	mWidth(parent.GetWindowWidth()),
	mHeight(parent.GetWindowHeight()),
	mCommandAllocator(nullptr),
	mCommandList(nullptr),
	mSwapChain(nullptr),
	mRTV_Heap(nullptr),
	mRT_Resouces(),
	mDSV_Heap(nullptr),
	mDSB_Resouce(nullptr),
	mFence(nullptr),
	mFenceValue(0)
{
	if (parent.GetHandle() == 0)
	{
		mIsSuccessInit = FAIL;

		return;
	}

	mIsSuccessInit = InitDirect3D();
}

Result Canvas::IsSuccessInit() const
{
	return mIsSuccessInit;
}

void Canvas::BeginDraw()
{

}

void Canvas::Clear(const Color& clearColor)
{
	DebugOutFloat4(System::strong_cast<MathUtil::float4>(clearColor));
}

void Canvas::EndDraw()
{

}

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

Result Canvas::InitDirect3D()
{

#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> debug;
		ReturnIfFiled
		(
			D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf())),
			Canvas::InitDirect3D()
		);
		debug->EnableDebugLayer();
	}
#endif // _DEBUG

	// デバイス作成
	{
		if (sDevice == nullptr)
		{
			ReturnIfFiled
			(
				D3D12CreateDevice
				(
					nullptr, D3D_FEATURE_LEVEL_12_0,
					IID_PPV_ARGS(sDevice.ReleaseAndGetAddressOf())
				),
				Canvas::InitDirect3D()
			);
		}
	}

	// コマンド関連作成
	{
		auto commandType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (sCommandQueue == nullptr)
		{
			D3D12_COMMAND_QUEUE_DESC cqd = {};
			cqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cqd.Type = commandType;
			ReturnIfFiled
			(
				sDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(sCommandQueue.ReleaseAndGetAddressOf())),
				Canvas::InitDirect3D()
			);
		}

		ReturnIfFiled
		(
			sDevice->CreateCommandAllocator
			(
				commandType,
				IID_PPV_ARGS(mCommandAllocator.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
		);

		ReturnIfFiled
		(
			sDevice->CreateCommandList
			(
				0, commandType, mCommandAllocator.Get(), NULL,
				IID_PPV_ARGS(mCommandList.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
		);
		mCommandList->Close();
	}


	// スワップチェイン作成
	{
		ComPtr<IDXGIFactory1> factory1 = nullptr;
		ReturnIfFiled
		(
			CreateDXGIFactory1(IID_PPV_ARGS(factory1.ReleaseAndGetAddressOf())),
			Canvas::InitDirect3D()
		);

		ComPtr<IDXGIFactory4> factory4 = nullptr;
		ReturnIfFiled
		(
			factory1->QueryInterface(factory4.ReleaseAndGetAddressOf()),
			Canvas::InitDirect3D()
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
				sCommandQueue.Get(),
				mWindow.GetHandle(),
				&scd, NULL, NULL,
				sw1.ReleaseAndGetAddressOf()
			),
			Canvas::InitDirect3D()
		);

		ReturnIfFiled
		(
			sw1->QueryInterface(IID_PPV_ARGS(mSwapChain.ReleaseAndGetAddressOf())),
			Canvas::InitDirect3D()
		);
	}

	// レンダーターゲット作成
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDhd = {};
		rtvDhd.NumDescriptors = mFrameCount;
		rtvDhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ReturnIfFiled
		(
			sDevice->CreateDescriptorHeap
			(
				&rtvDhd, IID_PPV_ARGS(mRTV_Heap.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
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
				Canvas::InitDirect3D()
			);

			sDevice->CreateRenderTargetView
			(
				mRT_Resouces[i].Get(),
				NULL, rtvHandle
			);

			rtvHandle.Offset
			(
				1,
				sDevice->GetDescriptorHandleIncrementSize
				(
					D3D12_DESCRIPTOR_HEAP_TYPE_RTV
				)
			);
		}
	}

	// デプスバッファ
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
			sDevice->CreateCommittedResource
			(
				&dsbHp,
				D3D12_HEAP_FLAG_NONE,
				&dsbRd,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(mDSB_Resouce.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
		);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHd = {};
		dsvHd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHd.NumDescriptors = 1;
		dsvHd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHd.NodeMask = NULL;

		ReturnIfFiled
		(
			sDevice->CreateDescriptorHeap
			(
				&dsvHd,
				IID_PPV_ARGS(mDSV_Heap.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_D32_FLOAT;
		dsvd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = D3D12_DSV_FLAG_NONE;
		dsvd.Texture2D.MipSlice = 0;
		auto dsvHandle =
			mDSV_Heap->GetCPUDescriptorHandleForHeapStart();

		sDevice->CreateDepthStencilView
		(
			mDSB_Resouce.Get(),
			&dsvd,
			dsvHandle
		);
	}

	{
		ReturnIfFiled
		(
			sDevice->CreateFence
			(
				0,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(mFence.ReleaseAndGetAddressOf())
			),
			Canvas::InitDirect3D()
		);

		mFenceValue = 1;
	}

	// last

	return SUCCESS;
}
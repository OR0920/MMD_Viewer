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
Canvas::Canvas(const ParentWindow& parent)
	:
	mIsSuccessInit(FAIL)
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

	{

	}

	return SUCCESS;
}
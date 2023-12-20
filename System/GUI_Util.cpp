// header
//#include "GUI_Util.h"
#include "Error.h"
#include "DebugMessage.h"
#include "System.h"


// std

// windows
#include<windows.h>
#include<tchar.h>

// my lib

using namespace System;
using namespace System::GUI;

// Window

ParentWindow::~ParentWindow()
{

}

//// MainWindow
//static HWND gMainWindowHandle = NULL;
//static WNDCLASSEX gMainWindowClass = {};

BOOL CALLBACK ParentResize(HWND hwnd, LPARAM lparam)
{
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

	if (mWindowHandle== NULL)
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

MainWindow& MainWindow::Instance()
{
	static MainWindow inst;
	return inst;
}

MainWindow::MainWindow()
	:
	mWindowHandle(NULL),
	mWindowClass({}),
	isClose(false)
{

}

MainWindow::~MainWindow()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}


// FileCatcher

bool FileCatcher::sIsUpdated = false;
TCHAR FileCatcher::sFilePath[MAX_PATH] = {};

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
{

}

FileCatcher::~FileCatcher() {}

Result FileCatcher::Create(const ParentWindow& parent)
{
	auto parentHwnd = parent.GetHandle();

	WNDCLASSEX wc = {};
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
	newArray_CreateMultiByteStrFromWideCharStr(&filePath, sFilePath);

	mFilePath = filePath;
	

	SafeDeleteArray(&filePath);

	sIsUpdated = false;
	return true;
}

int FileCatcher::GetLength() const
{
	return mFilePath.size();
}

const char* const FileCatcher::GetPath() const
{
	return mFilePath.c_str();
}

FileCatcher& FileCatcher::Instance()
{
	static FileCatcher inst;
	return inst;
}

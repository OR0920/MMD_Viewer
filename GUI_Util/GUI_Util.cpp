// header
#include "GUI_Util.h"

// std
#include<cassert>

// windows
#include<windows.h>
#include<tchar.h>

// my lib
#include "System.h"


// エラー出力
void OutputLastError()
{
	auto le = GetLastError();
	wchar_t* messageBuff = nullptr;
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
}


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

// コールバック関数類
// ヘッダに書きたくないので、メンバにはしない
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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_BACKGROUND));
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
		OutputLastError();
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

	RECT cr = {};
	GetClientRect(mWindowHandle, &cr);

	mClientWidth = cr.right - cr.left;
	mClientHeight = cr.bottom - cr.top;

	return Result::SUCCESS;
}

Result MainWindow::ProsessMessage()
{
	MSG msg = {};

	if (GetMessage(&msg, NULL, 0, 0))
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

Result MainWindow::ProcessMessageNoWait()
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

const int MainWindow::GetClientWidth() const
{
	return mClientWidth;
}

const int MainWindow::GetClientHeight() const
{
	return mClientHeight;
}

const HWND MainWindow::GetHandle() const
{
	return mWindowHandle;
}

MainWindow::MainWindow()
	:
	mWidth(0),
	mHeight(0),
	mClientWidth(0),
	mClientHeight(0),
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


Result FileCatcher::Create(const ParentWindow& parent)
{
	// 2重で作らせない
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
		OutputLastError();
		return FAIL;
	}

	auto hwnd = CreateWindowEx
	(
		WS_EX_ACCEPTFILES,
		wc.lpszClassName,
		wc.lpszClassName,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0,
		0,
		parent.GetClientWidth(),
		parent.GetClientHeight(),
		parentHwnd,
		NULL,
		wc.hInstance,
		NULL
	);


	if (hwnd == NULL)
	{
		DebugMessageFunctionError(CreateWindowEx(), FileCatcher::Create());
		return FAIL;
	}

	return SUCCESS;
}

bool FileCatcher::Update()
{
	// プロシージャーから更新されているかをもらう
	if (sIsUpdated == false)
	{
		return false;
	}

	// ワイド文字からマルチバイトへ変換
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



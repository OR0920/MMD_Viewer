#include<Windows.h>
#include<tchar.h>

#include"ParentWindow.h"
#include"Result.h"
#include"MainWindow.h"
#include"OutputLastError.h"

#include"System.h"

using namespace GUI_Util;

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


Result MainWindow::Create(int width, int height, const LPCTSTR windowTitle)
{
	SET_JAPANESE_ENABLE;

	// ウィンドウクラスの名前が登録されていたら、ウィンドウ作成済み
	if (mWindowClass.lpszClassName != nullptr)
	{
		DebugMessage("Error at " << ToString(MainWindow::Create()) " : The " << ToString(MainWindow) << " is already Created !");
		return FAIL;
	}

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
		windowTitle,
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

#define WINDOW_IS_CREATED \
if(mWindowHandle == NULL)\
{\
	DebugMessageError("This Window is not created ! Call " << ToString(MainWindow::Create()) << " to Create Window");\
}

Result MainWindow::ProsessMessage()
{
	WINDOW_IS_CREATED;

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
	WINDOW_IS_CREATED;
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
	RECT wr = {};
	GetWindowRect(mWindowHandle, &wr);

	return wr.right - wr.left;
}

const int MainWindow::GetWindowHeight() const
{
	RECT wr = {};
	GetWindowRect(mWindowHandle, &wr);

	return wr.bottom - wr.right;
}

const int MainWindow::GetClientWidth() const
{
	RECT cr = {};
	GetClientRect(mWindowHandle, &cr);

	return cr.right - cr.left;
}

const int MainWindow::GetClientHeight() const
{
	RECT cr = {};
	GetClientRect(mWindowHandle, &cr);

	return cr.bottom - cr.top;
}

const HWND MainWindow::GetHandle() const
{
	return mWindowHandle;
}

MainWindow::MainWindow()
	:
	mWindowHandle(NULL),
	mWindowClass({})
{

}

MainWindow::~MainWindow()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}
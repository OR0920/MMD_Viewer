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

	return Result::SUCCESS;
}

#define WINDOW_IS_CREATED \
if(mWindowHandle == NULL)\
{\
	DebugMessageError("This Window is not created ! Call " << ToString(MainWindow::Create()) << " to Create Window")\
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

// ErrorBox
void GUI::ErrorBox(const TCHAR* const message)
{
	MessageBox(NULL, message, _T("ERROR"), MB_ICONERROR);
}

// FileCatcher

// プロシージャー
LRESULT CALLBACK FileCatcher::FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ウィンドウに紐づいているFileCatcherのインスタンスをもらう
	FileCatcher* fc = reinterpret_cast<FileCatcher*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_CREATE:
	{
		DebugMessage("File Catcher Created");

		// CreateWindow関数の末尾に渡したパラメータを取得する
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lp);
		// FileCatcherのインスタンスへを、ウィンドウに紐づける
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		DragAcceptFiles(hwnd, true);
		break;
	}
	case WM_DROPFILES:
	{
		RECT rect = {};
		GetWindowRect(hwnd, &rect);
		GetCursorPos(reinterpret_cast<LPPOINT>(&fc->mDropPos));
		// ファイルのドロップ場所はディスプレイ全体からの座標になってしまうため、
		// ウィンドウの原点基準に修正
		fc->mDropPos.x -= rect.left;
		fc->mDropPos.y -= rect.top;

		// ドロップされたパスを取得
		DragQueryFile((HDROP)wp, 0, fc->mWideFilePath, MAX_PATH);
		DragFinish((HDROP)wp);
		
		// 更新を伝える
		fc->mIsUpdated = true;

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
	auto& wc = mWindowClass;
	auto parentHwnd = parent.GetHandle();

	// ウィンドウクラス構造体は一つだけ　とりあえず名前で判定
	if (wc.lpszClassName == nullptr)
	{
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
		this// 自身へのポインタを渡し、プロシージャに書き換えてもらう
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
	// 更新されていればプロシージャーがtrueを入れているはず
	if (mIsUpdated == false)
	{
		return false;
	}

	// ワイド文字からマルチバイトへ変換　固定長の変換を作っていないので、可変長で
	// 余裕があれば固定長の変換を実装する。
	char* filePath = nullptr;
	System::newArray_CreateMultiByteStrFromWideCharStr(&filePath, mWideFilePath);

	mFilePath = filePath;

	System::SafeDeleteArray(&filePath);

	mIsUpdated = false;
	return true;
}

int FileCatcher::GetLength() const
{
	return static_cast<int>(mFilePath.size());
}

int FileCatcher::GetWideLength() const
{
	return System::GetStringLength(mWideFilePath);
}

const char* const FileCatcher::GetPath() const
{
	return mFilePath.c_str();
}

const wchar_t* const FileCatcher::GetWidePath() const
{
	return mWideFilePath;
}

const FileCatcher::DropPos& FileCatcher::GetDropPos() const
{
	return mDropPos;
}

WNDCLASSEX FileCatcher::mWindowClass = {};

FileCatcher::FileCatcher()
	:
	mIsUpdated(false),
	mFilePath(),
	mWideFilePath(),
	mDropPos({})
{

}

FileCatcher::~FileCatcher()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}



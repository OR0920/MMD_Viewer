// ヘッダーに必要なモジュール
#include<Windows.h>
#include<string>

#include"Result.h"
#include"ParentWindow.h"

// ヘッダー
#include"FileCatcher.h"

// 他モジュール
#include"private/OutputLastError.h"

// 外部モジュール
#include<tchar.h>
#include"System.h"

using namespace GUI_Util;

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
		// FileCatcherのインスタンスを、ウィンドウに紐づける
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		DragAcceptFiles(hwnd, true);
		break;
	}
	// ファイルがドロップされたらここに来る
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
	// サイズが変更されたらここに来る。　親ウィンドウが教えてくれる
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




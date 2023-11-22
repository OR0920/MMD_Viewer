// window
#include<Windows.h>
#include<tchar.h>

// window size
static const int gWindowWidth = 1280;
static const int gWindowHeight = 720;


// debug memory leak
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>

// é©çÏÉâÉCÉuÉâÉä
#include"System.h"
#include"MathUtil.h"
#include"MMDsdk.h"


LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);


#ifdef _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WNDCLASSEX wcEx = {};

	wcEx.cbSize = sizeof(WNDCLASSEX);
	wcEx.lpfnWndProc = WinProc;
	wcEx.lpszClassName = _T("dx12sample");
	wcEx.hInstance = GetModuleHandle(NULL);

	RegisterClassEx(&wcEx);

	RECT rect = { 0, 0, gWindowWidth, gWindowHeight };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd =
		CreateWindow
		(
			wcEx.lpszClassName,
			_T("dx12test"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			wcEx.hInstance,
			nullptr
		);

	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	UnregisterClass(wcEx.lpszClassName, wcEx.hInstance);


	_CrtDumpMemoryLeaks();
	return 0;
}

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


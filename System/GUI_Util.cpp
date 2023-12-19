// header
#include "GUI_Util.h"
#include "Error.h"
#include "DebugMessage.h"


// std

// windows
#include<windows.h>
#include<tchar.h>

// my lib

using namespace System;

// MainWindow
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

MainWindow& MainWindow::Instance()
{
	static MainWindow inst;
	return inst;
}

static HWND MainWindowHandle = NULL;

Result MainWindow::Create(int width, int height)
{
	SET_JAPANESE_ENABLE;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = MainWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = _T(ToString(MainWindow));
	wc.style = CS_HREDRAW | CS_VREDRAW;

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

	MainWindowHandle = CreateWindowEx
	(
		NULL,
		wc.lpszClassName,
		_T("MMD Viewer"),
		WS_OVERLAPPEDWINDOW | WS_TABSTOP | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (MainWindowHandle == NULL)
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

MainWindow::MainWindow()
	:
	isClose(false)
{

}

MainWindow::~MainWindow()
{

}

// FileCatcherWindow
LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		DragAcceptFiles(hwnd, true);
		break;
	}
	case WM_DROPFILES:
	{
		break;
	}
	default:
		break;
	}


	return DefWindowProc(hwnd, msg, wp, lp);
}

FileCatcherWindow::FileCatcherWindow()
{

}

FileCatcherWindow::~FileCatcherWindow()
{

}

Result FileCatcherWindow::Create()
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = FileCatcherProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = _T(ToString(FileCatcherWindow));
	

	return SUCCESS;
}
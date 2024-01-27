#include"Application.h"
#include"Dx12Wrapper.h"
#include"PMDRenderer.h"
#include"PMDActor.h"

#include<tchar.h>

#include"System.h"
#include"MathUtil.h"


Application& Application::Instance()
{
	static Application instance;
	return instance;
}


Application::Application()
	:
	mWindowWidth(0),
	mWindowHeight(0),
	mWindowClass({}),
	mWindowHandle(NULL),
	mDx12(nullptr),
	mPmdRenderer(nullptr),
	mPmdActor(nullptr)
{

}


Application::~Application()
{

}


bool Application::Init
(
	const unsigned int argWindowWidth,
	const unsigned int argWindowHeight,
	const char* const modelPath
)
{
	mWindowWidth = argWindowWidth;
	mWindowHeight = argWindowHeight;

	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);

	CreateGameWindow();

	mDx12.reset(new Dx12Wrapper(mWindowHandle));
	mPmdRenderer.reset(new PMDRenderer(*mDx12));
	mPmdActor.reset(new PMDActor(modelPath, *mPmdRenderer));

	return true;
}

double mFrameTime = 0;

LRESULT CALLBACK Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	if (msg == WM_KEYDOWN)
	{
		static const MathUtil::Vector x(1.f, 0.f, 0.f);
		static const MathUtil::Vector y(0.f, 1.f, 0.f);
		static const MathUtil::Vector z(0.f, 0.f, 1.f);

		MathUtil::Vector move;

		float speed = 0.5f;

		auto& inst = Application::Instance().mDx12;
		switch (wparam)
		{
		case 'W': move = z * speed; break;
		case 'S': move = z * -speed; break;
		case 'A': move = x * -speed; break;
		case 'D': move = x * speed; break;
		case 'Q': move = y * speed; break;
		case 'E': move = y * -speed; break;
		case 'R': inst->ResetCamera(); break;
		default:
			break;
		}
		inst->MoveCamera(move);
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}

void Application::CreateGameWindow()
{
	HINSTANCE appInctanceHandle = GetModuleHandle(NULL);
	mWindowClass.cbSize = sizeof(WNDCLASSEX);
	mWindowClass.lpfnWndProc = WindowProcedure;
	mWindowClass.lpszClassName = _T("DirectXTest");
	mWindowClass.hInstance = appInctanceHandle;
	auto result = RegisterClassEx(&mWindowClass);
	if (result == 0)
	{
		DebugMessageFunctionError(RegisterClassEx, CreateGameWindow);
		return;
	}

	RECT wrc{ 0, 0, mWindowWidth, mWindowHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	mWindowHandle =
		CreateWindow
		(
			mWindowClass.lpszClassName,
			_T("DX12リファクタリング"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wrc.right - wrc.left,
			wrc.bottom - wrc.top,
			nullptr,
			nullptr,
			mWindowClass.hInstance,
			nullptr
		);

	if (mWindowHandle == NULL)
	{
		DebugMessageFunctionError(CreateWindow, CreateGameWindow);
		return;
	}
}

SIZE Application::GetWindowSize() const
{
	SIZE ret = {};
	ret.cx = mWindowWidth;
	ret.cy = mWindowHeight;
	return ret;
}


void Application::Run()
{
	ShowWindow(mWindowHandle, SW_SHOW);
	float angle = 0.f;
	MSG msg = {};

	unsigned int frame = 0;

	LARGE_INTEGER mTimeFreq = {}, mTimeStart = {}, mTimeEnd = {};
	QueryPerformanceFrequency(&mTimeFreq);
	while (true)
	{
		QueryPerformanceCounter(&mTimeStart);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		mDx12->BeginDraw();

		mDx12->GetCommandList()->SetPipelineState(mPmdRenderer->GetPipelineState().Get());
		mDx12->GetCommandList()->SetGraphicsRootSignature(mPmdRenderer->GetRootSignature().Get());

		mDx12->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mDx12->SetScene();

		mPmdActor->Update();
		mPmdActor->Draw();

		mDx12->EndDraw();

		mDx12->GetSwapChain()->Present(1, 0);

		QueryPerformanceCounter(&mTimeEnd);
		mFrameTime = static_cast<double>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<double>(mTimeFreq.QuadPart);
	}

}

void
Application::Term()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}


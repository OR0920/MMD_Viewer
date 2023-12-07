#include"Application.h"
#include"Dx12Wrapper.h"
#include"PMDRenderer.h"
#include"PMDActor.h"

#include<tchar.h>

#include"System.h"

static const char* const kabanPath = "../x64/Debug/Test/Model/PMX/かばんちゃん/かばんちゃん/かばんちゃん.pmx";
static const char* const hashibiroPath = "../x64/debug/PMX/ハシビロコウ/ハシビロコウ.pmx";
static const char* const stagePath = "../x64/debug/Test/Model/PMX/キョウシュウエリアver1.0/キョウシュウエリア/キョウシュウエリア20170914.pmx";
static const char* const mikuPath = "../x64/Debug/Test/Model/PMD/初音ミク.pmd";
static const char* const miku2Path = "../x64/Debug/Test/Model/PMD/初音ミクVer2.pmd";
static const char* const metalMikuPath = "../x64/Debug/Test/Model/PMD/初音ミクmetal.pmd";
static const char* const meikoPath = "../x64/Debug/Test/Model/PMD/MEIKO.pmd";
static const char* const kaitoPath = "../x64/Debug/Test/Model/PMD/カイト.pmd";
static const char* const rinPath = "../x64/Debug/Test/Model/PMD/鏡音リン.pmd";
static const char* const rukaPath = "../x64/Debug/Test/Model/PMD/巡音ルカ.pmd";
static const char* const hachunePath = "D:/Projects/MMD_Viewer/x64/Debug/Test/Model/PMD/PMDモデル はちゅねミク_hatsune_hachi_202312021756/PMD_hachune_1.41/hachune.pmd";


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
	const unsigned int argWindowHeight
)
{
	mWindowWidth = argWindowWidth;
	mWindowHeight = argWindowHeight;

	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);

	CreateGameWindow();

	mDx12.reset(new Dx12Wrapper(mWindowHandle));
	mPmdRenderer.reset(new PMDRenderer(*mDx12));
	mPmdActor.reset(new PMDActor(mikuPath, *mPmdRenderer));

	return true;
}

LRESULT CALLBACK Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	if (msg == WM_KEYDOWN)
	{
		DirectX::XMFLOAT3 x = { 1.f, 0.f, 0.f }; DirectX::XMVECTOR xv = DirectX::XMLoadFloat3(&x);
		DirectX::XMFLOAT3 y = { 0.f, 1.f, 0.f }; DirectX::XMVECTOR yv = DirectX::XMLoadFloat3(&y);
		DirectX::XMFLOAT3 z = { 0.f, 0.f, 1.f }; DirectX::XMVECTOR zv = DirectX::XMLoadFloat3(&z);
		float speed = 0.5f;
		DirectX::XMFLOAT3 speedf = { speed, speed, speed };
		DirectX::XMFLOAT3 ispeedf = { -speed, -speed, -speed };

		DirectX::XMVECTOR speedv = DirectX::XMLoadFloat3(&speedf);
		DirectX::XMVECTOR ispeedv = DirectX::XMLoadFloat3(&ispeedf);


		auto& inst = Application::Instance().mDx12;
		switch (wparam)
		{
		case 'W': inst->MoveCamera(DirectX::XMVectorMultiply(zv, speedv)); break;
		case 'S': inst->MoveCamera(DirectX::XMVectorMultiply(zv, ispeedv)); break;
		case 'A': inst->MoveCamera(DirectX::XMVectorMultiply(xv, ispeedv)); break;
		case 'D': inst->MoveCamera(DirectX::XMVectorMultiply(xv, speedv)); break;
		case 'Q': inst->MoveCamera(DirectX::XMVectorMultiply(yv, speedv)); break;
		case 'E': inst->MoveCamera(DirectX::XMVectorMultiply(yv, ispeedv)); break;
		case 'R': inst->ResetCamera();
		default:
			break;
		}
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
		DebugMessageFunctionError(RegisterClassEx, CreateGameWindow)
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

	while (true)
	{
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
	}
}

void
Application::Term()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}


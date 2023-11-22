// window
#include<Windows.h>
#include<tchar.h>

template<class C>
void SafeRelease(C** ptr)
{
	if (*ptr != nullptr)
	{
		(*ptr)->Release();
		*ptr = nullptr;
	}
}


// window size
static const int gWindowWidth = 1280;
static const int gWindowHeight = 720;

// direct3d lib
#include<d3d12.h>
#include<dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

ID3D12Device* gDevice = nullptr;
IDXGIFactory6* gDxgiFactory = nullptr;
IDXGISwapChain4* gSwapChain = nullptr;
ID3D12CommandAllocator* gCmdAllocator = nullptr;
ID3D12GraphicsCommandList* gCmdList = nullptr;
ID3D12CommandQueue* gCmdQueue = nullptr;


void SafeReleaseAll_D3D_Interface()
{
	SafeRelease(&gCmdQueue);
	SafeRelease(&gCmdList);
	SafeRelease(&gCmdAllocator);

	SafeRelease(&gSwapChain);
	SafeRelease(&gDxgiFactory);
	SafeRelease(&gDevice);
}

// debug memory leak
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>

// ���색�C�u����
#include"System.h"
#include"MathUtil.h"
#include"MMDsdk.h"


int ReturnWithErrorMessage(const char* const message)
{
	DebugMessage(message);
	SafeReleaseAll_D3D_Interface();
	return -1;
}

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#ifdef _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG
{
	// ���������[�N�`�F�b�N
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	// �E�B���h�E�N���X�쐬�E�o�^
	WNDCLASSEX wcEx = {};
	wcEx.cbSize = sizeof(WNDCLASSEX);
	wcEx.lpfnWndProc = WinProc;
	wcEx.lpszClassName = _T("dx12sample");
	wcEx.hInstance = GetModuleHandle(NULL);
	RegisterClassEx(&wcEx);


	// �E�B���h�E�T�C�Y���� 
	RECT rect = { 0, 0, gWindowWidth, gWindowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);


	// �E�B���h�E�쐬�E�`��
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


	// Direct3D ������
		// �f�o�C�X�̍쐬
	{
		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D_FEATURE_LEVEL featureLevel;

		for (auto lv : levels)
		{
			if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&gDevice)) == S_OK)
			{
				featureLevel = lv;
				break;
			}
		}
	}

	if (gDevice == nullptr)
	{
		return ReturnWithErrorMessage("Failed Create D3D Device !");
	}

	// �R�}���h���X�g�A�R�}���h�A���P�[�^�̍쐬
	auto result =
		gDevice->CreateCommandAllocator
		(
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&gCmdAllocator)
		);
	if (result != S_OK)
	{
		return ReturnWithErrorMessage("Failed To Create Command Allocator !");
	}
	result =
		gDevice->CreateCommandList
		(
			0,
			D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
			gCmdAllocator,
			nullptr,
			IID_PPV_ARGS(&gCmdList)
		);
	if (result != S_OK)
	{
		return ReturnWithErrorMessage("Failed To Create Command List !");
	}

	// �R�}���h�L���[�̍쐬
	{
		D3D12_COMMAND_QUEUE_DESC cqd = {};
		cqd.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
		cqd.NodeMask = 0;
		cqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cqd.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

		result = gDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(&gCmdQueue));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed To Create Command Queue !");
		}
	}



	// ���b�Z�[�W���[�v
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// �v���O�����I���̃��b�Z�[�W���o���甲����
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}


	// �I������
	UnregisterClass(wcEx.lpszClassName, wcEx.hInstance);

	SafeReleaseAll_D3D_Interface();

	// ���������[�N�`�F�b�N
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


// window
#include<Windows.h>
#include<tchar.h>

#include<wrl.h>

template <class C>
using ComPtr = Microsoft::WRL::ComPtr<C>;

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

static const int gBufferCount = 2;

// std lib
#include<vector>

// direct3d lib
#include<d3d12.h>
#include<dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

ComPtr<ID3D12Device> gDevice = nullptr;
ComPtr<IDXGIFactory6> gDxgiFactory = nullptr;
ComPtr<IDXGISwapChain4> gSwapChain = nullptr;
ComPtr<ID3D12CommandAllocator> gCmdAllocator = nullptr;
ComPtr<ID3D12GraphicsCommandList> gCmdList = nullptr;
ComPtr<ID3D12CommandQueue> gCmdQueue = nullptr;
ComPtr<ID3D12DescriptorHeap> gRtvHeaps = nullptr;
std::vector<ComPtr<ID3D12Resource>> gBackBuffers(gBufferCount);

ComPtr<ID3D12Fence> gFence = nullptr;
UINT64 gFenceVal = 0;

void SafeReleaseAll_D3D_Interface()
{
	SafeRelease(gFence.GetAddressOf());
	for (int i = 0; i < gBufferCount; ++i)
	{
		SafeRelease(gBackBuffers[i].GetAddressOf());
	}
	SafeRelease(gRtvHeaps.GetAddressOf());
	SafeRelease(gCmdQueue.GetAddressOf());
	SafeRelease(gCmdList.GetAddressOf());
	SafeRelease(gCmdAllocator.GetAddressOf());
	SafeRelease(gSwapChain.GetAddressOf());
	SafeRelease(gDxgiFactory.GetAddressOf());
	SafeRelease(gDevice.GetAddressOf());
}

// debug memory leak
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>

// 自作ライブラリ
#include"System.h"
#include"MathUtil.h"
#include"MMDsdk.h"


int ReturnWithErrorMessage(const char* const message)
{
	DebugMessage(message);
	SafeReleaseAll_D3D_Interface();
	return -1;
}

int Frame();

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

#ifdef _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG
{
	// メモリリークチェック
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ウィンドウクラス作成・登録
	WNDCLASSEX wcEx = {};
	wcEx.cbSize = sizeof(WNDCLASSEX);
	wcEx.lpfnWndProc = WinProc;
	wcEx.lpszClassName = _T("dx12sample");
	wcEx.hInstance = GetModuleHandle(NULL);
	RegisterClassEx(&wcEx);


	// ウィンドウサイズ調整 
	RECT rect = { 0, 0, gWindowWidth, gWindowHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);


	// ウィンドウ作成・描画
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
	if (hwnd == nullptr)
	{
		return ReturnWithErrorMessage("Failed Create Window !");
	}
	ShowWindow(hwnd, SW_SHOW);


	// Direct3D 初期化
	{
		ID3D12Debug* debugLayer = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
			debugLayer->EnableDebugLayer();
			SafeRelease(&debugLayer);
		}
	}


	// デバイスの作成
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

	{
#ifdef _DEBUG
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&gDxgiFactory));
#else
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(&gDxgiFactory));
#endif // _DEBUG

		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create Dxgi Factory !");
		}
	}

	// コマンドリスト、コマンドアロケータの作成
	{
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
	}
	{
		auto result =
			gDevice->CreateCommandList
			(
				0,
				D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
				gCmdAllocator.Get(),
				nullptr,
				IID_PPV_ARGS(&gCmdList)
			);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed To Create Command List !");
		}

	}

	// コマンドキューの作成
	{
		D3D12_COMMAND_QUEUE_DESC cqd = {};
		cqd.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
		cqd.NodeMask = 0;
		cqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cqd.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

		auto result = gDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(&gCmdQueue));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed To Create Command Queue !");
		}
	}

	// スワップチェーンの作成
	{
		DXGI_SWAP_CHAIN_DESC1 scd = {};

		scd.Width = gWindowWidth;
		scd.Height = gWindowHeight;
		scd.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.Stereo = false;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		scd.BufferCount = gBufferCount;
		scd.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
		scd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		auto result =
			gDxgiFactory->CreateSwapChainForHwnd
			(
				gCmdQueue.Get(),
				hwnd,
				&scd,
				nullptr,
				nullptr,
				reinterpret_cast<IDXGISwapChain1**>(gSwapChain.GetAddressOf())
			);

		if (result != S_OK)
		{
			DebugOutParamHex(result);
			return ReturnWithErrorMessage("Failed Create Swap Chain !");
		}
	}

	// RTV の作成
	{
		// ディスクリプタヒープの作成
		D3D12_DESCRIPTOR_HEAP_DESC hd = {};

		hd.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		hd.NodeMask = 0;
		hd.NumDescriptors = gBufferCount;
		hd.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


		auto result = gDevice->CreateDescriptorHeap(&hd, IID_PPV_ARGS(&gRtvHeaps));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create RTV Heap !");
		}

		// スワップチェーンの情報を取得
		DXGI_SWAP_CHAIN_DESC scd = {};
		result = gSwapChain->GetDesc(&scd);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Get SwapchainDesc to Create RenderTargetView !");
		}

		// 全レンダーターゲットを作成
		for (int idx = 0; idx < scd.BufferCount; ++idx)
		{
			result = gSwapChain->GetBuffer(idx, IID_PPV_ARGS(&gBackBuffers[idx]));
			if (result != S_OK)
			{
				DebugOutParamI(idx);
				return ReturnWithErrorMessage("Failed Get Buffer to Create RenderTargetView !");
			}
			auto handle = gRtvHeaps->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += idx * gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			gDevice->CreateRenderTargetView(gBackBuffers[idx].Get(), nullptr, handle);
		}
	}

	// フェンスの作成
	{
		auto result = gDevice->CreateFence(gFenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&gFence));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create Fence");
		}
	}

	// メッセージループ
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// プログラム終了のメッセージが出たら抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}

		if (Frame() == -1)
		{
			return ReturnWithErrorMessage("Error in Main Loop !");
		}
	}


	// 終了処理
	UnregisterClass(wcEx.lpszClassName, wcEx.hInstance);

	SafeReleaseAll_D3D_Interface();

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

int Frame()
{
	auto bbidx = gSwapChain->GetCurrentBackBufferIndex();

	D3D12_RESOURCE_BARRIER bd = {};
	bd.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	bd.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	bd.Transition.pResource = gBackBuffers[bbidx].Get();
	bd.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	bd.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	bd.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	gCmdList->ResourceBarrier(1, &bd);

	auto rtvH = gRtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += static_cast<ULONG_PTR>(bbidx * gDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	gCmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

	float color[] = { 1.f, 1.f, 0.f, 1.f };
	gCmdList->ClearRenderTargetView(rtvH, color, 0, nullptr);

	bd.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	bd.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	gCmdList->ResourceBarrier(1, &bd);

	gCmdList->Close();

	ID3D12CommandList* cmdlists[] = { gCmdList.Get()};
	gCmdQueue->ExecuteCommandLists(1, cmdlists);

	gCmdQueue->Signal(gFence.Get(), ++gFenceVal);

	if (gFence->GetCompletedValue() != gFenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		gFence->SetEventOnCompletion(gFenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	gCmdAllocator->Reset();
	gCmdList->Reset(gCmdAllocator.Get(), nullptr);

	gSwapChain->Present(1, 0);

	return 0;
}


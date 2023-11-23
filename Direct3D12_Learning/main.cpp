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

#include<d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

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

ComPtr<ID3D12Resource> gVertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW gVertexBufferView = {};
D3D12_INPUT_ELEMENT_DESC gInputLayout[] =
{
	{
		"POSITION",
		0,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	},
	{
		"TEXCOORD",
		0,
		DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	}
};

ComPtr<ID3D12Resource> gIndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW gIndexBufferView = {};

ComPtr<ID3DBlob> gVsBlob = nullptr;
ComPtr<ID3DBlob> gPsBlob = nullptr;

ComPtr<ID3D12RootSignature> gRootSignature = nullptr;
ComPtr<ID3D12PipelineState> gPipelineState = nullptr;

D3D12_VIEWPORT gViewport = {};
D3D12_RECT gScissorRect = {};

void SafeReleaseAll_D3D_Interface()
{
	SafeRelease(gPipelineState.GetAddressOf());
	SafeRelease(gPsBlob.GetAddressOf());
	SafeRelease(gVsBlob.GetAddressOf());
	SafeRelease(gVertexBuffer.GetAddressOf());
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


// 頂点データ
int gVertexCount = 0;
struct Vertex
{
	MathUtil::float3 pos;
	MathUtil::float2 uv;
};
Vertex gMesh[] =
{
	{ { -0.4f, -0.7f,  0.f }, { 0.f, 1.f } },
	{ { -0.4f,  0.7f,  0.f }, { 0.f, 0.f } },
	{ {  0.4f, -0.7f,  0.f }, { 1.f, 1.f } },
	{ {  0.4f,  0.7f,  0.f }, { 1.f, 0.f } },
};

// インデックスデータ
int gIndexCount = 0;
unsigned short gIndices[] =
{
	0, 1, 2,
	2, 1, 3
};


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
		ComPtr<IDXGIFactory2> dxgiF = nullptr;
#ifdef _DEBUG
		auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiF.GetAddressOf()));
#else
		auto result = CreateDXGIFactory1(IID_PPV_ARGS(dxgiF.GetAddressOf()));
#endif // _DEBUG
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create DXGI Factory2 !");
		}
		result = dxgiF->QueryInterface(gDxgiFactory.GetAddressOf());
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Query Interface from DXGI Factory2 to 6");
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
		for (unsigned int idx = 0; idx < scd.BufferCount; ++idx)
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


	// リソースの作成
	// 頂点バッファ
	{
		// ヒープの設定
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;

		// リソースの設定
		D3D12_RESOURCE_DESC vrd = {};
		vrd.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		vrd.Width = sizeof(gMesh);
		vrd.Height = 1;
		vrd.DepthOrArraySize = 1;
		vrd.MipLevels = 1;
		vrd.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		vrd.SampleDesc.Count = 1;
		vrd.SampleDesc.Quality = 0;
		vrd.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		vrd.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


		// リソースを作成
		auto result = gDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&vrd,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(gVertexBuffer.GetAddressOf())
		);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create Vertex Buffer Resource !");
		}

		// 確保されているリソース領域を取得
		Vertex* vertexBufferMap = nullptr;
		result = gVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferMap));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Map Vertex Buffer !");
		}

		// 取得したリソース領域に、頂点データを書き込む
		std::copy(std::begin(gMesh), std::end(gMesh), vertexBufferMap);

		// リソース領域はいったん使用しないため、マップを解除
		gVertexBuffer->Unmap(0, nullptr);

		// 作成した頂点バッファのビューを作成
		gVertexBufferView.BufferLocation = gVertexBuffer->GetGPUVirtualAddress();
		gVertexBufferView.SizeInBytes = sizeof(gMesh);
		gVertexBufferView.StrideInBytes = sizeof(gMesh[0]);

		gVertexCount = _countof(gMesh);
	}

	// インデックスバッファの作成
	{
		// ヒープの設定
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;

		// リソースの設定
		D3D12_RESOURCE_DESC ird = {};
		ird.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		ird.Width = sizeof(gIndices);
		ird.Height = 1;
		ird.DepthOrArraySize = 1;
		ird.MipLevels = 1;
		ird.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		ird.SampleDesc.Count = 1;
		ird.SampleDesc.Quality = 0;
		ird.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		ird.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


		// リソースを作成
		auto result = gDevice->CreateCommittedResource
		(
			&heapProp,
			D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
			&ird,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(gIndexBuffer.GetAddressOf())
		);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create Vertex Buffer Resource !");
		}

		// 確保されているリソース領域を取得
		unsigned short* indexBufferMap = nullptr;
		result = gIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexBufferMap));
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Map Vertex Buffer !");
		}

		// 取得したリソース領域に、頂点データを書き込む
		std::copy(std::begin(gIndices), std::end(gIndices), indexBufferMap);

		// リソース領域はいったん使用しないため、マップを解除
		gIndexBuffer->Unmap(0, nullptr);

		// 作成した頂点バッファのビューを作成
		gIndexBufferView.BufferLocation = gIndexBuffer->GetGPUVirtualAddress();
		gIndexBufferView.SizeInBytes = sizeof(gIndices);
		gIndexBufferView.Format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		gIndexCount = _countof(gIndices);
	}

	// テクスチャデータの作成
	{
		// 実験的に元リソースをスコープ内に置く
		// エラーが出るならグローバルに
		struct TexRGBA
		{
			unsigned char r, g, b, a;
		};

		const int texSize = 256;

		std::vector<TexRGBA> texturedata(texSize * texSize);

		for (auto& rgba : texturedata)
		{
			rgba.r = rand() % texSize;
			rgba.g = rand() % texSize;
			rgba.b = rand() % texSize;
			rgba.a = rand() % texSize;
		}

		{
			D3D12_HEAP_PROPERTIES heapProp = {};
			heapProp.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
			heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
			heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
			heapProp.CreationNodeMask = 0;
			heapProp.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC resDesc = {};
			resDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			resDesc.Width = resDesc.Height = texSize;
			resDesc.DepthOrArraySize = 1;
			resDesc.SampleDesc = { 1, 0 };
			resDesc.MipLevels = 1;
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resDesc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;

			ComPtr<ID3D12Resource> texBuffer = nullptr;
			auto result = gDevice->CreateCommittedResource
			(
				&heapProp,
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				nullptr,
				IID_PPV_ARGS(texBuffer.GetAddressOf())
			);

			if (result != S_OK)
			{
				return ReturnWithErrorMessage("Failed Create Texture Resource !");
			}
		}
	}

	// シェーダーのコンパイル
	{
		// 頂点シェーダー
		ComPtr<ID3DBlob> errorBlob = nullptr;
		auto result = D3DCompileFromFile
		(
			_T("BasicVertexShader.hlsl"),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicVS",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			gVsBlob.GetAddressOf(),
			errorBlob.GetAddressOf()
		);
		if (result != S_OK)
		{
			SafeRelease(errorBlob.GetAddressOf());
			return ReturnWithErrorMessage("Failed Compile Vertex Shader !");
		}

		// ピクセルシェーダー
		result = D3DCompileFromFile
		(
			_T("BasicPixelShader.hlsl"),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"BasicPS",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			gPsBlob.GetAddressOf(),
			errorBlob.GetAddressOf()
		);
		if (result != S_OK)
		{
			SafeRelease(errorBlob.GetAddressOf());
			return ReturnWithErrorMessage("Failed Compile Pixel Shader");
		}
	}

	// ルートシグネチャの作成
	{
		D3D12_ROOT_SIGNATURE_DESC rsd = {};
		rsd.Flags =
			D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> rsb = nullptr;
		ComPtr<ID3DBlob> err = nullptr;

		auto result = D3D12SerializeRootSignature
		(
			&rsd,
			D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_0,
			rsb.GetAddressOf(),
			err.GetAddressOf()
		);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Serialize RootSigunature !");
		}

		result = gDevice->CreateRootSignature
		(
			0,
			rsb->GetBufferPointer(),
			rsb->GetBufferSize(),
			IID_PPV_ARGS(gRootSignature.GetAddressOf())
		);
		if (result != S_OK)
		{
			return ReturnWithErrorMessage("Failed Create RootSignature !");
		}

		SafeRelease(rsb.GetAddressOf());
		SafeRelease(err.GetAddressOf());
	}

	// パイプラインステートの作成
	{
		// パイプラインステートの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC plsd = {};
		// ルートシグネチャの設定
		plsd.pRootSignature = gRootSignature.Get();

		// シェーダをセット
		plsd.VS.pShaderBytecode = gVsBlob->GetBufferPointer();
		plsd.VS.BytecodeLength = gVsBlob->GetBufferSize();
		plsd.PS.pShaderBytecode = gPsBlob->GetBufferPointer();
		plsd.PS.BytecodeLength = gPsBlob->GetBufferSize();

		// サンプリングの設定
		plsd.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		plsd.RasterizerState.MultisampleEnable = false;
		plsd.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		plsd.RasterizerState.FillMode = D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		plsd.RasterizerState.DepthClipEnable = true;

		// ブレンドの設定
		plsd.BlendState.AlphaToCoverageEnable = false;
		plsd.BlendState.IndependentBlendEnable = false;
		D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
		rtBlendDesc.BlendEnable = false;
		rtBlendDesc.LogicOpEnable = false;
		rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
		plsd.BlendState.RenderTarget[0] = rtBlendDesc;

		// 頂点の設定
		plsd.InputLayout.pInputElementDescs = gInputLayout;
		plsd.InputLayout.NumElements = _countof(gInputLayout);
		plsd.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE::D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// プリミティブトポロジーの設定
		plsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// レンダーターゲットの設定
		plsd.NumRenderTargets = 1;
		plsd.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		// アンチエイリアシングの設定
		plsd.SampleDesc.Count = 1;
		plsd.SampleDesc.Quality = 0;

		// パイプラインステートオブジェクトの生成
		auto result = gDevice->CreateGraphicsPipelineState(&plsd, IID_PPV_ARGS(gPipelineState.GetAddressOf()));
		if (result != S_OK)
		{
			DebugOutParamHex(result);
			return ReturnWithErrorMessage("Failed Create PIpeline state !");
		}
	}

	gViewport.Width = gWindowWidth;
	gViewport.Height = gWindowHeight;
	gViewport.TopLeftX = 0;
	gViewport.TopLeftY = 0;
	gViewport.MaxDepth = 1.f;
	gViewport.MinDepth = 0.f;

	gScissorRect.top = 0;
	gScissorRect.left = 0;
	gScissorRect.right = gScissorRect.left + gWindowWidth;
	gScissorRect.bottom = gScissorRect.top + gWindowHeight;

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

	float color[] = { 0.f, 0.f, 0.5f, 1.f };
	gCmdList->ClearRenderTargetView(rtvH, color, 0, nullptr);

	gCmdList->SetPipelineState(gPipelineState.Get());
	gCmdList->SetGraphicsRootSignature(gRootSignature.Get());
	gCmdList->RSSetViewports(1, &gViewport);
	gCmdList->RSSetScissorRects(1, &gScissorRect);
	gCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gCmdList->IASetVertexBuffers(0, 1, &gVertexBufferView);
	gCmdList->IASetIndexBuffer(&gIndexBufferView);
	gCmdList->DrawIndexedInstanced(gIndexCount, 1, 0, 0, 0);

	bd.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	bd.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	gCmdList->ResourceBarrier(1, &bd);

	gCmdList->Close();

	ID3D12CommandList* cmdlists[] = { gCmdList.Get() };
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


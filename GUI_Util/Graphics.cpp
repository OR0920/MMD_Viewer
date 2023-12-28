#include"GUI_Util.h"

// std

// windows
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#include"d3dx12.h"
#include<d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#define D3D12Allignment(buffSize) (buffSize + 0xff) & ~0xff;

// my lib
#include "System.h"

#define ReturnIfFailed(func, at)\
{\
	auto result = func;\
	if(FAILED(result))\
	{\
		DebugMessageFunctionError(func, at);\
		DebugOutParamHex(result);\
		return GUI::Result::FAIL;\
	}\
}

using namespace GUI;
using namespace GUI::Graphics;

// デバッグレイヤ

Result Graphics::EnalbleDebugLayer()
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;
	ReturnIfFailed
	(
		D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf())),
		Graphics::EnalbleDebugLayer()
	);

	debug->EnableDebugLayer();

#endif // _DEBUG
	return SUCCESS;
}

// デバイス
Device::Device()
	:
	mDevice(nullptr)
{

}

Device::~Device()
{

}

Result Device::Create()
{
	ReturnIfFailed
	(
		D3D12CreateDevice
		(
			nullptr,
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(mDevice.ReleaseAndGetAddressOf())
		),
		Device::Create()
	);

	return SUCCESS;
}

Result Device::CreateGraphicsCommand(GraphicsCommand& command)
{
	auto type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Type = type;

	ReturnIfFailed
	(
		mDevice->CreateCommandQueue
		(
			&commandQueueDesc,
			IID_PPV_ARGS(command.mCommandQueue.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	ReturnIfFailed
	(
		mDevice->CreateCommandAllocator
		(
			type,
			IID_PPV_ARGS(command.mCommandAllocator.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	ReturnIfFailed
	(
		mDevice->CreateCommandList
		(
			0,
			type,
			command.mCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(command.mCommandList.ReleaseAndGetAddressOf())
		),
		Device::CreateGraphicsCommand()
	);

	return SUCCESS;
}

// コマンド
GraphicsCommand::GraphicsCommand()
	:
	mCommandQueue(nullptr),
	mCommandAllocator(nullptr),
	mCommandList()
{

}

GraphicsCommand::~GraphicsCommand()
{

}

// スワップチェイン

SwapChain::SwapChain()
	:
	mSwapChain(nullptr)
{

}

SwapChain::~SwapChain()
{

}

Result SwapChain::Create
(
	const GraphicsCommand& device,
	const ParentWindow& targetWindow,
	const int frameCount
)
{
	if (targetWindow.GetHandle() == 0)
	{
		DebugMessage("The Target Window Is not Exist !");

		return FAIL;
	}

	ComPtr<IDXGIFactory> tFactory;
	ReturnIfFailed
	(
		CreateDXGIFactory2
		(
			DXGI_CREATE_FACTORY_DEBUG,
			IID_PPV_ARGS(tFactory.ReleaseAndGetAddressOf())
		),
		SwapChain::Create();
	);

	ComPtr<IDXGIFactory4> factory4;

	ReturnIfFailed
	(
		tFactory->QueryInterface(IID_PPV_ARGS(factory4.ReleaseAndGetAddressOf())),
		SwapChain::Create()
	);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = targetWindow.GetWindowWidth();
	swapChainDesc.Height = targetWindow.GetWindowHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = frameCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> tSwCh1;

	ReturnIfFailed
	(
		factory4->CreateSwapChainForHwnd
		(
			device.mCommandQueue.Get(),
			targetWindow.GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			tSwCh1.ReleaseAndGetAddressOf()
		),
		SwapChain::Create()
	);

	ReturnIfFailed
	(
		tSwCh1->QueryInterface(mSwapChain.ReleaseAndGetAddressOf()),
		SwapChain::Create()
	);
	return SUCCESS;
}



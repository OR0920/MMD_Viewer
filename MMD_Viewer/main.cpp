#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"VertexShader2D.h"
#include"PixelShader2D.h"

static const int HD = 720;
static const int FHD = 1080;
static const int windowHeight = HD;
static const int windowWidth = windowHeight / 9 * 16;

int MAIN()
{
	// デバッグ表示を日本語に対応させる
	SET_JAPANESE_ENABLE;

	// メモリリークのデバッグを有効化
	System::CheckMemoryLeak();

	//　メインウィンドウのインスタンスを取得
	auto& mainWindow = GUI::MainWindow::Instance();

	// ウィンドウを作成
	if (mainWindow.Create(windowWidth, windowHeight) == GUI::Result::FAIL)
	{
		return -1;
	}

	// ファイル取得ウィンドウの初期化
	auto& fc = GUI::FileCatcher::Instance();

	if (fc.Create(mainWindow) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 描画エンジン初期化

	// デバッグモード　有効化
	if (GUI::Graphics::EnalbleDebugLayer() == GUI::Result::FAIL)
	{
		return -1;
	}

	// デバイス作成
	GUI::Graphics::Device device;
	if (device.Create() == GUI::Result::FAIL)
	{
		return -1;
	}

	// コマンドオブジェクト作成
	GUI::Graphics::GraphicsCommand command;
	if (device.CreateGraphicsCommand(command) == GUI::Result::FAIL)
	{
		return -1;
	}

	// スワップチェイン作成

	GUI::Graphics::SwapChain swapChain;
	if (swapChain.Create(command, mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	// レンダーターゲット作成
	GUI::Graphics::RenderTarget renderTarget;
	if (device.CreateRenderTarget(renderTarget, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 深度ステンシル作成
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}


	// 入力レイアウト作成
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.SetDefaultPositionDesc();
	inputElementDesc.SetDefaultColorDesc();

	inputElementDesc.DebugOutLayout();


	// 頂点バッファ作成 インデックスバッファ作成
	struct Vertex
	{
		MathUtil::float3 position;
		MathUtil::float4 color;
	};

	Vertex triangle[] =
	{
		{ {  0.5f,  0.5f, 0.f }, { 1.f, 1.f, 0.f, 1.f } },
		{ {  0.5f, -0.5f, 0.f }, { 0.f, 1.f, 1.f, 1.f } },
		{ { -0.5f, -0.5f, 0.f }, { 1.f, 0.f, 1.f, 1.f } },
		{ { -0.5f,  0.5f, 0.f }, { 1.f, 1.f, 1.f, 1.f } },
	};

	int triangleIndex[] = { 0, 1, 2, 2, 3, 0 };

	auto triangleSize = sizeof(triangle);

	GUI::Graphics::VertexBuffer vertexBuffer;
	if (device.CreateVertexBuffer(vertexBuffer, sizeof(Vertex), _countof(triangle)) == GUI::Result::FAIL)
	{
		return -1;
	}

	if (vertexBuffer.Copy(triangle) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::Graphics::IndexBuffer indexBuffer;
	if (device.CreateIndexBuffer(indexBuffer, sizeof(int), _countof(triangleIndex)) == GUI::Result::FAIL)
	{
		return -1;
	}

	if (indexBuffer.Copy(triangleIndex) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 定数バッファ用のディスクリプタヒープ作成
	const int descriptorCount = 2;
	GUI::Graphics::DescriptorHeapForShaderData descHeap;
	{
		auto result = device.CreateDescriptorHeap(descHeap, descriptorCount);
		if (result == GUI::Result::FAIL)
		{
			return -1;
		}
	}

	// 定数バッファ作成

	struct Transform
	{
		MathUtil::Matrix translation;
		MathUtil::Matrix rotation;
	};

	GUI::Graphics::ConstantBuffer transform;
	{
		auto result = device.CreateConstantBuffer
		(
			transform,
			descHeap,
			sizeof(Transform)
		);

		if (result == GUI::Result::FAIL)
		{
			return -1;
		}
	}

	Transform* mappedTransform = nullptr;
	if (transform.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
	{
		mappedTransform->translation = MathUtil::Matrix::GenerateMatrixTranslation(MathUtil::Vector(0.5f, 0.f, 0.f));
		mappedTransform->rotation = MathUtil::Matrix::GenerateMatrixRotationZ(MathUtil::DegreeToRadian(45.f));
	}

	struct Color
	{
		GUI::Graphics::Color color;
	};
	
	GUI::Graphics::ConstantBuffer color;
	if (device.CreateConstantBuffer(color, descHeap, sizeof(Color)) == GUI::Result::FAIL)
	{
		return -1;
	}

	Color* mappedColor;
	if (color.Map(reinterpret_cast<void**>(&mappedColor)) == GUI::Result::SUCCESS)
	{
		mappedColor->color = GUI::Graphics::Color(1.f, 0.f, 0.f);
	}
	
	// ルートシグネチャ作成
	GUI::Graphics::RootSignature rootSignature;
	rootSignature.SetParameterCount(2);
	rootSignature.SetParamForCBV(0, 0);
	rootSignature.SetParamForCBV(1, 1);
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	// パイプラインステート
	GUI::Graphics::GraphicsPipeline pipeline;
	pipeline.SetInputLayout(inputElementDesc);
	pipeline.SetRootSignature(rootSignature);
	pipeline.SetVertexShader(VertexShader2D, _countof(VertexShader2D));
	pipeline.SetPixelShader(PixelShader2D, _countof(PixelShader2D));

	if (device.CreateGraphicsPipeline(pipeline) == GUI::Result::FAIL)
	{
		return -1;
	}

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
		}

		command.BeginDraw();

		command.SetGraphicsPipeline(pipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);
		command.SetGraphicsRootSignature(rootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		command.SetDescriptorHeap(descHeap);
		command.SetConstantBuffer(transform, 0);
		command.SetConstantBuffer(color, 1);

		//command.DrawTriangle(vertexBuffer);
		command.DrawTriangleList(vertexBuffer, indexBuffer);

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

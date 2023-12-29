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

int MAIN()
{
	// デバッグ表示を日本語に対応させる
	SET_JAPANESE_ENABLE;

	// メモリリークのデバッグを有効化
	System::CheckMemoryLeak();

	//　メインウィンドウのインスタンスを取得
	auto& mainWindow = GUI::MainWindow::Instance();

	// ウィンドウを作成
	if (mainWindow.Create(1280, 720) == GUI::Result::FAIL)
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

	// ルートシグネチャ作成
	GUI::Graphics::RootSignature rootSignature;
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 入力レイアウト作成
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.DefaultPosition();
	inputElementDesc.DefaultColor();

	inputElementDesc.DebugOutLayout();

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

	// 頂点バッファ作成
	struct Vertex
	{
		MathUtil::float3 position;
		MathUtil::float4 color;
	};

	Vertex triangle[] =
	{
		{ {   0.f, 0.5f, 0.f }, { 1.f, 0.f, 0.f, 1.f } },
		{ {  0.5f,  0.f, 0.f }, { 0.f, 1.f, 0.f, 1.f } },
		{ { -0.5f,  0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f } }
	};

	auto triangleSize = sizeof(triangle);
	DebugOutParam(triangleSize);

	GUI::Graphics::VertexBuffer vertexBuffer;
	if (device.CreateVertexBuffer(vertexBuffer, sizeof(Vertex), 3) == GUI::Result::FAIL)
	{
		return -1;
	}

	if (vertexBuffer.Copy(reinterpret_cast<unsigned char*>(triangle)) == GUI::Result::FAIL)
	{
		return -1;
	}

	// モデル作成
	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
		}

		command.BeginDraw(pipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(&renderTarget, &depthStencil);
		command.SetGraphicsRootSignature(rootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		command.DrawTriangles(vertexBuffer);

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

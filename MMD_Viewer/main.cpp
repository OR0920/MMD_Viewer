#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"VertexShader.h"
#include"PixelShader.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"

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

	// モデル作成
	Model model(device);
	

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			model.Load(fc.GetPath());
		}

		command.BeginDraw();

		//command.SetGraphicsPipeline(mmdPipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);
		//command.SetGraphicsRootSignature(mmdRootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		//command.SetDescriptorHeap(descHeap);
		//command.SetConstantBuffer(transform, 0);
		//command.SetConstantBuffer(psData, 1);

		if (model.IsSuccessLoad() == GUI::Result::SUCCESS)
		{
			model.Draw(command);
			//command.DrawTriangleList(model.GetVB(), model.GetIB());
		}

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

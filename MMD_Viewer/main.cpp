#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

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

	GUI::Button button;
	if (button.Create(mainWindow, L"回転ON/OFF", 100, 50, 10, 10) == GUI::Result::FAIL)
	{
		return -1;
	}

	Model* model = nullptr;
	

	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			System::SafeDelete(&model);
			model = new Model(device);
			model->Load(fc.GetPath());
			if (model->IsSuccessLoad() == GUI::Result::FAIL)
			{
				GUI::ErrorBox(L"対応していないファイルです");
				System::SafeDelete(&model);
			}
		}

		command.BeginDraw();

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);

		command.ClearRenderTarget(GUI::Graphics::Color(0.3f, 0.3f, 0.3f));
		command.ClearDepthBuffer();


		if (model != nullptr)
		{
				model->SetDefaultSceneData();
				model->Draw(command);
		}
		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}

	System::SafeDelete(&model);
}

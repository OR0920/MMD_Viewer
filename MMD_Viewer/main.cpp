#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"

static const int windowHeight = 1000;
static const int windowWidth = 1000;


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

	// マルチパス用のレンダーターゲット
	GUI::Graphics::SubRenderTarget subRenderTarget;
	GUI::Graphics::Format format[] = 
	{
		GUI::Graphics::Format::COLOR_8_4,
	};
	if (device.CreateSubRenderTarget(subRenderTarget, renderTarget, format, 1) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 深度ステンシル作成
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}


	// ここにモデルを作る
	Model* model = nullptr;


	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// モデル読み込み
			System::SafeDelete(&model);
			model = new Model(device);
			model->Load(fc.GetPath());
			
			// 読み込み失敗
			if (model->IsSuccessLoad() == GUI::Result::FAIL)
			{
				GUI::ErrorBox(L"対応していないファイルです");
				System::SafeDelete(&model);
				continue;
			}

			// シーン初期化、モデル1体のみなので、シーンクラスなどには分けない
			model->SetDefaultSceneData(renderTarget.GetAspectRatio());
		}

		// 描画準備
		command.BeginDraw();
		command.SetViewportAndRect(renderTarget);

		// レンダーターゲットを書き込み可能に
		command.UnlockRenderTarget(renderTarget);
		
		// レンダーターゲットのクリア
		command.SetRenderTarget(renderTarget, depthStencil);
		command.ClearRenderTarget(GUI::Graphics::Color(0.3f, 0.3f, 0.3f));
		command.ClearDepthBuffer();

		// モデル描画
		if (model != nullptr)
		{
			model->Draw(command);
		}

		// レンダーターゲットを保護
		command.LockRenderTarget(renderTarget);
		
		// 描画終了
		command.EndDraw();

		// バックバッファ切り替え
		command.Flip();
	}

	System::SafeDelete(&model);
}

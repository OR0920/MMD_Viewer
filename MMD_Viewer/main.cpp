#include"System.h"
#include"GUI_Util.h"
using namespace std;

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"

#define QuitIfFailed(func)\
if(func == GUI::Result::FAIL) \
{\
	DebugMessageFunctionError(func, MAIN);\
	return -1;\
}

static const int windowHeight = 1080;
static const int windowWidth = 1080;

const GUI::Graphics::Color clearColor(1.f, 1.f, 1.f);

#pragma comment( lib, "winmm.lib")

class Timer
{
public:
	Timer()
		:
		mInitTime((0xffffffff - timeGetTime()) - 6000),
		mPrevTime(0),
		mCurrentTime(0),
		mFrameTime(0),
		mFrameRate(0)
	{

	}
	unsigned int GetFrameTime()
	{
		return mFrameTime;
	}
	unsigned int GetFrameRate()
	{
		return mFrameRate;
	}
	void Update()
	{
		mCurrentTime = timeGetTime() + mInitTime;
		mFrameTime = mCurrentTime - mPrevTime;
		if (mFrameTime < 1)
		{
			DebugMessageWarning("The frame time is less than 1[ms]. The" << ToString(Timer::Update()) << " might have been Called more than twice at frame.");
		}

		mFrameRate = 1000.f / static_cast<float>(mFrameTime);

		mPrevTime = mCurrentTime;
	}
private:
	unsigned int mInitTime;
	unsigned int mPrevTime;
	unsigned int mCurrentTime;
	unsigned int mFrameTime;
	unsigned int mFrameRate;
};

int MAIN()
{
	// デバッグ表示を日本語に対応させる
	SET_JAPANESE_ENABLE;

	// メモリリークのデバッグを有効化
	System::CheckMemoryLeak();

	//　メインウィンドウのインスタンスを取得
	auto& mainWindow = GUI::MainWindow::Instance();

	// ウィンドウを作成
	QuitIfFailed(mainWindow.Create(windowWidth, windowHeight, L"MMD Viewer"));

	// ファイル取得ウィンドウの初期化
	GUI::FileCatcher fc;
	QuitIfFailed(fc.Create(mainWindow));

	// 描画エンジン初期化
	// デバッグモード　有効化
	QuitIfFailed(GUI::Graphics::EnalbleDebugLayer());

	// デバイス作成
	GUI::Graphics::Device device;
	QuitIfFailed(device.Create());

	// コマンドオブジェクト作成
	GUI::Graphics::GraphicsCommand command;
	QuitIfFailed(device.CreateGraphicsCommand(command));

	// スワップチェイン作成
	GUI::Graphics::SwapChain swapChain;
	QuitIfFailed(swapChain.Create(command, mainWindow, 2));

	// レンダーターゲット作成
	GUI::Graphics::RenderTarget renderTarget;
	QuitIfFailed(device.CreateRenderTarget(renderTarget, swapChain));

	// 深度ステンシル作成
	GUI::Graphics::DepthStencilBuffer depthStencil;
	QuitIfFailed(device.CreateDepthBuffer(depthStencil, swapChain));

	// ここにモデルを作る
	Model* model = nullptr;

	Timer time;

	unsigned int timeCounter = 0;
	unsigned int frameCounter = 0;
	const unsigned int waitTime = 5000;
	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		time.Update();
		timeCounter += time.GetFrameTime();
		frameCounter++;
		if (timeCounter > waitTime)
		{
			auto averageFPS = 
				static_cast<float>(frameCounter) / static_cast<float>(timeCounter/1000);

			DebugOutParam(time.GetFrameRate());
			DebugOutParam(averageFPS);

			frameCounter = 0;
			timeCounter %= waitTime;
		}

		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// モデル読み込み
			System::SafeDelete(&model);
			model = new Model(device);

			if (model->Load(fc.GetPath()) == GUI::Result::FAIL)
			{
				// 読み込み失敗したら無かったことにする
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
		command.ClearRenderTarget(clearColor);
		command.ClearDepthBuffer();

		// モデル描画
		if (model != nullptr)
		{
			model->Update();
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

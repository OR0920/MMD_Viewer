#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

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
	if (GUI::GraphicsDevice::Init(mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	auto& device = GUI::GraphicsDevice::Instance();

	GUI::Model model;

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// モデル削除

			// モデル読み込み
			// 読み込み用モデルクラス
			GUI::Model tModel;
			if (tModel.Load(fc.GetPath()) == GUI::Result::SUCCESS)
			{
				model.Reset();
				// モデルをコピー
				model = tModel;
			}
			else
			{
				GUI::ErrorBox(L"サポートされていないファイルです\n Not Supported File");
			}
			DebugMessageNewLine();

		}

		device.BeginDraw();
		// 画面クリア
		device.Clear(GUI::Color(0.5f, 0.5f, 0.5f));

		// カメラ更新

		// モデル描画
		model.Draw();

		device.EndDraw();
	}
	GUI::GraphicsDevice::Tern();
}

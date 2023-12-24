#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
#include"GraphicsEngine.h"
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

	GUI::Graphics::GraphicsEngine engine;
	if (engine.Init(mainWindow) == GUI::Result::FAIL)
	{
		return -1;
	}

	// シーン作成
	GUI::Graphics::Scene scene;

	// 背景色を設定
	scene.SetBackGroundColor(GUI::Graphics::Color::Gray);

	while (mainWindow.IsClose() == false)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// モデル読み込み
			

			// シーンにモデルを登録

		}

		// マウスの入力をカメラに反映 Scene
		
		// シーンを描画
		engine.Draw(scene); 
	}
	
}

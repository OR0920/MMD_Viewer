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

	// モデル作成
	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
		}

		device.BeginDraw();
		device.Clear(GUI::Color(0.5f, 0.5f, 0.5f));
		
		device.EndDraw();
	}
	GUI::GraphicsDevice::Tern();
}

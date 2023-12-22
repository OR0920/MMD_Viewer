#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;



int main()
{
	SET_JAPANESE_ENABLE;

	System::CheckMemoryLeak();

	auto& mainWindow = GUI::MainWindow::Instance();

	if (mainWindow.Create(1280, 720) == GUI::Result::FAIL)
	{
		return -1;
	}

	auto& fc = GUI::FileCatcher::Instance();



	if (fc.Create(mainWindow) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::GraphicsEngine engine;

	if (engine.Init(mainWindow) == GUI::Result::FAIL)
	{
		return -1;
	};

	GUI::GraphicsEngine::Scene scene;
	using Color = GUI::GraphicsEngine::Color;
	scene.SetBackGroundColor(Color::Gray);


	while (mainWindow.IsClose() == false)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
			DebugOutParam(fc.GetDropPos().x);
			DebugOutParam(fc.GetDropPos().y);

			GUI::GraphicsEngine::Model model;
			model.Load(fc.GetPath());
		}

		engine.Draw(scene);
	}
}

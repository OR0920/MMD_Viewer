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
	// �f�o�b�O�\������{��ɑΉ�������
	SET_JAPANESE_ENABLE;
	
	// ���������[�N�̃f�o�b�O��L����
	System::CheckMemoryLeak();

	//�@���C���E�B���h�E�̃C���X�^���X���擾
	auto& mainWindow = GUI::MainWindow::Instance();

	// �E�B���h�E���쐬
	if (mainWindow.Create(1280, 720) == GUI::Result::FAIL)
	{
		return -1;
	}

	// �t�@�C���擾�E�B���h�E�̏�����
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

	// �V�[���쐬
	GUI::Graphics::Scene scene;

	// �w�i�F��ݒ�
	scene.SetBackGroundColor(GUI::Graphics::Color::Gray);

	while (mainWindow.IsClose() == false)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// ���f���ǂݍ���
			

			// �V�[���Ƀ��f����o�^

		}

		// �}�E�X�̓��͂��J�����ɔ��f Scene
		
		// �V�[����`��
		engine.Draw(scene); 
	}
	
}

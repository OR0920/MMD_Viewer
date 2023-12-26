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

	// �`��G���W��������
	if (GUI::Canvas::Init(mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	auto& canvas = GUI::Canvas::Instance();

	GUI::Model model;

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// ���f���폜

			// ���f���ǂݍ���
			// �ǂݍ��ݗp���f���N���X
			GUI::Model tModel;
			if (tModel.Load(fc.GetPath()) == GUI::Result::SUCCESS)
			{
				model.Reset();
				// ���f�����R�s�[
				model = tModel;
			}
			else
			{
				GUI::ErrorBox(L"�T�|�[�g����Ă��Ȃ��t�@�C���ł�\n Not Supported File");
			}
			DebugMessageNewLine();

		}

		canvas.BeginDraw();
		// ��ʃN���A
		canvas.Clear(GUI::Color(0.5f, 0.5f, 0.5f));

		// �J�����X�V

		// ���f���`��
		model.Draw();

		canvas.EndDraw();
	}
	GUI::Canvas::Tern();
}

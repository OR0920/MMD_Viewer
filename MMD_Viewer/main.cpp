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
	if (GUI::GraphicsDevice::Init(mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	auto& device = GUI::GraphicsDevice::Instance();

	// ���f���쐬
	GUI::Model model;


	// �J����������
	MathUtil::float3 eye = { 0.f, 0.f, -50.f };
	MathUtil::float3 target = { 0.f, 0.f, 0.f };
	MathUtil::float3 up = MathUtil::Vector::basicZ.GetFloat3();
	device.SetCamera(eye, target, up);

	
	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		device.BeginDraw();
		// ��ʃN���A
		device.Clear(GUI::Color(0.f, 0.f, 1.f));

		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// ���f���폜

			// ���f���ǂݍ���
			// �ǂݍ��ݗp���f���N���X
			GUI::Model tModel;
			if (tModel.Load(fc.GetPath()) == GUI::Result::SUCCESS)
			{
				tModel.Draw();
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


		// �J�����X�V
		
		// ���f���`��
		model.Draw();

		device.EndDraw();
	}
	GUI::GraphicsDevice::Tern();
}

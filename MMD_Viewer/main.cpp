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
	
	// �f�o�b�O���[�h�@�L����
	if (GUI::Graphics::EnalbleDebugLayer() == GUI::Result::FAIL)
	{
		return -1;
	}

	// �f�o�C�X�쐬
	GUI::Graphics::Device device;
	if (device.Create() == GUI::Result::FAIL)
	{
		return -1;
	}

	// �R�}���h�I�u�W�F�N�g�쐬
	GUI::Graphics::GraphicsCommand command;
	if (device.CreateGraphicsCommand(command) == GUI::Result::FAIL)
	{
		return -1;
	}
	
	// �X���b�v�`�F�C���쐬

	GUI::Graphics::SwapChain swapChain;
	if (swapChain.Create(command, mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	// �����_�[�^�[�Q�b�g�쐬
	GUI::Graphics::RenderTarget renderTarget;
	if (device.CreateRenderTarget(renderTarget, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}

	// �[�x�X�e���V���쐬
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}

	// ���[�g�V�O�l�`���쐬
	GUI::Graphics::RootSignature rootSignature;
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}


	// ���f���쐬
	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
		}

		command.BeginDraw();
		
		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(&renderTarget, &depthStencil);

		command.ClearRenderTarget(GUI::Graphics::Color(0.f, 0.f, 0.5f));
		command.ClearDepthBuffer();

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

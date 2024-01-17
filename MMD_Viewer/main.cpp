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
	// �f�o�b�O�\������{��ɑΉ�������
	SET_JAPANESE_ENABLE;

	// ���������[�N�̃f�o�b�O��L����
	System::CheckMemoryLeak();

	//�@���C���E�B���h�E�̃C���X�^���X���擾
	auto& mainWindow = GUI::MainWindow::Instance();

	// �E�B���h�E���쐬
	if (mainWindow.Create(windowWidth, windowHeight) == GUI::Result::FAIL)
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

	// �}���`�p�X�p�̃����_�[�^�[�Q�b�g
	GUI::Graphics::SubRenderTarget subRenderTarget;
	GUI::Graphics::Format format[] = 
	{
		GUI::Graphics::Format::COLOR_8_4,
	};
	if (device.CreateSubRenderTarget(subRenderTarget, renderTarget, format, 1) == GUI::Result::FAIL)
	{
		return -1;
	}

	// �[�x�X�e���V���쐬
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}


	// �����Ƀ��f�������
	Model* model = nullptr;


	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// ���f���ǂݍ���
			System::SafeDelete(&model);
			model = new Model(device);
			model->Load(fc.GetPath());
			
			// �ǂݍ��ݎ��s
			if (model->IsSuccessLoad() == GUI::Result::FAIL)
			{
				GUI::ErrorBox(L"�Ή����Ă��Ȃ��t�@�C���ł�");
				System::SafeDelete(&model);
				continue;
			}

			// �V�[���������A���f��1�݂̂̂Ȃ̂ŁA�V�[���N���X�Ȃǂɂ͕����Ȃ�
			model->SetDefaultSceneData(renderTarget.GetAspectRatio());
		}

		// �`�揀��
		command.BeginDraw();
		command.SetViewportAndRect(renderTarget);

		// �����_�[�^�[�Q�b�g���������݉\��
		command.UnlockRenderTarget(renderTarget);
		
		// �����_�[�^�[�Q�b�g�̃N���A
		command.SetRenderTarget(renderTarget, depthStencil);
		command.ClearRenderTarget(GUI::Graphics::Color(0.3f, 0.3f, 0.3f));
		command.ClearDepthBuffer();

		// ���f���`��
		if (model != nullptr)
		{
			model->Draw(command);
		}

		// �����_�[�^�[�Q�b�g��ی�
		command.LockRenderTarget(renderTarget);
		
		// �`��I��
		command.EndDraw();

		// �o�b�N�o�b�t�@�؂�ւ�
		command.Flip();
	}

	System::SafeDelete(&model);
}

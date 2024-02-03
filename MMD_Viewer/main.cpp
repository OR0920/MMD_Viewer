#include"System.h"
#include"GUI_Util.h"
using namespace std;

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"
#include"FPS_Monitor.h"

#define QuitIfFailed(func)\
if(func == GUI::Result::FAIL) \
{\
	DebugMessageFunctionError(func, MAIN);\
	return -1;\
}

static const int windowHeight = 1000;
static const int windowWidth = 1000;

const GUI::Graphics::Color clearColor(1.f, 1.f, 1.f);

int MAIN()
{
	// �f�o�b�O�\������{��ɑΉ�������
	SET_JAPANESE_ENABLE;

	// ���������[�N�̃f�o�b�O��L����
	System::CheckMemoryLeak();

	//�@���C���E�B���h�E�̃C���X�^���X���擾
	auto& mainWindow = GUI::MainWindow::Instance();

	// �E�B���h�E���쐬
	QuitIfFailed(mainWindow.Create(windowWidth, windowHeight, L"MMD Viewer"));

	// �t�@�C���擾�E�B���h�E�̏�����
	GUI::FileCatcher fc;
	QuitIfFailed(fc.Create(mainWindow));

	// �`��G���W��������
	// �f�o�b�O���[�h�@�L����
	QuitIfFailed(GUI::Graphics::EnalbleDebugLayer());

	// �f�o�C�X�쐬
	GUI::Graphics::Device device;
	QuitIfFailed(device.Create());

	// �R�}���h�I�u�W�F�N�g�쐬
	GUI::Graphics::GraphicsCommand command;
	QuitIfFailed(device.CreateGraphicsCommand(command));

	// �X���b�v�`�F�C���쐬
	GUI::Graphics::SwapChain swapChain;
	QuitIfFailed(swapChain.Create(command, mainWindow, 2));

	// �����_�[�^�[�Q�b�g�쐬
	GUI::Graphics::RenderTarget renderTarget;
	QuitIfFailed(device.CreateRenderTarget(renderTarget, swapChain));

	// �[�x�X�e���V���쐬
	GUI::Graphics::DepthStencilBuffer depthStencil;
	QuitIfFailed(device.CreateDepthBuffer(depthStencil, swapChain));

	// �����Ƀ��f�������
	Model* model = nullptr;
	FPS_Monitor monitor(1000);

	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		monitor.Update();
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			// ���f���ǂݍ���
			System::SafeDelete(&model);
			model = new Model(device);

			if (model->Load(fc.GetPath()) == GUI::Result::FAIL)
			{
				// �ǂݍ��ݎ��s�����疳���������Ƃɂ���
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
		command.ClearRenderTarget(clearColor);
		command.ClearDepthBuffer();

		// ���f���`��
		if (model != nullptr)
		{
			model->Update(monitor.GetFrameTime());
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

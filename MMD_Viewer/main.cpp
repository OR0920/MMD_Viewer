#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"

static const int HD = 720;
static const int FHD = 1080;
static const int windowHeight = HD;
static const int windowWidth = windowHeight / 9 * 16;


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

	// �[�x�X�e���V���쐬
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::Button button;
	if (button.Create(mainWindow, L"��]ON/OFF", 100, 50, 10, 10) == GUI::Result::FAIL)
	{
		return -1;
	}

	Model* model = nullptr;
	

	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			System::SafeDelete(&model);
			model = new Model(device);
			model->Load(fc.GetPath());
			if (model->IsSuccessLoad() == GUI::Result::FAIL)
			{
				GUI::ErrorBox(L"�Ή����Ă��Ȃ��t�@�C���ł�");
				System::SafeDelete(&model);
			}
		}

		command.BeginDraw();

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);

		command.ClearRenderTarget(GUI::Graphics::Color(0.3f, 0.3f, 0.3f));
		command.ClearDepthBuffer();


		if (model != nullptr)
		{
				model->SetDefaultSceneData();
				model->Draw(command);
		}
		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}

	System::SafeDelete(&model);
}

#include"System.h"
#include"GUI_Util.h"
using namespace std;

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"Model.h"

#define QuitIfFailed(func)\
if(func == GUI::Result::FAIL) \
{\
	DebugMessageFunctionError(func, MAIN);\
	return -1;\
}

static const int windowHeight = 1080;
static const int windowWidth = 1080;

const GUI::Graphics::Color clearColor(1.f, 1.f, 1.f);

#pragma comment( lib, "winmm.lib")

class Timer
{
public:
	Timer()
		:
		mInitTime((0xffffffff - timeGetTime()) - 6000),
		mPrevTime(0),
		mCurrentTime(0),
		mFrameTime(0),
		mFrameRate(0)
	{

	}
	unsigned int GetFrameTime()
	{
		return mFrameTime;
	}
	unsigned int GetFrameRate()
	{
		return mFrameRate;
	}
	void Update()
	{
		mCurrentTime = timeGetTime() + mInitTime;
		mFrameTime = mCurrentTime - mPrevTime;
		if (mFrameTime < 1)
		{
			DebugMessageWarning("The frame time is less than 1[ms]. The" << ToString(Timer::Update()) << " might have been Called more than twice at frame.");
		}

		mFrameRate = 1000.f / static_cast<float>(mFrameTime);

		mPrevTime = mCurrentTime;
	}
private:
	unsigned int mInitTime;
	unsigned int mPrevTime;
	unsigned int mCurrentTime;
	unsigned int mFrameTime;
	unsigned int mFrameRate;
};

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

	Timer time;

	unsigned int timeCounter = 0;
	unsigned int frameCounter = 0;
	const unsigned int waitTime = 5000;
	while (mainWindow.ProcessMessageNoWait() == GUI::Result::CONTINUE)
	{
		time.Update();
		timeCounter += time.GetFrameTime();
		frameCounter++;
		if (timeCounter > waitTime)
		{
			auto averageFPS = 
				static_cast<float>(frameCounter) / static_cast<float>(timeCounter/1000);

			DebugOutParam(time.GetFrameRate());
			DebugOutParam(averageFPS);

			frameCounter = 0;
			timeCounter %= waitTime;
		}

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
			model->Update();
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

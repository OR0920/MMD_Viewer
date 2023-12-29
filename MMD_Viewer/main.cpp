#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"VertexShader2D.h"
#include"PixelShader2D.h"

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

	// ���[�g�V�O�l�`���쐬
	GUI::Graphics::RootSignature rootSignature;
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	// ���̓��C�A�E�g�쐬
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.SetDefaultPositionDesc();
	inputElementDesc.SetDefaultColorDesc();

	inputElementDesc.DebugOutLayout();

	// �p�C�v���C���X�e�[�g
	GUI::Graphics::GraphicsPipeline pipeline;
	pipeline.SetInputLayout(inputElementDesc);
	pipeline.SetRootSignature(rootSignature);
	pipeline.SetVertexShader(VertexShader2D, _countof(VertexShader2D));
	pipeline.SetPixelShader(PixelShader2D, _countof(PixelShader2D));

	if (device.CreateGraphicsPipeline(pipeline) == GUI::Result::FAIL)
	{
		return -1;
	}

	// ���_�o�b�t�@�쐬
	struct Vertex
	{
		MathUtil::float3 position;
		MathUtil::float4 color;
	};

	Vertex triangle[] =
	{
		{ {  0.5f,  0.5f, 0.f }, { 1.f, 1.f, 0.f, 1.f } },
		{ {  0.5f, -0.5f, 0.f }, { 0.f, 1.f, 1.f, 1.f } },
		{ { -0.5f, -0.5f, 0.f }, { 1.f, 0.f, 1.f, 1.f } },
		{ { -0.5f,  0.5f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
	};

	int triangleIndex[] = { 0, 1, 2, 2, 3, 0 };

	auto triangleSize = sizeof(triangle);

	GUI::Graphics::VertexBuffer vertexBuffer;
	if (device.CreateVertexBuffer(vertexBuffer, sizeof(Vertex), _countof(triangle)) == GUI::Result::FAIL)
	{
		return -1;
	}

	if (vertexBuffer.Copy(triangle) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::Graphics::IndexBuffer indexBuffer;
	if (device.CreateIndexBuffer(indexBuffer, sizeof(int), _countof(triangleIndex)) == GUI::Result::FAIL)
	{
		return -1;
	}

	if (indexBuffer.Copy(triangleIndex) == GUI::Result::FAIL)
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

		command.BeginDraw(pipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);
		command.SetGraphicsRootSignature(rootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		//command.DrawTriangle(vertexBuffer);
		command.DrawTriangleList(vertexBuffer, indexBuffer);

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

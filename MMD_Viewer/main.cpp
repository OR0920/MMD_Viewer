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


	// ���̓��C�A�E�g�쐬
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.SetDefaultPositionDesc();
	inputElementDesc.SetDefaultColorDesc();

	inputElementDesc.DebugOutLayout();


	// ���_�o�b�t�@�쐬 �C���f�b�N�X�o�b�t�@�쐬
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
		{ { -0.5f,  0.5f, 0.f }, { 1.f, 1.f, 1.f, 1.f } },
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

	// �萔�o�b�t�@�p�̃f�B�X�N���v�^�q�[�v�쐬
	const int descriptorCount = 2;
	GUI::Graphics::DescriptorHeapForShaderData descHeap;
	{
		auto result = device.CreateDescriptorHeap(descHeap, descriptorCount);
		if (result == GUI::Result::FAIL)
		{
			return -1;
		}
	}

	// �萔�o�b�t�@�쐬

	struct Transform
	{
		MathUtil::Matrix translation;
		MathUtil::Matrix rotation;
	};

	GUI::Graphics::ConstantBuffer transform;
	{
		auto result = device.CreateConstantBuffer
		(
			transform,
			descHeap,
			sizeof(Transform)
		);

		if (result == GUI::Result::FAIL)
		{
			return -1;
		}
	}

	Transform* mappedTransform = nullptr;
	if (transform.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
	{
		mappedTransform->translation = MathUtil::Matrix::GenerateMatrixTranslation(MathUtil::Vector(0.5f, 0.f, 0.f));
		mappedTransform->rotation = MathUtil::Matrix::GenerateMatrixRotationZ(MathUtil::DegreeToRadian(45.f));
	}

	struct Color
	{
		GUI::Graphics::Color color;
	};
	
	GUI::Graphics::ConstantBuffer color;
	if (device.CreateConstantBuffer(color, descHeap, sizeof(Color)) == GUI::Result::FAIL)
	{
		return -1;
	}

	Color* mappedColor;
	if (color.Map(reinterpret_cast<void**>(&mappedColor)) == GUI::Result::SUCCESS)
	{
		mappedColor->color = GUI::Graphics::Color(1.f, 0.f, 0.f);
	}
	
	// ���[�g�V�O�l�`���쐬
	GUI::Graphics::RootSignature rootSignature;
	rootSignature.SetParameterCount(2);
	rootSignature.SetParamForCBV(0, 0);
	rootSignature.SetParamForCBV(1, 1);
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

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

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
		}

		command.BeginDraw();

		command.SetGraphicsPipeline(pipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);
		command.SetGraphicsRootSignature(rootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		command.SetDescriptorHeap(descHeap);
		command.SetConstantBuffer(transform, 0);
		command.SetConstantBuffer(color, 1);

		//command.DrawTriangle(vertexBuffer);
		command.DrawTriangleList(vertexBuffer, indexBuffer);

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

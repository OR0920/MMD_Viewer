#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
#include"GUI_Util.h"
using namespace std;

#include"VertexShader.h"
#include"PixelShader.h"

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


	// ���̓��C�A�E�g�쐬
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.SetDefaultPositionDesc();
	inputElementDesc.SetDefaultColorDesc();

	inputElementDesc.DebugOutLayout();

	GUI::Graphics::InputElementDesc mmdInputLayout;
	mmdInputLayout.SetElementCount(2);
	mmdInputLayout.SetDefaultPositionDesc();
	mmdInputLayout.SetDefaultNormalDesc();

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
		MathUtil::Matrix world;
		MathUtil::Matrix view;
		MathUtil::Matrix proj;
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
		mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
		mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
		(
			MathUtil::Vector(0.f, 10.f, -50.f),
			MathUtil::Vector::basicZ,
			MathUtil::Vector::basicY
		);
		mappedTransform->proj = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
		(
			DirectX::XM_PIDIV4,
			static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
			0.1f,
			1000.f
		);
	}

	struct PixelShaderData
	{
		MathUtil::float3 lightDir;
	};

	GUI::Graphics::ConstantBuffer psData;
	if (device.CreateConstantBuffer(psData, descHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
	{
		return -1;
	}

	PixelShaderData* mappedPS_Data = nullptr;
	if (psData.Map(reinterpret_cast<void**>(&mappedPS_Data)) == GUI::Result::SUCCESS)
	{
		mappedPS_Data->lightDir = MathUtil::Vector(-1.f, -1.f, 1.f).GetFloat3();
	}

	// ���[�g�V�O�l�`���쐬
	GUI::Graphics::RootSignature mmdRootSignature;
	mmdRootSignature.SetParameterCount(2);
	mmdRootSignature.SetParamForCBV(0, 0);
	mmdRootSignature.SetParamForCBV(1, 1);
	if (device.CreateRootSignature(mmdRootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	// �p�C�v���C���X�e�[�g
	GUI::Graphics::GraphicsPipeline mmdPipeline;
	mmdPipeline.SetDepthEnable();
	mmdPipeline.SetAlphaEnable();
	mmdPipeline.SetInputLayout(mmdInputLayout);
	mmdPipeline.SetRootSignature(mmdRootSignature);
	mmdPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mmdPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	if (device.CreateGraphicsPipeline(mmdPipeline) == GUI::Result::FAIL)
	{
		return -1;
	}

	Model model(device);

	while (mainWindow.ProcessMessage() == GUI::Result::CONTINUE)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());

			model.Load(fc.GetPath());
		}

		command.BeginDraw();

		command.SetGraphicsPipeline(mmdPipeline);

		command.UnlockRenderTarget(renderTarget);

		command.SetRenderTarget(renderTarget, depthStencil);
		command.SetGraphicsRootSignature(mmdRootSignature);

		command.ClearRenderTarget(GUI::Graphics::Color(0.5f, 0.5f, 0.5f));
		command.ClearDepthBuffer();

		command.SetDescriptorHeap(descHeap);
		command.SetConstantBuffer(transform, 0);
		command.SetConstantBuffer(psData, 1);

		if (model.IsSuccessLoad() == GUI::Result::SUCCESS)
		{
			command.DrawTriangleList(model.GetVB(), model.GetIB());
		}

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

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

static const int HD = 720;
static const int FHD = 1080;
static const int windowHeight = HD;
static const int windowWidth = windowHeight / 9 * 16;

class Model
{
public:

	struct ModelVertex
	{
		MathUtil::float3 position;
		MathUtil::float3 normal;
	};

	Model(GUI::Graphics::Device& device); ~Model();

	GUI::Result	Load(const char* const filepath);
	GUI::Result IsSuccessLoad() const;
	const GUI::Graphics::VertexBuffer& GetVB() const;
	const GUI::Graphics::IndexBuffer& GetIB() const;
private:
	GUI::Graphics::Device& mDevice;
	GUI::Graphics::VertexBuffer mVB;
	GUI::Graphics::IndexBuffer mIB;

	GUI::Result LoadPMD(const char* const filepath);
	GUI::Result LoadPMX(const char* const filepath);
	GUI::Result isSuccessLoad;
};

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mVB(),
	mIB(),
	isSuccessLoad(GUI::Result::FAIL)
{

}

Model::~Model() {}


GUI::Result Model::Load(const char* const filepath)
{
	isSuccessLoad = GUI::Result::FAIL;

	if (LoadPMD(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}
	else if (LoadPMX(filepath) == GUI::Result::SUCCESS)
	{
		isSuccessLoad = GUI::Result::SUCCESS;
	}
	
	return isSuccessLoad;
}

GUI::Result Model::IsSuccessLoad() const
{
	return isSuccessLoad;
}

const GUI::Graphics::VertexBuffer& Model::GetVB() const
{
	return mVB;
}

const GUI::Graphics::IndexBuffer& Model::GetIB() const
{
	return mIB;
}


GUI::Result Model::LoadPMD(const char* const filepath)
{
	MMDsdk::PmdFile file(filepath);
	if (file.IsSuccessLoad() == true)
	{
		auto vCount = file.GetVertexCount();
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			auto& v = file.GetVertex(i);
			mesh[i].position = System::strong_cast<MathUtil::float3>(v.position);
			mesh[i].normal = System::strong_cast<MathUtil::float3>(v.normal);
		}

		if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		};

		if (mVB.Copy(mesh) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&mesh);

		auto iCount = file.GetIndexCount();
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (mDevice.CreateIndexBuffer(mIB, sizeof(int), iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		if (mIB.Copy(index) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);

		return GUI::Result::SUCCESS;
	}
	else
	{
		return GUI::Result::FAIL;
	}
}


GUI::Result Model::LoadPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);
	if (file.IsSuccessLoad() == true)
	{
		auto vCount = file.GetVertexCount();
		ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			auto& v = file.GetVertex(i);
			mesh[i].position = System::strong_cast<MathUtil::float3>(v.position);
			mesh[i].normal = System::strong_cast<MathUtil::float3>(v.normal);
		}

		if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		};

		if (mVB.Copy(mesh) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&mesh);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&mesh);

		auto iCount = file.GetIndexCount();
		int* index = new int[iCount];
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		if (mDevice.CreateIndexBuffer(mIB, sizeof(int), iCount) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		if (mIB.Copy(index) == GUI::Result::FAIL)
		{
			System::SafeDeleteArray(&index);
			return GUI::Result::FAIL;
		}

		System::SafeDeleteArray(&index);

		return GUI::Result::SUCCESS;
	}
	else
	{
		return GUI::Result::FAIL;
	}
}


int MAIN()
{
	// デバッグ表示を日本語に対応させる
	SET_JAPANESE_ENABLE;

	// メモリリークのデバッグを有効化
	System::CheckMemoryLeak();

	//　メインウィンドウのインスタンスを取得
	auto& mainWindow = GUI::MainWindow::Instance();

	// ウィンドウを作成
	if (mainWindow.Create(windowWidth, windowHeight) == GUI::Result::FAIL)
	{
		return -1;
	}

	// ファイル取得ウィンドウの初期化
	auto& fc = GUI::FileCatcher::Instance();

	if (fc.Create(mainWindow) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 描画エンジン初期化

	// デバッグモード　有効化
	if (GUI::Graphics::EnalbleDebugLayer() == GUI::Result::FAIL)
	{
		return -1;
	}

	// デバイス作成
	GUI::Graphics::Device device;
	if (device.Create() == GUI::Result::FAIL)
	{
		return -1;
	}

	// コマンドオブジェクト作成
	GUI::Graphics::GraphicsCommand command;
	if (device.CreateGraphicsCommand(command) == GUI::Result::FAIL)
	{
		return -1;
	}

	// スワップチェイン作成

	GUI::Graphics::SwapChain swapChain;
	if (swapChain.Create(command, mainWindow, 2) == GUI::Result::FAIL)
	{
		return -1;
	}

	// レンダーターゲット作成
	GUI::Graphics::RenderTarget renderTarget;
	if (device.CreateRenderTarget(renderTarget, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}

	// 深度ステンシル作成
	GUI::Graphics::DepthStencilBuffer depthStencil;
	if (device.CreateDepthBuffer(depthStencil, swapChain) == GUI::Result::FAIL)
	{
		return -1;
	}


	// 入力レイアウト作成
	GUI::Graphics::InputElementDesc inputElementDesc;
	inputElementDesc.SetElementCount(2);
	inputElementDesc.SetDefaultPositionDesc();
	inputElementDesc.SetDefaultColorDesc();

	inputElementDesc.DebugOutLayout();

	GUI::Graphics::InputElementDesc mmdInputLayout;
	mmdInputLayout.SetElementCount(2);
	mmdInputLayout.SetDefaultPositionDesc();
	mmdInputLayout.SetDefaultNormalDesc();

	// 頂点バッファ作成 インデックスバッファ作成
	struct Vertex
	{
		MathUtil::float3 position;
		MathUtil::float4 color;
	};

	Vertex triangle[] =
	{
		{ {  1.f,  1.f, 0.f }, { 1.f, 1.f, 0.f, 1.f } },
		{ {  1.f, -1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f } },
		{ { -1.f, -1.f, 0.f }, { 1.f, 0.f, 1.f, 1.f } },
		{ { -1.f,  1.f, 0.f }, { 1.f, 1.f, 1.f, 1.f } },
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


	// 定数バッファ用のディスクリプタヒープ作成
	const int descriptorCount = 2;
	GUI::Graphics::DescriptorHeapForShaderData descHeap;
	{
		auto result = device.CreateDescriptorHeap(descHeap, descriptorCount);
		if (result == GUI::Result::FAIL)
		{
			return -1;
		}
	}

	// 定数バッファ作成

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

	// ルートシグネチャ作成
	GUI::Graphics::RootSignature rootSignature;
	rootSignature.SetParameterCount(2);
	rootSignature.SetParamForCBV(0, 0);
	rootSignature.SetParamForCBV(1, 1);
	if (device.CreateRootSignature(rootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::Graphics::RootSignature mmdRootSignature;
	mmdRootSignature.SetParameterCount(1);
	mmdRootSignature.SetParamForCBV(0, 0);
	if (device.CreateRootSignature(mmdRootSignature) == GUI::Result::FAIL)
	{
		return -1;
	}

	// パイプラインステート
	GUI::Graphics::GraphicsPipeline pipeline;
	pipeline.SetInputLayout(inputElementDesc);
	pipeline.SetRootSignature(rootSignature);
	pipeline.SetVertexShader(VertexShader, _countof(VertexShader));
	pipeline.SetPixelShader(PixelShader, _countof(PixelShader));

	if (device.CreateGraphicsPipeline(pipeline) == GUI::Result::FAIL)
	{
		return -1;
	}

	GUI::Graphics::GraphicsPipeline mmdPipeline;
	mmdPipeline.SetDepthEnable();
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

		//command.DrawTriangle(vertexBuffer);
		//command.DrawTriangleList(vertexBuffer, indexBuffer);
		if (model.IsSuccessLoad() == GUI::Result::SUCCESS)
		{
			command.DrawTriangleList(model.GetVB(), model.GetIB());
		}

		command.LockRenderTarget(renderTarget);

		command.EndDraw();

		swapChain.Present();
	}
}

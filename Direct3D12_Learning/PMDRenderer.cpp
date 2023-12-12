#include"Dx12Wrapper.h"
#include"PMDRenderer.h"

#include<cassert>

#include<d3dcompiler.h>

#include"System.h"
#include"d3dx12.h"

#define CallInitFunctionWithAssert(func)\
if (FAILED(func))\
{\
	DebugMessageFunctionError(func, PMDRenderer::PMDRenderer());\
	assert(false);\
	return;\
}


PMDRenderer::PMDRenderer(Dx12Wrapper& argDx12)
	:
	mDx12(argDx12)
{
	CallInitFunctionWithAssert(CreateRootSignature());
	CallInitFunctionWithAssert(CreateGraphicsPipelineForPMD());
	mDefaultTextureWhite = CreateWhiteTexture();
	mDefaultTextureBlack = CreateBlackTexture();
	mDefaultTextureGrad = CreateGrayGradTexture();
}

HRESULT PMDRenderer::CreateRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE descTableRange[4] = {};
	descTableRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	descTableRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	descTableRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	descTableRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);

	CD3DX12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].InitAsDescriptorTable(1, &descTableRange[0]);

	rootParams[1].InitAsDescriptorTable(1, &descTableRange[1]);

	rootParams[2].InitAsDescriptorTable(2, &descTableRange[2]);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0].Init(0);
	samplerDesc[1].Init
	(
		1,
		D3D12_FILTER::D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init
	(
		3,
		rootParams,
		2,
		samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ComPtr<ID3DBlob> rootSigunatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	auto result = D3D12SerializeRootSignature
	(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1,
		rootSigunatureBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(D3D12SerializeRootSignature(), PMDRenderer::CreateRootSignature());
		DebugOutParamHex(result);
		assert(SUCCEEDED(result));
		return result;
	}

	result = mDx12.GetDevice()->CreateRootSignature
	(
		0,
		rootSigunatureBlob->GetBufferPointer(),
		rootSigunatureBlob->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateRootSignature(), PMDRenderer::CreateRootSignature());
		assert(SUCCEEDED(result));
		return result;
	}
	return result;
}

HRESULT PMDRenderer::CreateGraphicsPipelineForPMD()
{
	ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
	ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	auto result = D3DCompileFromFile
	(
		L"BasicVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		vertexShaderBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);
	if (CheckShaderCompilieResult(result, errorBlob) == false)
	{
		DebugMessage("Vertex Shader Compile Failed !");
		assert(SUCCEEDED(result));
		return result;
	}

	result = D3DCompileFromFile
	(
		L"BasicPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		pixelShaderBlob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);
	if (CheckShaderCompilieResult(result, errorBlob) == false)
	{
		DebugMessage("Pixel Shader Compile Failed !");
		assert(SUCCEEDED(result));
		return result;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		//{ "WEIGHT",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		//{ "EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = mRootSignature.Get();


	pso.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pso.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());


	pso.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


	pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso.BlendState.AlphaToCoverageEnable = true;

	pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;


	pso.DepthStencilState.DepthEnable = true;
	pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	pso.DepthStencilState.StencilEnable = false;


	pso.InputLayout.pInputElementDescs = inputLayout;
	pso.InputLayout.NumElements = _countof(inputLayout);


	pso.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;


	pso.SampleDesc.Count = 1;
	pso.SampleDesc.Quality = 0;


	result = mDx12.GetDevice()->
		CreateGraphicsPipelineState
		(
			&pso,
			IID_PPV_ARGS(mPipeLine.ReleaseAndGetAddressOf())
		);
	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()->CreateGraphicsPipelineState(),
			PMDRenderer::CreateGraphicsPipelineForPMD()
		);
		DebugOutParamHex(result);
		assert(SUCCEEDED(result));
		return result;
	}

	return result;
}

bool PMDRenderer::CheckShaderCompilieResult(HRESULT argResult, ComPtr<ID3DBlob> argError)
{
	if (FAILED(argResult))
	{
		if (argResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			DebugMessage("Shader Source File is Not Fount !");
		}
		else
		{
			/*std::string errStr;
			errStr.resize(argError->GetBufferSize());
			std::copy_n(reinterpret_cast<char*>(argError->GetBufferPointer()), argError->GetBufferPointer(), errStr.begin());
			errStr += "\n";
			DebugMessage(errStr);*/
		}
		return false;
	}
	else
	{
		return true;
	}
}


ComPtr<ID3D12Resource> PMDRenderer::CreateWhiteTexture()
{
	ComPtr<ID3D12Resource> whiteBuff = CreateDefaultTexture(4, 4);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);

	auto result =
		whiteBuff->WriteToSubresource
		(
			0,
			nullptr,
			data.data(),
			4 * 4,
			data.size()
		);

	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			whiteBuff->WriteToSubresource(),
			PMDRenderer::CreateWhiteTexture()
		);

		assert(SUCCEEDED(result));
		return nullptr;
	}

	return whiteBuff;
}

ComPtr<ID3D12Resource> PMDRenderer::CreateBlackTexture()
{
	auto blackBuff = CreateDefaultTexture(4, 4);
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0x00);

	auto result = blackBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	if (FAILED(result))
	{
		DebugMessageFunctionError(blackBuff->WriteToSubresource(), PMDRenderer::CreateBlackTexture());
		assert(SUCCEEDED(result));
		return nullptr;
	}

	return blackBuff;
}
ComPtr<ID3D12Resource> PMDRenderer::CreateGrayGradTexture() {
	return CreateWhiteTexture();

	//auto gradBuff = CreateDefaultTexture(4, 256);
	////上が白くて下が黒いテクスチャデータを作成
	//std::vector<unsigned int> data(4 * 256);
	//auto it = data.begin();
	//unsigned int c = 0xff;
	//for (; it != data.end(); it += 4) {
	//	auto col = (0xff << 24);
	//	if (c > 0xff / 16)
	//	{
	//		col |= RGB(0xff, 0xff, 0xff);//RGBAが逆並びしているためRGBマクロと0xff<<24を用いて表す;
	//	}
	//	else
	//	{
	//		col |= RGB(0xff * 0.8, 0xff * 0.8, 0xff * 0.8);
	//	}
	//	std::fill(it, it + 4, col);
	//	--c;
	//}

	//auto result = gradBuff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int), sizeof(unsigned int) * data.size());
	//if (FAILED(result))
	//{
	//	DebugMessageFunctionError(gradBuff->WriteToSubresource(), PMDRenderer::CreateGrayGradTexture());
	//	assert(SUCCEEDED(result));
	//	return nullptr;
	//}

	//return gradBuff;
}

ComPtr<ID3D12Resource> PMDRenderer::CreateDefaultTexture
(
	size_t argWidth,
	size_t argHeight
)
{
	auto resDesc =
		CD3DX12_RESOURCE_DESC::Tex2D
		(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			argWidth,
			argHeight
		);
	auto texHeapProp =
		CD3DX12_HEAP_PROPERTIES
		(
			D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0
		);

	ComPtr<ID3D12Resource> buff = nullptr;

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(buff.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()->CreateCommittedResource(),
			PMDRenderer::CreateDefaultTexture()
		);
		assert(SUCCEEDED(result));
		return nullptr;
	}

	return buff;
}

ComPtr<ID3D12PipelineState> PMDRenderer::GetPipelineState()
{
	return mPipeLine;
}

ComPtr<ID3D12RootSignature> PMDRenderer::GetRootSignature()
{
	return mRootSignature;
}

PMDRenderer::~PMDRenderer()
{

}

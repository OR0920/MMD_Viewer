#pragma once

#include<D3D12.h>

#include"UsingComPtr.h"

class Dx12Wrapper;
class PMDActor;

class PMDRenderer
{
	friend PMDActor;

public:
	PMDRenderer(Dx12Wrapper& argDx12);
	~PMDRenderer();
	void Update();
	void Clear();

	ComPtr<ID3D12PipelineState> GetPipelineState();
	ComPtr<ID3D12RootSignature> GetRootSignature();

private:
	Dx12Wrapper& mDx12;

	ComPtr<ID3D12PipelineState> mPipeLine;
	ComPtr<ID3D12RootSignature> mRootSignature;


	ComPtr<ID3D12Resource> mDefaultTextureWhite;
	ComPtr<ID3D12Resource> mDefaultTextureBlack;
	ComPtr<ID3D12Resource> mDefaultTextureGrad;


	ComPtr<ID3D12Resource> CreateDefaultTexture(size_t argWidth, size_t argHeight);
	ComPtr<ID3D12Resource> CreateWhiteTexture();
	ComPtr<ID3D12Resource> CreateBlackTexture();
	ComPtr<ID3D12Resource> CreateGrayGradTexture();

	HRESULT CreateGraphicsPipelineForPMD();
	HRESULT CreateRootSignature();

	bool CheckShaderCompilieResult(HRESULT argResult, ComPtr<ID3DBlob> argError = nullptr);
};



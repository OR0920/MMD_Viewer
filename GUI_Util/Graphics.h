#pragma once

// windows
#include<d3d12.h>
#include<dxgi1_6.h>

// other
#include"Result.h"

namespace GUI
{
	class ParentWindow;

	// D3D12�̔������b�p�[
	namespace Graphics
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		struct Color
		{
			float r = 0.f;
			float g = 0.f;
			float b = 0.f;
			float a = 0.f;

			Color
			(
				float _r,
				float _g,
				float _b,
				float _a = 1.f
			);

			Color();
		};

		enum Format : unsigned int
		{
			COLOR_8_4 = 0,	// �F 32bitColor 
			FLOAT_32_4,		// float4
			FORMAT_COUNT
		};

		// �f�o�b�O���[�h��L���ɂ���
		Result EnalbleDebugLayer();

		class GraphicsCommand;
		class SwapChain;
		class RenderTarget;
		class DepthStencilBuffer;
		class RootSignature;
		class InputElementDesc;
		class GraphicsPipeline;
		class VertexBuffer;
		class IndexBuffer;
		class SignateBuffer;
		class ConstantBuffer;
		class Texture2D;


		class DescriptorHeap;

		// �f�o�C�X
		// �f�t�H���g�̃O���t�B�b�N�f�o�C�X���w�肳���
		class Device
		{
		public:
			Device(); ~Device();

			Result Create();

			// �e��C���^�[�t�F�C�X�����֐�
			Result CreateGraphicsCommand(GraphicsCommand& graphicsCommand);

			// �����_�[�^�[�Q�b�g�쐬
			Result CreateRenderTarget
			(
				RenderTarget& renderTarget,
				const SwapChain& swapChain
			);


			// �[�x�o�b�t�@�̂ݎg�p����ꍇ
			Result CreateDepthBuffer
			(
				DepthStencilBuffer& depthStencilBuffer,
				const SwapChain& swapChain
			);

			Result CreateRootSignature(RootSignature& rootSignature);
			Result CreateGraphicsPipeline(GraphicsPipeline& pipeline);

			// ���_�o�b�t�@�𐶐�����
			// vertexBuffer		: �o��	: ���_�o�b�t�@�̃C���^�[�t�F�C�X
			// vertexTypeSize	: ����	: ���_�\���̂̃T�C�Y
			// vertexCount		: ����	: ���_��
			Result CreateVertexBuffer
			(
				VertexBuffer& vertexBuffer,
				const unsigned int vertexTypeSize,
				const unsigned int vertexCount
			);

			// �C���f�b�N�X�o�b�t�@�𐶐�����
			// indexBuffer		: �o��	: �C���f�b�N�X�o�b�t�@�̃C���^�[�t�F�C�X
			// indexTypeSize	: ����	: ��̃C���f�b�N�X�̃T�C�Y
			// indexCount		: ����	: �C���f�b�N�X�̐�
			Result CreateIndexBuffer
			(
				IndexBuffer& indexBuffer,
				const unsigned int indexTypeSize,
				const unsigned int indexCount
			);

			// �萔�o�b�t�@�𐶐�����
			// constantBuffer	: �o��		: �萔�o�b�t�@�̃C���^�[�t�F�C�X 
			// viewHeap			: ���́E�o��	: �萔�o�b�t�@�̏����i�[����̈�
			// bufferStructSize	: ����		: �萔�f�[�^��̃T�C�Y
			// bufferCount		: ����		: �f�[�^�� 
			Result CreateConstantBuffer
			(
				ConstantBuffer& constantBuffer,
				DescriptorHeap& viewHeap,
				const unsigned int bufferStructSize,
				const unsigned int bufferCount = 1
			);

			Result CreateTexture2D
			(
				Texture2D& texture,
				DescriptorHeap& viewHeap
			);

			// �萔�o�b�t�@�E�e�N�X�`���p�̃f�B�X�N���v�^�q�[�v
			// heap				: �o��	: �q�[�v�̃C���^�[�t�F�C�X
			// resouceCount		: ����	: ��x�Ƀo�C���h�����萔�o�b�t�@�E�e�N�X�`���̐�
			Result CreateDescriptorHeap
			(
				DescriptorHeap& heap,
				const unsigned int resourceCount
			);
		private:
			ComPtr<ID3D12Device> mDevice;
		};

		// �_�u���o�b�t�@�ȏ�̃����_�[�^�[�Q�b�g���Ǘ�����
		class SwapChain
		{
		public:
			SwapChain(); ~SwapChain();

			// �w�肵���E�B���h�E�ɕ`�悳���
			// command		: ����	: �R�}���h�̃C���^�[�t�F�C�X 
			// targetWindow	: ����	: �`���
			// frameCount	: ����	: �o�b�t�@��
			Result Create
			(
				GraphicsCommand& command,
				const ParentWindow& targetWindow,
				const int frameCount
			);

			const float GetAspectRatio() const;

			// ���݂̃o�b�N�o�b�t�@���擾����ۂɎg�p����
			// 0 <= return < frameCount �̒l���Ԃ����
			int GetCurrentBackBufferIndex() const;

			// �o�b�t�@�����ւ���
			void Present();

			// ���C�u��������Ăяo���֐�
			Result GetDesc(DXGI_SWAP_CHAIN_DESC* desc) const;
			Result GetBuffer(const unsigned int bufferID, ID3D12Resource** resource) const;
		private:
			ComPtr<IDXGISwapChain4> mSwapChain;

			float mAspectRatio;
		};

		// �`��R�}���h�𔭍s����
		class GraphicsCommand
		{
			// ���ÂɃQ�b�^�[����肽���Ȃ��̂Ńt�����h��
			friend Result Device::CreateGraphicsCommand(GraphicsCommand&);
			friend Result SwapChain::Create
			(
				GraphicsCommand&,
				const ParentWindow&,
				const int
			);
		public:
			GraphicsCommand(); ~GraphicsCommand();

			// �`��J�n���ɌĂ�
			void BeginDraw();

			// �p�C�v���C�����Z�b�g
			void SetGraphicsPipeline(const GraphicsPipeline& pipeline);

			// �����_�[�^�[�Q�b�g���������݉\�ɂ���
			void UnlockRenderTarget(const RenderTarget& renderTarget);

			// �r���[�|�[�g�ƃV�U�[��`���Z�b�g����B
			void SetViewportAndRect(const RenderTarget& renderTarget);

			// �����_�[�^�[�Q�b�g���Z�b�g����
			void SetRenderTarget
			(
				const RenderTarget& renderTarget
			);

			// �����_�[�^�[�Q�b�g�Ɛ[�x�o�b�t�@���Z�b�g����
			void SetRenderTarget
			(
				const RenderTarget& renderTarget,
				const DepthStencilBuffer& depthStenilBuffer
			);

			// �����_�[�^�[�Q�b�g��h��Ԃ�
			// �f�t�H���g�̓O���[
			void ClearRenderTarget(const Color& color);

			// �[�x�o�b�t�@������������
			void ClearDepthBuffer();

			// �萔�o�b�t�@�A�e�N�X�`�������o�C���h����
			// ���[�g�V�O�l�`���Őݒ肵���l�ƑΉ�������K�v������
			void SetGraphicsRootSignature(const RootSignature& rootSignature);
			void SetDescriptorHeap(const DescriptorHeap& descHeap);
			void SetConstantBuffer
			(
				const ConstantBuffer& constBuffer,
				const int rootParamID,
				const int bufferID = 0
			);

			void SetDescriptorTable
			(
				const SignateBuffer& buffer,
				const int rootParamID,
				const int bufferID = 0
			);

			// ���_�݂̂ŕ`��
			void SetVertexBuffer
			(
				const VertexBuffer& vertex
			);
			void DrawTriangle(const int vertexCount);

			// ���_�ƃC���f�b�N�X�ŕ`��
			void SetVertexBuffer
			(
				const VertexBuffer& vertex,
				const IndexBuffer& intdex
			);
			void DrawTriangleList
			(
				const int indexCount, const int offs
			);

			// �����_�[�^�[�Q�b�g���������ݕs�ɂ���
			void LockRenderTarget(const RenderTarget& renderTarget);

			// �`��I���ɌĂяo��
			void EndDraw();

			// �o�b�N�o�b�t�@�ɐ؂�ւ�
			void Flip();
		private:

			ID3D12Device* mDevice;
			SwapChain* mSwapChain;

			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;

			D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle;
			D3D12_CPU_DESCRIPTOR_HANDLE mDSV_Handle;

			ComPtr<ID3D12Fence> mFence;
			UINT64 mFenceValue;
		};

		// �`�����Ǘ�����
		// �X���b�v�`�F�C������A�`���̗̈�����炤
		// ���[�U�[�����烁���o���Ăяo���K�v�͂Ȃ�
		class RenderTarget
		{
			friend Result Device::CreateRenderTarget(RenderTarget&, const SwapChain&);

		public:
			RenderTarget(); ~RenderTarget();

			const float GetAspectRatio() const;

			// ���C�u��������Ăяo���֐�
			const D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(const int bufferID) const;
			const ComPtr<ID3D12Resource> GetRenderTargetResource(const int bufferID) const;

			const DXGI_FORMAT GetFormat() const;
			const int GetWidth() const;
			const int GetHeight() const;

			const D3D12_VIEWPORT& GetViewPort() const;
			const D3D12_RECT& GetRect() const;
		private:
			ComPtr<ID3D12DescriptorHeap> mHeaps;
			ComPtr<ID3D12Resource>* mResource;
			int mBufferCount;

			int mViewIncrementSize;

			D3D12_VIEWPORT mViewPort;
			D3D12_RECT mScissorRect;
			float mAspectRatio;
		};

		// �[�x�o�b�t�@
		// �����_�[�^�[�Q�b�g���l�A���[�U�[���͂��̃C���^�[�t�F�C�X�𐶐����邾���ł悢
		class DepthStencilBuffer
		{
			friend Result Device::CreateDepthBuffer
			(
				DepthStencilBuffer&, const SwapChain&
			);
		public:
			DepthStencilBuffer(); ~DepthStencilBuffer();

			// ���C�u��������Ăяo���֐�
			const D3D12_CPU_DESCRIPTOR_HANDLE& GetDescriptorHandle() const;
		private:
			ComPtr<ID3D12Resource> mDSB_Resource;
			ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
		};

		// ���[�g�V�O�l�`������̃C���^�[�t�F�C�X
		// �Q�l: https://learn.microsoft.com/ja-jp/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12A
		// 
		// �p�C�v���C���ɓn�����f�[�^(���\�[�X)��4��ނŁA���ׂăr���[����ēn�����
		// 
		// 1.���̓A�Z���u��(IA)�ɓn��������
		//		�E���_�o�b�t�@
		//		�E�C���f�b�N�X�o�b�t�@		(�Ȃ��Ă��悢)
		// 2.�o�̓}�[�W���[(OM)�ɓn��������
		//		�E�����_�[�^�[�Q�b�g
		//		�E�[�x�X�e���V���o�b�t�@		(�Ȃ��Ă��悢)
		// 3.�X�g���[���o��(SO)�̏������ݐ�	(�Ȃ��Ă��悢)
		// 
		// 4.���[�g�����ɓn��������			(�Ȃ��Ă��悢�E�ő�64��)
		//		�E�萔										(���[�g�萔)
		//		�E�萔�o�b�t�@�A�e�N�X�`�����̃f�B�X�N���v�^	(���[�g�f�B�X�N���v�^)
		//		�E�����̃f�B�X�N���v�^���܂Ƃ߂��e�[�u��		(�f�B�X�N���v�^�e�[�u��)
		// 
		// ���̂���4���ǂ̂悤�Ɏg�p���邩���L�q�������̂����[�g�V�O�l�`��
		//

		// �f�B�X�N���v�^�e�[�u���̐ݒ���s��
		class DescriptorRange
		{
		public:
			DescriptorRange(); ~DescriptorRange();

			// �e�[�u����������
			void SetRangeCount(const int rangeCount);
			// ���Ԗڂ̃e�[�u�����A���Ԃ̃��W�X�^�ɑΉ����A�f�B�X�N���v�^���������邩
			void SetRangeForCBV(const int rangeID, const int registerID, const int descriptorCount);
			void SetRangeForSRV(const int rangeID, const int registerID, const int descriptorCount);

			// ���C�u��������Ăяo���֐�
			int GetRangeCount() const;
			const D3D12_DESCRIPTOR_RANGE* const GetRange() const;
		private:
			D3D12_DESCRIPTOR_RANGE* mRange;
			int mRangeCount;
		};

		// ���[�g�V�O�l�`���{��
		class RootSignature
		{
			friend Result Device::CreateRootSignature(RootSignature&);
		public:
			RootSignature(); ~RootSignature();

			// Create�O�ɌĂяo��

			// ���[�g�������������邩
			void SetParameterCount(const int count);

			// ���Ԗڂ̈����ɉ���n����
			// ���[�g�p�����[�^(����K�v�Ȃ��̂ŕK�v�ɂȂ�������)

			// ���[�g�f�B�X�N���v�^
			void SetParamForCBV(const int paramID, const int registerID);
			void SetParamForSRV(const int paramID, const int registerID);

			// �f�B�X�N���v�^�e�[�u��
			void SetParamForDescriptorTable
			(
				const int paramID,
				const DescriptorRange& range
			);

			// �T���v���[���������邩
			void SetStaticSamplerCount(const int count);
			void SetSamplerDefault(const int samplerID, const int registerID);
			void SetSamplerUV_Clamp(const int samplerID, const int registerID);

			// ���C�u��������Ăяo���֐�
			const ComPtr<ID3D12RootSignature> GetRootSignature() const;
		private:
			ComPtr<ID3D12RootSignature> mRootSignature;
			D3D12_ROOT_SIGNATURE_DESC mDesc;

			D3D12_ROOT_PARAMETER* mRootParamter;
			D3D12_STATIC_SAMPLER_DESC* mSamplerDesc;

			bool IsParamSizeOver(const int i) const;
			bool IsSamplerSizeOver(const int i) const;
		};

		// ���͒��_�̃��C�A�E�g���L�q����
		class InputElementDesc
		{
		public:
			InputElementDesc(); ~InputElementDesc();

			// �\���̗̂v�f��
			void SetElementCount(const int count);

			// �ʒu
			void SetDefaultPositionDesc(const char* const semantics = "POSITION");
			// �F
			void SetDefaultColorDesc(const char* const semantics = "COLOR");
			// �@��
			void SetDefaultNormalDesc(const char* const semantics = "NORMAL");
			// UV
			void SetDefaultUV_Desc(const char* const semantics = "UV");

			// �C�ӂ�4�o�C�g���������_
			void SetFloatParam(const char* const semantics);

			void DebugOutLayout() const;

			// ���C�u�������Ăяo���֐�
			int GetDescCount() const;
			const D3D12_INPUT_ELEMENT_DESC* const GetElementDesc() const;
		private:
			bool IsSizeOver() const;

			int mCount;
			int mLastID;
			D3D12_INPUT_ELEMENT_DESC* mInputElementDesc;
		};

		// �p�C�v���C���S�̂̐ݒ�
		class GraphicsPipeline
		{
			friend Result Device::CreateGraphicsPipeline(GraphicsPipeline&);
		public:
			GraphicsPipeline(); ~GraphicsPipeline();

			// Create�O�ɌĂяo��


			// �[�x�o�b�t�@�L����
			void SetDepthEnable();
			// �X�e���V�����g���ꍇ�B�ʂ�API�����

			// ���ߗL�����@�Ăяo���Ȃ��ꍇ����
			void SetAlphaEnable();

			// �J�����O�������@�Ăяo���Ȃ��ꍇ�w�ʃJ�����O�L��
			void SetCullDisable();

			// �O�ʃJ�����O�L��
			void SetFrontCullEnable();

			// ���_���C�A�E�g
			void SetInputLayout(const InputElementDesc& inputElementDesc);
			// ���[�g�V�O�l�`��
			void SetRootSignature(const RootSignature& rootSignature);

			// �V�F�[�_�[�ւ̃|�C���^
			// �w�b�_�֏����o�������̂��o�C���h����ꍇ
			void SetVertexShader(const unsigned char* const vertexShader, const int length);
			void SetPixelShader(const unsigned char* const pixelShader, const int length);
#define SetShader(shader) shader, _countof(shader)


			// �t�@�C�����Ăяo���ꍇ(������)

			// ���C�u��������Ăяo���֐�
			const ComPtr<ID3D12PipelineState> GetPipelineState() const;
		private:
			ComPtr<ID3D12PipelineState> mPipelineState;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		};

		// ���_�o�b�t�@
		class VertexBuffer
		{
			friend Result Device::CreateVertexBuffer
			(
				VertexBuffer&,
				const unsigned int,
				const unsigned int
			);
		public:
			VertexBuffer(); ~VertexBuffer();

			// ���_�f�[�^���R�s�[
			Result Copy(const void* const data);
			const int GetVertexCount() const;

			// ���C�u��������Ăяo���֐�
			const D3D12_VERTEX_BUFFER_VIEW* const GetView() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_VERTEX_BUFFER_VIEW mView;
			int mVertexCount = 0;
		};

		// �C���f�b�N�X�o�b�t�@
		class IndexBuffer
		{
			friend Result Device::CreateIndexBuffer
			(
				IndexBuffer&,
				unsigned int,
				unsigned int
			);
		public:
			IndexBuffer(); ~IndexBuffer();

			Result Copy(const void* const data);
			const int GetIndexCount() const;

			// ���C�u��������Ăяo���֐�
			const D3D12_INDEX_BUFFER_VIEW* const GetView() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_INDEX_BUFFER_VIEW mView;
			int mIndexCount;

		};

		// �萔�o�b�t�@�A�e�N�X�`�����A���[�g�V�O�l�`���o�R�Ńo�C���h�����
		// ���\�[�X�̃C���^�[�t�F�[�X
		class SignateBuffer
		{
		public:
			virtual ~SignateBuffer();

			virtual const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const = 0;
		private:
		};

		// �萔�o�b�t�@
		class ConstantBuffer : public SignateBuffer
		{
			friend Result Device::CreateConstantBuffer
			(
				ConstantBuffer&,
				DescriptorHeap&,
				unsigned int, unsigned int
			);
		public:
			ConstantBuffer(); ~ConstantBuffer();

			// �\���̂���܂�Ȃ��̂Ń|�C���^�փ}�b�v
			// �A���C�������g�ɂ��萔�o�b�t�@�̍\���̂̃T�C�Y�ɂ�����炸
			// 256�o�C�g�P�ʂƂȂ��Ă��邽��
			// �萔�o�b�t�@�̍\���̂��̂��̂̔z��ł̓f�[�^�������
			// 
			// unsigned char* mappedData;
			// *reinterpret_cast<Buffer*>(mappedData) = srcData[i];
			// mappedData += (256�o�C�g�P�ʂփA���C�������g���ꂽ�o�b�t�@�̃T�C�Y);
			// 
			// ���̂悤�ɂ��Ȃ��ƃf�[�^�������B
			// �}�e���A�������������Ȃ����炱�����^������
			Result Map(void** ptr);
			void Unmap();

			const int GetBufferIncrementSize() const;

			//���C�u��������Ăяo���֐�
			const D3D12_GPU_VIRTUAL_ADDRESS GetGPU_Address(const int i = 0) const;
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_CONSTANT_BUFFER_VIEW_DESC mViewDesc;
			D3D12_CPU_DESCRIPTOR_HANDLE mCPU_Handle;
			D3D12_GPU_DESCRIPTOR_HANDLE mGPU_Handle;

			int mViewIncrementSize;
		};

		class TextureData;

		class Texture2D : public SignateBuffer
		{
			friend Result Device::CreateTexture2D
			(
				Texture2D&, DescriptorHeap&
			);
		public:
			Texture2D();
			~Texture2D();

			Result LoadFromFile(const wchar_t* const filePath);
			Result LoadFromFile(const char* const filepath);

			// ���C�u��������Ăяo���֐�
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_SHADER_RESOURCE_VIEW_DESC mViewDesc;
			D3D12_GPU_DESCRIPTOR_HANDLE mGPU_Handle;

			TextureData* mData;

			int mViewIncrementSize;

			Result WriteToSubresource();
		};

		// �萔�o�b�t�@�A�e�N�X�`���̃r���[���i�[����q�[�v
		// �`�撆�ɐ؂�ւ���Ə������d���Ȃ�炵���̂ŁA�ł�����菭�Ȃ��^�p���邱��
		class DescriptorHeap
		{
			friend Result Device::CreateDescriptorHeap
			(
				DescriptorHeap&,
				const unsigned int
			);
			friend Result Device::CreateConstantBuffer
			(
				ConstantBuffer&,
				DescriptorHeap&,
				const unsigned int,
				const unsigned int
			);
			friend Result Device::CreateTexture2D
			(
				Texture2D&,
				DescriptorHeap&
			);
		public:
			DescriptorHeap();
			~DescriptorHeap();

			// ���C�u��������Ăяo���֐�
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCPU_Handle();
			const D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGPU_Handle();

			const ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const;
		private:
			void MoveToNextHeapPos();

			ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
			int mDescriptorCount;
			int mViewIncrementSize;
			int mLastID;
		};
	}
}
// 
// GUI������API�̃��b�p���C�u����
// �K�v�ȋ@�\�݂̂�����
// ���܂�ėp�I�ɂ��������A�킩��₷�����ŏd��
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>
#include<vector>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>

// �G���g���|�C���g���B��
// �f�o�b�O�����R���\�[����
#ifdef _DEBUG
#define MAIN main 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG

// WIN32API�̃E�B���h�E��������b�v
namespace GUI
{
	// ���b�Z�[�W�@���@����
	// SUCCESS��FAIL��HRESULT�Ɠ���
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
		CONTINUE,
		QUIT,
	};

	// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
	// ������p�����Ă��Ȃ��E�B���h�E�͎q�E�B���h�E�����ĂȂ�
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetHandle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
	protected:
	};

	// ���C���E�B���h�E
	// �^�C�g���o�[�ƃ{�^���ނ����E�B���h�E
	class MainWindow : public ParentWindow
	{
	public:
		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// �T�C�Y���w�肵����
		Result Create(int width, int height);
		
		// ���b�Z�[�W����������
		// ����{�^������������false��Ԃ�
		// ���b�Z�[�W��҂�
		Result ProsessMessage();

		// ���b�Z�[�W��҂��Ȃ�
		Result ProcessMessageNoWait();

		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ���C�u����������Ăяo���֐�
		const HWND GetHandle() const;
	private:
		MainWindow();
		~MainWindow();

		int mWidth;
		int mHeight;

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};

	// �G���[�\���p�_�C�A���O�{�b�N�X
	void ErrorBox(const TCHAR* const message);

	// �̈���Ƀh���b�v���ꂽ�t�@�C����F������E�B���h�E
	class FileCatcher
	{
	public:
		// �V���O���g���p�֐�
		static FileCatcher& Instance();

		// �e���w�肵����
		// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
		// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
		Result Create(const ParentWindow& parent);

		bool Update();

		// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
		int GetLength() const;
		const char* const GetPath() const;

		// �t�@�C�����h���b�v���ꂽ�ꏊ
		struct DropPos
		{
			long x;
			long y;
		};
		const DropPos& GetDropPos() const;


		// �R�[���o�b�N�֐��B�@���[�U�[����͌Ăяo���Ȃ��B
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	private:
		// �R�[���o�b�N�֐�����p�X���擾����s����A
		// ��������Ɩʓ|�Ȃ̂ŃV���O���g��
		FileCatcher(); ~FileCatcher();

		WNDCLASSEX mWindowClass;

		std::string mFilePath;

		static bool sIsUpdated;
		static TCHAR sFilePath[MAX_PATH];
		static DropPos sDropPos;
	};

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
		class SignaturedBuffer;
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
			// vertesCount		: ����	: ���_��
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
			void ClearRenderTarget
			(
				const Color& color = Color(0.5f, 0.5f, 0.5f)
			);

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
				const SignaturedBuffer& buffer,
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
			void SetViewportAndRect(const RenderTarget& renderTarget);

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
			friend Result Device::CreateRenderTarget
			(
				RenderTarget&, const SwapChain&
			);
		public:
			RenderTarget(); ~RenderTarget();

			// ���C�u��������Ăяo���֐�
			void GetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE& handle, const int bufferID) const;
			const ComPtr<ID3D12Resource> GetGPU_Address(const int bufferID) const;

			D3D12_VIEWPORT GetViewPort() const;
			D3D12_RECT GetRect() const;
		private:
			ComPtr<ID3D12DescriptorHeap> mRTV_Heaps;
			ComPtr<ID3D12Resource>* mRT_Resource;
			int mBufferCount;

			int mViewIncrementSize;

			D3D12_VIEWPORT mViewPort;
			D3D12_RECT mScissorRect;
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
			void GetDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE& handle) const;
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
		// ���̂���4���ǂ̂悤�ɔz�u���邩���L�q�������̂����[�g�V�O�l�`��
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
			// �X�e���V�����g���ꍇ�B�@���ݎg���\��͂Ȃ����ߖ�����
			void SetDepthStencilEnable();

			// ���ߗL�����@�Ăяo���Ȃ��ꍇ����
			void SetAlphaEnable();

			// �J�����O�������@�Ăяo���Ȃ��ꍇ�L��
			void SetCullDisable();

			// ���_���C�A�E�g
			void SetInputLayout(const InputElementDesc& inputElementDesc);
			// ���[�g�V�O�l�`��
			void SetRootSignature(const RootSignature& rootSignature);

			// �V�F�[�_�[�ւ̃|�C���^
			// �w�b�_�֏����o�������̂��o�C���h����ꍇ
			void SetVertexShader(const unsigned char* const vertexShader, const int length);
			void SetPixelShader(const unsigned char* const pixelShader, const int length);

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

			// ���C�u��������Ăяo���֐�
			const D3D12_VERTEX_BUFFER_VIEW* const GetView() const;
			const int GetVertexCount() const;
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

			// ���C�u��������Ăяo���֐�
			const D3D12_INDEX_BUFFER_VIEW* const GetView() const;
			const int GetIndexCount() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_INDEX_BUFFER_VIEW mView;
			int mIndexCount;

		};

		// �萔�o�b�t�@�A�e�N�X�`�����A���[�g�V�O�l�`���o�R�Ńo�C���h�����
		// ���\�[�X�̃C���^�[�t�F�[�X
		class SignaturedBuffer
		{
		public:
			virtual ~SignaturedBuffer();

			virtual const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const = 0;
		private:
		};

		// �萔�o�b�t�@
		class ConstantBuffer : public SignaturedBuffer
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

		class Texture2D : public SignaturedBuffer
		{
			friend Result Device::CreateTexture2D
			(
				Texture2D&, DescriptorHeap&
			);
		public:
			Texture2D(); 
			~Texture2D();

			Result LoadFromFile(const wchar_t* const filePath);

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

#endif // !_GUI_UTIL_H_



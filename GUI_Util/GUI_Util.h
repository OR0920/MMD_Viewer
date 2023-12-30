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

#ifdef _DEBUG
#define MAIN main 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG


// GUI���쐬����N���X��������
namespace GUI
{
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
		CONTINUE,
		QUIT,
	};

	// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
	// ���̃C���^�[�t�F�C�X�Ő錾����Ă��郁�\�b�h�́A
	// ���[�U�[������͌Ăяo���Ȃ�
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetCPU_Handle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
	protected:
	};

	// ���C���E�B���h�E�@���j���[�͍��̂Ƃ������Ă��Ȃ�
	class MainWindow : public ParentWindow
	{
	public:
		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// �T�C�Y���w�肵����
		Result Create(int width, int height);
		// ���b�Z�[�W����������
		// ����{�^������������false��Ԃ�
		Result ProcessMessage();

		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ���C�u����������Ăяo���֐�
		const HWND GetCPU_Handle() const;
	private:
		MainWindow();
		~MainWindow();

		int mWidth;
		int mHeight;

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};

	void ErrorBox(const TCHAR* const message);

	// �̈���Ƀh���b�v���ꂽ�E�B���h�E��F������E�B���h�E
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
		class ConstantBuffer;
		class DescriptorHeapForShaderData;

		class Device
		{
		public:
			Device(); ~Device();

			Result Create();

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
			Result CreateVertexBuffer
			(
				VertexBuffer& vertexBuffer,
				const unsigned int vertexTypeSize,
				const unsigned int vertexCount
			);

			Result CreateIndexBuffer
			(
				IndexBuffer& indexBuffer,
				const unsigned int indexTypeSize,
				const unsigned int indexCount
			);

			Result CreateConstantBuffer
			(
				ConstantBuffer& constantBuffer,
				DescriptorHeapForShaderData& viewHeap,
				const unsigned int bufferStructSize,
				const unsigned int bufferCount = 1
			);

			Result CreateDescriptorHeap
			(
				DescriptorHeapForShaderData& heap,
				const unsigned int descriptorCount
			);
		private:
			ComPtr<ID3D12Device> mDevice;
		};

		class SwapChain
		{
		public:
			SwapChain(); ~SwapChain();

			Result Create
			(
				GraphicsCommand& device,
				const ParentWindow& targetWindow,
				const int frameCount
			);

			int GetCurrentBackBufferIndex() const;

			void Present();

			// ���C�u��������Ăяo���֐�
			Result GetDesc(DXGI_SWAP_CHAIN_DESC* desc) const;
			Result GetBuffer(const unsigned int bufferID, ID3D12Resource** resource) const;
		private:
			ComPtr<IDXGISwapChain4> mSwapChain;

		};

		class GraphicsCommand
		{
			friend Result Device::CreateGraphicsCommand(GraphicsCommand&);
			friend Result SwapChain::Create
			(
				GraphicsCommand&,
				const ParentWindow&,
				const int
			);
		public:
			GraphicsCommand(); ~GraphicsCommand();

			void BeginDraw();
			void BeginDraw(const GraphicsPipeline& pipeline);

			void SetGraphicsPipeline(const GraphicsPipeline& pipeline);

			void UnlockRenderTarget(const RenderTarget& renderTarget);

			void SetRenderTarget
			(
				const RenderTarget& renderTarget
			);

			void SetRenderTarget
			(
				const RenderTarget& renderTarget,
				const DepthStencilBuffer& depthStenilBuffer
			);

			void ClearRenderTarget
			(
				const Color& color = Color(0.5f, 0.5f, 0.5f)
			);

			void ClearDepthBuffer();

			void SetGraphicsRootSignature(const RootSignature& rootSignature);

			void SetDescriptorHeap(const DescriptorHeapForShaderData& descHeap);
			void SetConstantBuffer
			(
				const ConstantBuffer& constBuffer, 
				const int rootParamID
			);
			void SetDescriptorTable
			(
				const ConstantBuffer& constBuffer,
				const int bufferID,
				const int rootParamID
			);

						
			void DrawTriangle(const VertexBuffer& vertex);
			void DrawTriangleList
			(
				const VertexBuffer& vertex,
				const IndexBuffer& index
			);

			void LockRenderTarget(const RenderTarget& renderTarget);

			void EndDraw();
		private:
			void SetViewportAndRect(const RenderTarget& renderTarget);

			ID3D12Device* mDevice;
			IDXGISwapChain4* mSwapChain;

			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;

			D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle;
			D3D12_CPU_DESCRIPTOR_HANDLE mDSV_Handle;

			ComPtr<ID3D12Fence> mFence;
			UINT64 mFenceValue;
		};

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

			int mIncrementSize;

			D3D12_VIEWPORT mViewPort;
			D3D12_RECT mScissorRect;
		};

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

		class DescriptorRange
		{
		public:
			DescriptorRange(); ~DescriptorRange();


			void SetRangeCount(const int rangeCount);
			void SetRangeForCBV(const int rangeID, const int registerID, const int descriptorCount);
			//void SetRangeForSRV(const int registerID, const int descriptorCount);


			// ���C�u��������Ăяo���֐�
			int GetRangeCount() const;
			const D3D12_DESCRIPTOR_RANGE* const GetRange() const;
		private:
			D3D12_DESCRIPTOR_RANGE* mRange;
			int mRangeCount;
		};

		class RootSignature
		{
			friend Result Device::CreateRootSignature(RootSignature&);
		public:
			RootSignature(); ~RootSignature();

			// ������
			void SetParameterCount(const int count);
			void SetParamForCBV(const int paramID, const int registerID);
			void SetParamForDescriptorTable
			(
				const int paramID,
				const DescriptorRange& range
			);


			// ���C�u��������Ăяo���֐�
			const ComPtr<ID3D12RootSignature> GetRootSignature() const;
		private:
			ComPtr<ID3D12RootSignature> mRootSignature;
			D3D12_ROOT_SIGNATURE_DESC mDesc;
			D3D12_ROOT_PARAMETER* mRootParamter;
			
			bool IsSizeOver(const int i) const;
		};

		class InputElementDesc
		{
		public:
			InputElementDesc(); ~InputElementDesc();

			void SetElementCount(const int count);

			void SetDefaultPositionDesc(const char* const semantics = "POSITION");
			void SetDefaultColorDesc(const char* const semantics = "COLOR");
			void SetDefaultNormalDesc(const char* const semantics = "NORMAL");

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

		class GraphicsPipeline
		{
			friend Result Device::CreateGraphicsPipeline(GraphicsPipeline&);
		public:
			GraphicsPipeline(); ~GraphicsPipeline();

			// �[�x�o�b�t�@�L����
			void SetDepthEnable();
			// �X�e���V�����g���ꍇ�B�@���ݎg���\��͂Ȃ����ߖ�����
			void SetDepthStencilEnable();

			void SetAlphaEnable();

			void SetInputLayout(const InputElementDesc& inputElementDesc);
			void SetRootSignature(const RootSignature& rootSignature);

			void SetVertexShader(const unsigned char* const vertexShader, const int length);
			void SetPixelShader(const unsigned char* const pixelShader, const int length);

			// ���C�u��������Ăяo���֐�
			const ComPtr<ID3D12PipelineState> GetPipelineState() const;
		private:
			ComPtr<ID3D12PipelineState> mPipelineState;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		};

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

			Result Copy(const void* const data);

			// ���C�u��������Ăяo���֐�
			const D3D12_VERTEX_BUFFER_VIEW* const GetView() const;
			const int GetVertexCount() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_VERTEX_BUFFER_VIEW mView;
			int mVertexCount = 0;
		};

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

		class ConstantBuffer
		{
			friend Result Device::CreateConstantBuffer
			(
				ConstantBuffer&, 
				DescriptorHeapForShaderData&,
				unsigned int, unsigned int
			);
		public:
			ConstantBuffer(); ~ConstantBuffer();

			Result Map(void** ptr);
			void Unmap();

			//���C�u��������Ăяo���֐�
			const D3D12_GPU_VIRTUAL_ADDRESS GetGPU_Address() const;
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i) const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_CONSTANT_BUFFER_VIEW_DESC mViewDesc;
			D3D12_CPU_DESCRIPTOR_HANDLE mCPU_Handle;
			D3D12_GPU_DESCRIPTOR_HANDLE mGPU_Handle;

			int mIncrementSize;
		};

		class DescriptorHeapForShaderData
		{
			friend Result Device::CreateDescriptorHeap
			(
				DescriptorHeapForShaderData&,
				const unsigned int
			);
		public:
			DescriptorHeapForShaderData();
			~DescriptorHeapForShaderData();

			// ���C�u��������Ăяo���֐�
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCPU_Handle();
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle();
			void MoveToNextHeapPos(const int offset);

			const ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const;
		private:
			ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
			int mDescriptorCount;
			int mIncrementSize;
			int mLastID;

		};
	}
}

#endif // !_GUI_UTIL_H_



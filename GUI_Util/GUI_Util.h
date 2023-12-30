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


// GUIを作成するクラスが属する
namespace GUI
{
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
		CONTINUE,
		QUIT,
	};

	// 子ウィンドウを持てるウィンドウのインターフェイス
	// このインターフェイスで宣言されているメソッドは、
	// ユーザー側からは呼び出さない
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetCPU_Handle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
	protected:
	};

	// メインウィンドウ　メニューは今のところ作っていない
	class MainWindow : public ParentWindow
	{
	public:
		// メインウィンドウは一つだけを想定しシングルトン
		static MainWindow& Instance();

		// サイズを指定し生成
		Result Create(int width, int height);
		// メッセージを処理する
		// 閉じるボタンが押されるとfalseを返す
		Result ProcessMessage();

		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ライブラリ側から呼び出す関数
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

	// 領域内にドロップされたウィンドウを認識するウィンドウ
	class FileCatcher
	{
	public:
		// シングルトン用関数
		static FileCatcher& Instance();

		// 親を指定し生成
		// 親のクライアント領域いっぱいにサイズが指定される
		// 親のサイズ変更に合わせて自動で調整される
		Result Create(const ParentWindow& parent);

		bool Update();

		// パスの長さ、パス、ドロップされた位置を取得する
		int GetLength() const;
		const char* const GetPath() const;

		struct DropPos
		{
			long x;
			long y;
		};
		const DropPos& GetDropPos() const;


		// コールバック関数。　ユーザーからは呼び出さない。
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	private:
		// コールバック関数からパスを取得する都合上、
		// 複数あると面倒なのでシングルトン
		FileCatcher(); ~FileCatcher();

		WNDCLASSEX mWindowClass;

		std::string mFilePath;

		static bool sIsUpdated;
		static TCHAR sFilePath[MAX_PATH];
		static DropPos sDropPos;
	};

	// D3D12の薄いラッパー
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

			// 深度バッファのみ使用する場合
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

			// ライブラリから呼び出す関数
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

			// ライブラリから呼び出す関数
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

			// ライブラリから呼び出す関数
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


			// ライブラリから呼び出す関数
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

			// 実装中
			void SetParameterCount(const int count);
			void SetParamForCBV(const int paramID, const int registerID);
			void SetParamForDescriptorTable
			(
				const int paramID,
				const DescriptorRange& range
			);


			// ライブラリから呼び出す関数
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

			// ライブラリが呼び出す関数
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

			// 深度バッファ有効化
			void SetDepthEnable();
			// ステンシルも使う場合。　現在使う予定はないため未実装
			void SetDepthStencilEnable();

			void SetAlphaEnable();

			void SetInputLayout(const InputElementDesc& inputElementDesc);
			void SetRootSignature(const RootSignature& rootSignature);

			void SetVertexShader(const unsigned char* const vertexShader, const int length);
			void SetPixelShader(const unsigned char* const pixelShader, const int length);

			// ライブラリから呼び出す関数
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

			// ライブラリから呼び出す関数
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
			
			// ライブラリから呼び出す関数
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

			//ライブラリから呼び出す関数
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

			// ライブラリから呼び出す関数
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



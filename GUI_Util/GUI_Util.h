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
		virtual const HWND GetHandle() const = 0;
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
		const HWND GetHandle() const;
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

		Result EnalbleDebugLayer();

		class GraphicsCommand;
		class SwapChain;
		class RenderTarget;
		class DepthStencilBuffer;
		class Fence;

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

			Result CreateFence(Fence& fence);
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

			// ライブラリから呼び出す関数
			Result GetDesc(void* desc) const;
			Result GetBuffer(const unsigned int bufferID, void** resource) const;
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

			void UnlockRenderTarget(const RenderTarget& renderTarget);

			void SetRenderTarget
			(
				const RenderTarget* const renderTarget,
				const DepthStencilBuffer* const depthStencilBuffer = nullptr
			);

			void LockRenderTarget(const RenderTarget& renderTarget);

			void EndDraw();
		private:
			ID3D12Device* mDevice;
			IDXGISwapChain4* mSwapChain;

			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;

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
			void GetDescriptorHandle(void* handlePtr, const int bufferID) const;
			const ComPtr<ID3D12Resource> GetResource(const int bufferID) const;
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
			void GetDescriptorHandle(void* handlePtr) const;
		private:
			ComPtr<ID3D12Resource> mDSB_Resource;
			ComPtr<ID3D12DescriptorHeap> mDSV_Heap;


		};

		class Fence
		{
			friend Result Device::CreateFence(Fence&);
		public:
			Fence(); ~Fence();

		private:
			ComPtr<ID3D12Fence> mFence;
			UINT64 mFenceValue;
		};
	}
}

#endif // !_GUI_UTIL_H_



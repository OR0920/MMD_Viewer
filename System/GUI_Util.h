#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_4.h>

namespace System
{
	enum Result;

	// GUIを作成するクラスが属する
	namespace GUI
	{
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
			// サイズを指定し生成
			Result Create(int width, int height);
			// 閉じるボタンが押されるとtrueを返す
			bool IsClose();


			const int GetWindowWidth() const;
			const int GetWindowHeight() const;

			// メインウィンドウは一つだけを想定しシングルトン
			static MainWindow& Instance();

			// ライブラリ側から呼び出す関数
			const HWND GetHandle() const;
		private:
			bool isClose;
			MainWindow();
			~MainWindow();

			int mWidth;
			int mHeight;

			HWND mWindowHandle;
			WNDCLASSEX mWindowClass;
		};


		// 領域内にドロップされたウィンドウを認識するウィンドウ
		class FileCatcher
		{
		public:
			struct DropPos
			{
				long x;
				long y;
			};

			// 親を指定し生成
			// 親のクライアント領域いっぱいにサイズが指定される
			// 親のサイズ変更に合わせて自動で調整される
			Result Create(const ParentWindow& parent);

			bool Update();

			// パスの長さ、パス、ドロップされた位置を取得する
			int GetLength() const;
			const char* const GetPath() const;
			const DropPos& GetDropPos() const;

			// シングルトン用関数
			static FileCatcher& Instance();

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


		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		// parent で指定したウィンドウにDirect3Dでの描画を有効にする
		static const int gFrameCount = 2;
		class GraphicsEngine
		{
		public:
			GraphicsEngine(); ~GraphicsEngine();

			Result Init(const ParentWindow& parent);

			struct Color
			{
				float r = 0.f;
				float g = 0.f;
				float b = 0.f;
				float a = 1.f;

				static const Color Black;
				static const Color Gray;
				static const Color White;

				Color
				(
					float _r, 
					float _g, 
					float _b,
					float _a = 1.f
				);

				Color();
			};

			// 描画の単位
			class Scene
			{
			public:
				Scene(); ~Scene();

				void SetBackGroundColor
				(
					const Color clearColor
				);

				const Color& GetBackGroundColor() const;
			private:
				Color mClearColor;
			};

			void Draw(const Scene& scene);

		private:
			int mParentWidth;
			int mParentHeight;

			ComPtr<ID3D12Device> mDevice;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;
			ComPtr<IDXGISwapChain3> mSwapChain;

			ComPtr<ID3D12DescriptorHeap> mRTV_Heap;
			ComPtr<ID3D12Resource> mRenderTargets[gFrameCount];
			ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
			ComPtr<ID3D12Resource> mDepthBuffer;

			ComPtr<ID3D12Fence> mFence;

			uint64_t mFenceValue;

		};
	}
}

#endif // !_GUI_UTIL_H_



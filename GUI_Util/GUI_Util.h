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
#include<dxgi1_4.h>


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


	struct Color
	{
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;

		Color();
		Color(float _r, float _g, float _b, float _a = 1.f);
	};

	class Canvas
	{
	public:
		Canvas(const ParentWindow& parent, const int frameCount);

		Result IsSuccessInit() const;

		void BeginDraw();
		void Clear(const Color& clearColor = Color(0.f, 0.f, 1.f));
		void EndDraw();
	private:
		Result mIsSuccessInit;
		const int mFrameCount;
		const ParentWindow& mWindow;
		const int mWidth, mHeight;

		Result InitDirect3D();

		static ComPtr<ID3D12Device> sDevice;
		static ComPtr<ID3D12CommandQueue> sCommandQueue;

		ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;
		ComPtr<IDXGISwapChain3> mSwapChain;

		// RT : Render Target, RTV : Render Target View
		ComPtr<ID3D12DescriptorHeap> mRTV_Heap;
		std::vector<ComPtr<ID3D12Resource>>	mRT_Resouces;

		// DSB : Depth Stencil Buffer, DSV : Depth Stencil View
		ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
		ComPtr<ID3D12Resource> mDSB_Resouce;

		ComPtr<ID3D12Fence> mFence;
		UINT mFenceValue;
	};
}

#endif // !_GUI_UTIL_H_



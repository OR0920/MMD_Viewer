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

// myLib
#include"MathUtil.h"

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

	class Model
	{
	public:
		Model();
		Model(const Model& other);
		const Model& operator=(const Model& other);

		Result Load(const char* const filepath);
		void Reset();
		void Draw();

	private:
		Result LoadAsPMD(const char* const filepath);
		Result LoadAsPMX(const char* const filepath);

		ComPtr<ID3D12Resource> mVB_Resource;
		D3D12_VERTEX_BUFFER_VIEW mVB_View;
		ComPtr<ID3D12Resource> mIB_Resource;
		D3D12_INDEX_BUFFER_VIEW mIB_View;

		
	};

	class GraphicsDevice
	{
		friend Model;
	public:
		// 描画先のウィンドウとバックバッファ数を指定する
		static Result Init(const ParentWindow& window, const int frameCount);
		// シングルトン
		static GraphicsDevice& Instance();
		// 終了処理
		static void Tern();

		// 描画の準備を行う
		void BeginDraw();
		// 画面クリア
		void Clear(const Color& clearColor = Color(0.f, 0.f, 1.f));

		// カメラセット
		void SetCamera
		(
			const MathUtil::float3 eye,
			const MathUtil::float3 target,
			const MathUtil::float3 up
		);

		// 描画処理を実行する
		void EndDraw();
		
	private:
		static GraphicsDevice* sCanvas;
		GraphicsDevice(const ParentWindow& window, const int frameCount); ~GraphicsDevice();

		Result InitDirect3D();
		Result InitConstantResource();

		Result mIsSuccessInit;
		const int mFrameCount;
		const ParentWindow& mWindow;
		const int mWidth, mHeight;

		static ComPtr<ID3D12Device> GetDevice();

		ComPtr<ID3D12Device> mDevice;
		ComPtr<ID3D12CommandQueue> mCommandQueue;

		ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;
		ComPtr<IDXGISwapChain3> mSwapChain;

		// RT : Render Target, RTV : Render Target View
		ComPtr<ID3D12DescriptorHeap> mRTV_Heap;
		std::vector<ComPtr<ID3D12Resource>>	mRT_Resouces;
		int mCurrentBufferID;

		D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle;

		// DSB : Depth Stencil Buffer, DSV : Depth Stencil View
		ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
		ComPtr<ID3D12Resource> mDSB_Resouce;

		D3D12_CPU_DESCRIPTOR_HANDLE mDSV_Handle;

		ComPtr<ID3D12Fence> mFence;
		UINT mFenceValue;

		struct ConstantBuffer
		{
			MathUtil::Matrix world;
			MathUtil::Matrix view;
			MathUtil::Matrix projection;
		};
		ComPtr<ID3D12Resource> mCB_Resource;
		ComPtr<ID3D12DescriptorHeap> mCB_Heap;
		ConstantBuffer* mappedCB;


	};
}

#endif // !_GUI_UTIL_H_



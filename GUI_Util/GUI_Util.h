// 
// GUIを扱うAPIのラッパライブラリ
// 必要な機能のみを実装
// あまり汎用的にしすぎず、わかりやすさを最重視
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

// other
#include"Result.h"
#include"Graphics.h"

// エントリポイントを隠蔽
// デバッグ情報をコンソールへ
#ifdef _DEBUG
#define MAIN() main() 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG

// WIN32APIのウィンドウ周りをラップ
namespace GUI
{
	// 子ウィンドウを持てるウィンドウのインターフェイス
	// これを継承していないウィンドウは子ウィンドウを持てない
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetHandle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
		virtual	const int GetClientWidth() const = 0; 
		virtual const int GetClientHeight() const = 0;
	};

	// メインウィンドウ
	// タイトルバーと閉じるなどのボタンを持つウィンドウ
	class MainWindow : public ParentWindow
	{
	public:
		// メインウィンドウは一つだけを想定しシングルトン
		static MainWindow& Instance();

		// サイズを指定し生成
		Result Create(int width, int height);
		
		// メッセージを処理する
		// 閉じるボタンが押されるとfalseを返す
		// メッセージを待つ
		Result ProsessMessage();

		// メッセージを待たない
		Result ProcessMessageNoWait();

		// ウィンドウそのもののサイズ
		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ウィンドウの描画領域のサイズ
		const int GetClientWidth() const;
		const int GetClientHeight() const;

		// ライブラリ側から呼び出す関数
		const HWND GetHandle() const;
	private:
		MainWindow();
		~MainWindow();

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};

	// エラー表示用ダイアログボックス
	void ErrorBox(const TCHAR* const message);

	// 領域内にドロップされたファイルを認識するウィンドウ
	class FileCatcher
	{
	public:
		FileCatcher(); ~FileCatcher();

		// 親を指定し生成
		// 親のクライアント領域いっぱいにサイズが指定される
		// 親のサイズ変更に合わせて自動で調整される
		Result Create(const ParentWindow& parent);

		bool Update();

		// パスの長さ、パス、ドロップされた位置を取得する
		int GetLength() const;
		int GetWideLength() const;
		const char* const GetPath() const;
		const wchar_t* const GetWidePath() const;

		// ファイルがドロップされた場所
		struct DropPos
		{
			long x;
			long y;
		};
		const DropPos& GetDropPos() const;

	private:
		FileCatcher(const FileCatcher&) = delete;
		const FileCatcher& operator=(const FileCatcher&) = delete;


		std::string mFilePath;
		bool mIsUpdated;
		wchar_t mWideFilePath[MAX_PATH];
		DropPos mDropPos;


		// このコールバック関数に、データを更新してもらう。
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
		static WNDCLASSEX mWindowClass;
	};
}

#endif // !_GUI_UTIL_H_



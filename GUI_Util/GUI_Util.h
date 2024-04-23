// 
// GUIを扱うAPIのラッパライブラリ
// 必要な機能のみを実装
// あまり汎用的にしすぎず、わかりやすさを最重視
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>

// windows
#include<windows.h>
#include<tchar.h>

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

#include"ParentWindow.h"
#include"MainWindow.h"

// WIN32APIのウィンドウ周りをラップ
namespace GUI_Util
{
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



// 
// GUIを扱うAPIのラッパライブラリ
// 必要な機能のみを実装
// あまり汎用的にしすぎず、わかりやすさを最重視
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

#include<string>
#include<windows.h>

#include"ParentWindow.h"
#include"Result.h"
#include"MainWindow.h"
#include"FileCatcher.h"

// エントリポイントを隠蔽
// デバッグ情報をコンソールへ
#ifdef _DEBUG
#define MAIN() main() 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG


// WIN32APIのウィンドウ周りをラップ
namespace GUI_Util
{
	// エラー表示用ダイアログボックス
	void ErrorBox(const TCHAR* const message);

	class ParentWindow;
	class MainWindow;
	class FileCatcher;
}

#endif // !_GUI_UTIL_H_



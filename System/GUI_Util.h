#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

#include<string>

#include<windows.h>
#include<tchar.h>

namespace System
{
	enum Result;

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
		protected:
		};

		class MainWindow : public ParentWindow
		{
		public:
			// サイズを指定し生成
			Result Create(int width, int height);
			// 閉じるボタンが押されるとtrueを返す
			bool IsClose();

			// メインウィンドウは一つだけを想定しシングルトン
			static MainWindow& Instance();


			// ライブラリ側から呼び出す関数
			const HWND GetHandle() const final;
		private:
			bool isClose;
			MainWindow();
			~MainWindow() final;

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

			int GetLength() const;
			const char* const GetPath() const;
			const DropPos& GetDropPos() const;

			static FileCatcher& Instance();
			static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

		private:
			FileCatcher(); ~FileCatcher();
			std::string mFilePath;

			static bool sIsUpdated;
			static TCHAR sFilePath[MAX_PATH];
			static DropPos sDropPos;
		};
	}
}

#endif // !_GUI_UTIL_H_



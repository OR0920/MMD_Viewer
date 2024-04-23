#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

namespace GUI_Util
{
	// メインウィンドウ
	// タイトルバーと閉じるなどのボタンを持つウィンドウ
	class MainWindow : public ParentWindow
	{
	public:
		// メインウィンドウは一つだけを想定しシングルトン
		static MainWindow& Instance();

		// サイズを指定し生成
		Result Create(int width, int height, const LPCTSTR windowTitle);

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
}

#endif // !_MAIN_WINDOW_H_


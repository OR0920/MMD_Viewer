#pragma once

namespace GUI_Util
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
}

#ifndef _FILE_CATCHER_H_
#define _FILE_CATCHER_H_

namespace GUI_Util
{
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

#endif // !_FILE_CATCHER_H_

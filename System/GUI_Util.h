#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

namespace System
{
	enum Result;

	class MainWindow
	{
	public:
		static MainWindow& Instance();

		Result Create(int width, int height);
		bool IsClose();
	private:
		bool isClose;
		MainWindow();
		~MainWindow();
	};

	class FileCatcherWindow
	{
	public:
		FileCatcherWindow();
		~FileCatcherWindow();

		Result Create();
	private:
		FileCatcherWindow(const FileCatcherWindow&);
		const FileCatcherWindow& operator=(const FileCatcherWindow&);
	};
}

#endif // !_GUI_UTIL_H_



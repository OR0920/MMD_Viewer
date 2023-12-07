#pragma once

#include<windows.h>
#include<memory>

class Dx12Wrapper;
class PMDRenderer;
class PMDActor;

class Application
{
public:
	static Application& Instance();
		

	bool Init
	(
		const unsigned int argWindowWidth ,
		const unsigned int argWindowHeight,
		const char* const modelPath
	);
	void Run();
	void Term();
	SIZE GetWindowSize() const;

	~Application();
private:
	unsigned int mWindowWidth;
	unsigned int mWindowHeight;

	WNDCLASSEX mWindowClass;
	HWND mWindowHandle;

	std::shared_ptr<Dx12Wrapper> mDx12;
	std::shared_ptr<PMDRenderer> mPmdRenderer;
	std::shared_ptr<PMDActor> mPmdActor;

	void CreateGameWindow();
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
};


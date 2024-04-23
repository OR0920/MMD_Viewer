// �w�b�_�[�ɕK�v�ȃ��W���[��
#include<Windows.h>
#include<string>

#include"Result.h"
#include"ParentWindow.h"

// �w�b�_�[
#include"FileCatcher.h"

// �����W���[��
#include"private/OutputLastError.h"

// �O�����W���[��
#include<tchar.h>
#include"System.h"

using namespace GUI_Util;

// FileCatcher
// �v���V�[�W���[
LRESULT CALLBACK FileCatcher::FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// �E�B���h�E�ɕR�Â��Ă���FileCatcher�̃C���X�^���X�����炤
	FileCatcher* fc = reinterpret_cast<FileCatcher*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_CREATE:
	{
		DebugMessage("File Catcher Created");

		// CreateWindow�֐��̖����ɓn�����p�����[�^���擾����
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lp);
		// FileCatcher�̃C���X�^���X���A�E�B���h�E�ɕR�Â���
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));

		DragAcceptFiles(hwnd, true);
		break;
	}
	// �t�@�C�����h���b�v���ꂽ�炱���ɗ���
	case WM_DROPFILES:
	{
		RECT rect = {};
		GetWindowRect(hwnd, &rect);
		GetCursorPos(reinterpret_cast<LPPOINT>(&fc->mDropPos));
		// �t�@�C���̃h���b�v�ꏊ�̓f�B�X�v���C�S�̂���̍��W�ɂȂ��Ă��܂����߁A
		// �E�B���h�E�̌��_��ɏC��
		fc->mDropPos.x -= rect.left;
		fc->mDropPos.y -= rect.top;

		// �h���b�v���ꂽ�p�X���擾
		DragQueryFile((HDROP)wp, 0, fc->mWideFilePath, MAX_PATH);
		DragFinish((HDROP)wp);

		// �X�V��`����
		fc->mIsUpdated = true;

		break;
	}
	// �T�C�Y���ύX���ꂽ�炱���ɗ���B�@�e�E�B���h�E�������Ă����
	case WM_SIZE:
	{
		auto parentHwnd = GetParent(hwnd);

		RECT parentClientRect{};
		GetClientRect(parentHwnd, &parentClientRect);
		MoveWindow(hwnd, 0, 0, parentClientRect.right, parentClientRect.bottom, true);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}


Result FileCatcher::Create(const ParentWindow& parent)
{
	auto& wc = mWindowClass;
	auto parentHwnd = parent.GetHandle();

	// �E�B���h�E�N���X�\���͈̂�����@�Ƃ肠�������O�Ŕ���
	if (wc.lpszClassName == nullptr)
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = FileCatcherProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = _T(ToString(FileCatcher));
		wc.hIconSm = NULL;
		wc.hbrBackground = (HBRUSH)0x00000000;

		if (RegisterClassEx(&wc) == 0)
		{
			DebugMessageFunctionError(RegisterClassEx(), FileCatcher::Create());
			OutputLastError();
			return FAIL;
		}
	}


	auto hwnd = CreateWindowEx
	(
		WS_EX_ACCEPTFILES,
		wc.lpszClassName,
		wc.lpszClassName,
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		0,
		0,
		parent.GetClientWidth(),
		parent.GetClientHeight(),
		parentHwnd,
		NULL,
		wc.hInstance,
		this// ���g�ւ̃|�C���^��n���A�v���V�[�W���ɏ��������Ă��炤
	);


	if (hwnd == NULL)
	{
		DebugMessageFunctionError(CreateWindowEx(), FileCatcher::Create());
		return FAIL;
	}

	return SUCCESS;
}

bool FileCatcher::Update()
{
	// �X�V����Ă���΃v���V�[�W���[��true�����Ă���͂�
	if (mIsUpdated == false)
	{
		return false;
	}

	// ���C�h��������}���`�o�C�g�֕ϊ��@�Œ蒷�̕ϊ�������Ă��Ȃ��̂ŁA�ϒ���
	// �]�T������ΌŒ蒷�̕ϊ�����������B
	char* filePath = nullptr;
	System::newArray_CreateMultiByteStrFromWideCharStr(&filePath, mWideFilePath);

	mFilePath = filePath;

	System::SafeDeleteArray(&filePath);

	mIsUpdated = false;
	return true;
}

int FileCatcher::GetLength() const
{
	return static_cast<int>(mFilePath.size());
}

int FileCatcher::GetWideLength() const
{
	return System::GetStringLength(mWideFilePath);
}

const char* const FileCatcher::GetPath() const
{
	return mFilePath.c_str();
}

const wchar_t* const FileCatcher::GetWidePath() const
{
	return mWideFilePath;
}

const FileCatcher::DropPos& FileCatcher::GetDropPos() const
{
	return mDropPos;
}

WNDCLASSEX FileCatcher::mWindowClass = {};

FileCatcher::FileCatcher()
	:
	mIsUpdated(false),
	mFilePath(),
	mWideFilePath(),
	mDropPos({})
{

}

FileCatcher::~FileCatcher()
{
	UnregisterClass(mWindowClass.lpszClassName, mWindowClass.hInstance);
}




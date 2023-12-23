#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_4.h>


	// GUI���쐬����N���X��������
namespace GUI
{
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
	};

	// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
	// ���̃C���^�[�t�F�C�X�Ő錾����Ă��郁�\�b�h�́A
	// ���[�U�[������͌Ăяo���Ȃ�
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetHandle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
	protected:
	};

	// ���C���E�B���h�E�@���j���[�͍��̂Ƃ������Ă��Ȃ�
	class MainWindow : public ParentWindow
	{
	public:
		// �T�C�Y���w�肵����
		Result Create(int width, int height);
		// ����{�^������������true��Ԃ�
		bool IsClose();


		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// ���C�u����������Ăяo���֐�
		const HWND GetHandle() const;
	private:
		bool isClose;
		MainWindow();
		~MainWindow();

		int mWidth;
		int mHeight;

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};


	// �̈���Ƀh���b�v���ꂽ�E�B���h�E��F������E�B���h�E
	class FileCatcher
	{
	public:
		struct DropPos
		{
			long x;
			long y;
		};

		// �e���w�肵����
		// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
		// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
		Result Create(const ParentWindow& parent);

		bool Update();

		// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
		int GetLength() const;
		const char* const GetPath() const;
		const DropPos& GetDropPos() const;

		// �V���O���g���p�֐�
		static FileCatcher& Instance();

		// �R�[���o�b�N�֐��B�@���[�U�[����͌Ăяo���Ȃ��B
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	private:
		// �R�[���o�b�N�֐�����p�X���擾����s����A
		// ��������Ɩʓ|�Ȃ̂ŃV���O���g��
		FileCatcher(); ~FileCatcher();

		WNDCLASSEX mWindowClass;

		std::string mFilePath;

		static bool sIsUpdated;
		static TCHAR sFilePath[MAX_PATH];
		static DropPos sDropPos;
	};


}

#endif // !_GUI_UTIL_H_


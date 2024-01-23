// 
// GUI������API�̃��b�p���C�u����
// �K�v�ȋ@�\�݂̂�����
// ���܂�ėp�I�ɂ��������A�킩��₷�����ŏd��
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>
#include<vector>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>

// other
#include"Result.h"
#include"Graphics.h"

// �G���g���|�C���g���B��
// �f�o�b�O�����R���\�[����
#ifdef _DEBUG
#define MAIN() main() 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG

// WIN32API�̃E�B���h�E��������b�v
namespace GUI
{
	// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
	// ������p�����Ă��Ȃ��E�B���h�E�͎q�E�B���h�E�����ĂȂ�
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

	// ���C���E�B���h�E
	// �^�C�g���o�[�ƕ���Ȃǂ̃{�^�������E�B���h�E
	class MainWindow : public ParentWindow
	{
	public:
		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// �T�C�Y���w�肵����
		Result Create(int width, int height);
		
		// ���b�Z�[�W����������
		// ����{�^������������false��Ԃ�
		// ���b�Z�[�W��҂�
		Result ProsessMessage();

		// ���b�Z�[�W��҂��Ȃ�
		Result ProcessMessageNoWait();

		// �E�B���h�E���̂��̂̃T�C�Y
		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// �E�B���h�E�̕`��̈�̃T�C�Y
		const int GetClientWidth() const;
		const int GetClientHeight() const;

		// ���C�u����������Ăяo���֐�
		const HWND GetHandle() const;
	private:
		MainWindow();
		~MainWindow();

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};

	// �G���[�\���p�_�C�A���O�{�b�N�X
	void ErrorBox(const TCHAR* const message);

	// �̈���Ƀh���b�v���ꂽ�t�@�C����F������E�B���h�E
	class FileCatcher
	{
	public:
		FileCatcher(); ~FileCatcher();

		// �e���w�肵����
		// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
		// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
		Result Create(const ParentWindow& parent);

		bool Update();

		// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
		int GetLength() const;
		int GetWideLength() const;
		const char* const GetPath() const;
		const wchar_t* const GetWidePath() const;

		// �t�@�C�����h���b�v���ꂽ�ꏊ
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


		// ���̃R�[���o�b�N�֐��ɁA�f�[�^���X�V���Ă��炤�B
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
		static WNDCLASSEX mWindowClass;
	};
}

#endif // !_GUI_UTIL_H_



// 
// GUI������API�̃��b�p���C�u����
// �K�v�ȋ@�\�݂̂�����
// ���܂�ėp�I�ɂ��������A�킩��₷�����ŏd��
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>

// windows
#include<windows.h>
#include<tchar.h>

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

#include"ParentWindow.h"
#include"MainWindow.h"

// WIN32API�̃E�B���h�E��������b�v
namespace GUI_Util
{
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



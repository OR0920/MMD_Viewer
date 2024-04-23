// 
// GUI������API�̃��b�p���C�u����
// �K�v�ȋ@�\�݂̂�����
// ���܂�ėp�I�ɂ��������A�킩��₷�����ŏd��
// 


#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

#include<string>
#include<windows.h>

#include"ParentWindow.h"
#include"Result.h"
#include"MainWindow.h"
#include"FileCatcher.h"

// �G���g���|�C���g���B��
// �f�o�b�O�����R���\�[����
#ifdef _DEBUG
#define MAIN() main() 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG


// WIN32API�̃E�B���h�E��������b�v
namespace GUI_Util
{
	// �G���[�\���p�_�C�A���O�{�b�N�X
	void ErrorBox(const TCHAR* const message);

	class ParentWindow;
	class MainWindow;
	class FileCatcher;
}

#endif // !_GUI_UTIL_H_



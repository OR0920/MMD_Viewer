/*
	System���W���[��
	�W�����C�u������OSAPI�ɃA�N�Z�X����C���^�[�t�F�[�X
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include"File.h"
#include"DebugMessage.h"



namespace System
{
	template <class T>
	void SafeDelete(T** ptr)
	{
		if (*ptr != nullptr)
		{
			delete* ptr;
			*ptr = nullptr;
		}
	}

	template <class T>
	void SafeDeleteArray(T** ptr)
	{
		if (*ptr != nullptr)
		{
			delete[] * ptr;
			*ptr = nullptr;
		}
	}

	// �������r
	bool StringEqual(const void* _str1, const void* _str2);

	// �f�B���N�g���̃p�X���擾����֐�
	// dirpath	�f�B���N�g���̃p�X���󂯎��|�C���^ nullptr�ł��邱��
	//			������new�����̂ŁA�K��delete���邱��
	// filepath ���̃|�C���^
	void CopyDirectoryPath(char** _dirpath, const char* const filepath);
	
}

#endif // _SYSTEM_H_
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
	bool StringEqual(const void* const _str1, const void* const _str2);

	// NULL�������܂ޒ�����Ԃ�
	int GetStringLength(const char* const text);

	// �f�B���N�g���̃p�X���擾����֐�
	// dirpath	:�f�B���N�g���̃p�X���󂯎��|�C���^ nullptr�ł��邱�ƁB������new[]�����̂ŁA�K��delete[]���邱��
	// filepath :���̃|�C���^
	void NewArrayAndCopyDirPathFromFilePath(char** _dirpath, const char* const filepath);
	
	// ����̃f�B���N�g���ւ̃p�X�ƃA�Z�b�g�t�@�C��������A�t�@�C�����J�����߂̃p�X��Ԃ��֐�
	// assetpath	:�A�Z�b�g�ւ̃p�X���󂯎��|�C���^ nullptr�ł��邱�ƁB������new[]�����̂ŁA�K��delete[]���邱��
	// dirpath		:�f�B���N�g���̃p�X
	// filename		:�A�Z�b�g�t�@�C����
	// ����̃C���[�W
	// assetpath = dirpath + filename
	void NewArrayAndCopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename);
}

#endif // _SYSTEM_H_
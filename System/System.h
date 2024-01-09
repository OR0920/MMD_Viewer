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

	void CheckMemoryLeak();

	// �������r
	bool StringEqual(const char* const _str1, const char* const _str2);
	bool StringEqual(const wchar_t* const _str1, const wchar_t* const _str2);
	bool StringEqual(const char16_t* const _str1, const char16_t* const _str2);

	// NULL�������܂ޒ�����Ԃ�
	int GetStringLength(const char* const text);
	int GetStringLength(const wchar_t* const text);

	// �f�B���N�g���̃p�X���擾����֐�
	// dirpath	:�f�B���N�g���̃p�X���󂯎��|�C���^ nullptr�ł��邱�ƁB������new[]�����̂ŁA�K��delete[]���邱��
	// filepath :���̃|�C���^
	void newArray_CopyDirPathFromFilePath(char** _dirpath, const char* const filepath);
	
	// ����̃f�B���N�g���ւ̃p�X�ƃA�Z�b�g�t�@�C��������A�t�@�C�����J�����߂̃p�X��Ԃ��֐�
	// assetpath	:�A�Z�b�g�ւ̃p�X���󂯎��|�C���^ nullptr�ł��邱�ƁB������new[]�����̂ŁA�K��delete[]���邱��
	// dirpath		:�f�B���N�g���̃p�X
	// filename		:�A�Z�b�g�t�@�C����
	// ����̃C���[�W
	// assetpath = dirpath + filename
	void newArray_CopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename);


	// ������ϊ�
	// cText		:�W���̕�����@	 "������String"
	// wText		:���C�h������		L"������String"
	// u16Text		:UTF16������		u"������String"
	// ��1�������w���|�C���^�̐悪new[]�����̂ŕK��delete[]���邱��
	// SafeDelete()����
	void newArray_CreateMultiByteStrFromWideCharStr(char** cText, const wchar_t* const wText);
	void newArray_CreateMultiByteStrFromWideCharStr(char** cText, const char16_t* const u16Text);

	void newArray_CreateWideCharStrFromMultiByteStr(wchar_t** wText, const char* const cText);
	void newArray_CreateWideCharStrFromMultiByteStr(char16_t** u16Text, const char* const cText);

	// �ėp�|�C���^��p���������Ȍ^�ϊ�
	// ���炩�Ƀ�������̃o�C�i������v���Ă���ꍇ�ɂ̂ݎg�p����
	template <class To, class From>
	To strong_cast(From from)
	{
		assert(sizeof(To) == sizeof(From));
		return *(static_cast<To*>(static_cast<void*>(&from)));
	}
}

#endif // _SYSTEM_H_
/*
	Systemモジュール
	標準ライブラリやOSAPIにアクセスするインターフェース
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

	// 文字列比較
	bool StringEqual(const void* _str1, const void* _str2);

	// NULL文字を含む長さを返す
	int GetStringLength(const char* const text);

	// ディレクトリのパスを取得する関数
	// dirpath	ディレクトリのパスを受け取るポインタ nullptrであること
	//			内部でnewされるので、必ずdeleteすること
	// filepath 元のポインタ
	void NewArrayAndCopyDirPathFromFilePath(char** _dirpath, const char* const filepath);
	
	void NewArrayAndCopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename);
}

#endif // _SYSTEM_H_
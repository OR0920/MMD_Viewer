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
	bool StringEqual(const void* const _str1, const void* const _str2);

	// NULL文字を含む長さを返す
	int GetStringLength(const char* const text);

	// ディレクトリのパスを取得する関数
	// dirpath	:ディレクトリのパスを受け取るポインタ nullptrであること。内部でnew[]されるので、必ずdelete[]すること
	// filepath :元のポインタ
	void NewArrayAndCopyDirPathFromFilePath(char** _dirpath, const char* const filepath);
	
	// 特定のディレクトリへのパスとアセットファイル名から、ファイルを開くためのパスを返す関数
	// assetpath	:アセットへのパスを受け取るポインタ nullptrであること。内部でnew[]されるので、必ずdelete[]すること
	// dirpath		:ディレクトリのパス
	// filename		:アセットファイル名
	// 動作のイメージ
	// assetpath = dirpath + filename
	void NewArrayAndCopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename);
}

#endif // _SYSTEM_H_
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
	void newArray_CopyDirPathFromFilePath(char** _dirpath, const char* const filepath);
	
	// 特定のディレクトリへのパスとアセットファイル名から、ファイルを開くためのパスを返す関数
	// assetpath	:アセットへのパスを受け取るポインタ nullptrであること。内部でnew[]されるので、必ずdelete[]すること
	// dirpath		:ディレクトリのパス
	// filename		:アセットファイル名
	// 動作のイメージ
	// assetpath = dirpath + filename
	void newArray_CopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename);


	// 文字列変換
	// cText		:標準の文字列　	 "文字列String"
	// wText		:ワイド文字列		L"文字列String"
	// u16Text		:UTF16文字列		u"文字列String"
	// 第1引数が指すポインタの先がnew[]されるので必ずdelete[]すること
	// SafeDelete()推奨
	void newArray_CreateMultiByteStrFromWideCharStr(char** cText, const wchar_t* const wText);
	void newArray_CreateMultiByteStrFromWideCharStr(char** cText, const char16_t* const u16Text);

	void newArray_CreateWideCharStrFromMultiByteStr(wchar_t** wText, const char* const);
	void newArray_CreateWideCharStrFromMultiByteStr(char16_t** u16Text, const char* const);
}

#endif // _SYSTEM_H_
/*
	Systemモジュール
	標準ライブラリやOSAPIにアクセスするインターフェース
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include"File.h"
#include"DebugMessage.h"
#include"SystemTimer.h"

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

	template <class T>
	class varray
	{
	public:
		varray()
			:
			mLength(0),
			mData(nullptr)
		{

		}

		varray(const int count)
			:
			mLength(count),
			mData(new T[count])
		{

		}

		~varray()
		{
			SafeDeleteArray(&mData);
		}

		void Init(const int length)
		{
			mLength = length;
			mData = new T[length];
		}

		T& operator[](const int i)
		{
			assert((mData != nullptr) && "The Array is not initialized !");
			return mData[i];
		}

		const T& operator[](const int i) const
		{
			assert((mData != nullptr) && "The Array is not initialized !");
			return mData[i];
		}

		const T* GetStart() const
		{
			assert((mData != nullptr) && "The Array is not initialized !");
			return &mData[0];
		}

	private:
		varray(const varray& other);
		const varray& operator=(const varray& other) const;
		int mLength;
		T* mData;
	};

	void CheckMemoryLeak();

	// 文字列比較
	bool StringEqual(const char* const _str1, const char* const _str2);
	bool StringEqual(const wchar_t* const _str1, const wchar_t* const _str2);
	bool StringEqual(const char16_t* const _str1, const char16_t* const _str2);

	// NULL文字を含む長さを返す
	int GetStringLength(const char* const text);
	int GetStringLength(const wchar_t* const text);

	// ファイルの拡張を取得する
	const char* const GetExt(const char* const filename);
	const wchar_t* const GetExt(const wchar_t* const filename);

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

	void newArray_CreateWideCharStrFromMultiByteStr(wchar_t** wText, const char* const cText);
	void newArray_CreateWideCharStrFromMultiByteStr(char16_t** u16Text, const char* const cText);

	// 汎用ポインタを用いた強引な型変換
	// 明らかにメモリ状のバイナリが一致している場合にのみ使用する
	template <class To, class From>
	To strong_cast(From from)
	{
		assert(sizeof(To) == sizeof(From));
		return *(static_cast<To*>(static_cast<void*>(&from)));
	}
}

#endif // _SYSTEM_H_
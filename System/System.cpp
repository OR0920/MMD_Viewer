#include"System.h"
#include<string>

#include<Windows.h>

#define IS_USED_PTR(ptr)\
if(ptr != nullptr)\
{\
	DebugMessage("The " << ToString(ptr) << " is already Used");\
	return;\
}

bool System::StringEqual(const void* const _str1, const void* const _str2)
{
	if (_str1 == nullptr && _str2 == nullptr) return true;
	if (_str1 == nullptr) return false;
	if (_str2 == nullptr) return false;
	std::string str1(reinterpret_cast<const char*>(_str1));
	std::string str2(reinterpret_cast<const char*>(_str2));
	return str1 == str2;
}

void System::newArray_CopyDirPathFromFilePath(char** _dirpath, const char* const filepath)
{
	auto& dirpath = *_dirpath;
	IS_USED_PTR(dirpath);

	// ファイルパス末尾のID
	int pathLastID = GetStringLength(filepath) - 1;

	for (int i = pathLastID; i >= 0; --i)
	{
		if (filepath[i] == '/')
		{
			// ファイル名の最後のディレクトリ名までの文字数
			// 最後の'/'までの文字数 + NULL文字分　//
			const int dirPathLength = i + 2;

			dirpath = new char[dirPathLength] {'\0'};

			// 末尾のNULL文字は残し、それまでをコピー
			for (int j = 0; j < dirPathLength - 1; ++j)
			{
				dirpath[j] = filepath[j];
			}
			break;
		}
	}
}

// NULL文字を含む文字列の長さを返す	
int System::GetStringLength(const char* const text)
{
	if (text == nullptr)
	{
		return 0;
	}

	if (text[0] == '\0')
	{
		return 0;
	}

	int length = 1;
	for (length; text[length - 1] != '\0'; ++length);
	return length;
}

void System::newArray_CopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename)
{
	auto& assetpath = *_assetpath;
	IS_USED_PTR(assetpath);

	const int dirLengthNotIncludeNULL = GetStringLength(dirpath) - 1;
	const int filenameLength = GetStringLength(filename);
	const int assetPathLength = dirLengthNotIncludeNULL + filenameLength;

	assetpath = new char[assetPathLength] {'\0'};
	for (int i = 0; i < dirLengthNotIncludeNULL; ++i)
	{
		assetpath[i] = dirpath[i];
	}

	for (int i = 0; (dirLengthNotIncludeNULL + i) < assetPathLength; ++i)
	{
		assetpath[dirLengthNotIncludeNULL + i] = filename[i];
	}
}

static const char* const errorMessageWrongText = " ERROR : Wrong Text !";
static const char* const errorMessageFailedTranslate = " ERROR : Failed Translate !";

void System::newArray_CreateMultiByteStrFromWideCharStr(char** cText, const wchar_t* const wText)
{
	newArray_CreateMultiByteStrFromWideCharStr(cText, reinterpret_cast<const char16_t* const>(wText));
}


void System::newArray_CreateMultiByteStrFromWideCharStr(char** cText, const char16_t* const u16Text)
{
	IS_USED_PTR(*cText);
	auto size = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<LPCWSTR>(u16Text), -1, NULL, 0, NULL, NULL);
	if (size == 0)
	{
		DebugMessage(ToString(newArray_CreateMultiByteStrFromWideCharStr) << errorMessageWrongText);
	}
	*cText = new char[size] {};
	if (size != WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<LPCWSTR>(u16Text), -1, *cText, size, NULL, NULL))
	{
		DebugMessage(ToString(newArray_CreateMultiByteStrFromWideCharStr) << errorMessageFailedTranslate);
	};
}


void System::newArray_CreateWideCharStrFromMultiByteStr(wchar_t** wText, const char* const cText)
{
	newArray_CreateWideCharStrFromMultiByteStr(reinterpret_cast<char16_t**>(wText), cText);
}


void System::newArray_CreateWideCharStrFromMultiByteStr(char16_t** u16Text, const char* const cText)
{
	IS_USED_PTR(*u16Text);
	auto size = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(cText), -1, NULL, 0);
	if (size == 0)
	{
		DebugMessage(ToString(newArray_CreateWideCharStrFromMultiByteStr) << errorMessageWrongText);
		return;
	}
	*u16Text = new char16_t[size] {};
	if (size != MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(cText), -1, reinterpret_cast<LPWSTR>(*u16Text), size))
	{
		DebugMessage(ToString(newArray_CreateWideCharStrFromMultiByteStr) << errorMessageFailedTranslate);
		return;
	}
}
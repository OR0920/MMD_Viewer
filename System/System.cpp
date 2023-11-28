#include"System.h"
#include<string>

#include<Windows.h>

bool System::StringEqual(const void* const _str1, const void* const _str2)
{
	if (_str1 == nullptr && _str2 == nullptr) return true;
	if (_str1 == nullptr) return false;
	if (_str2 == nullptr) return false;
	std::string str1(reinterpret_cast<const char*>(_str1));
	std::string str2(reinterpret_cast<const char*>(_str2));
	return str1 == str2;
}

void System::NewArrayAndCopyDirPathFromFilePath(char** _dirpath, const char* const filepath)
{
	auto& dirpath = *_dirpath;
	if (dirpath != nullptr)
	{
		DebugMessage("The pointer is already used");
	}

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

void System::NewArrayAndCopyAssetPath(char** _assetpath, const char* const dirpath, const char* const filename)
{
	auto& assetpath = *_assetpath;

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


void System::CreateNewStringFrom_u16_to_c(char** cText, const char16_t* const u16Text)
{
	auto size = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)u16Text, -1, NULL, 0, NULL, NULL);
	*cText = new char[size] {};
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)u16Text, -1, (LPSTR)(*cText), size, NULL, NULL);
}

void System::CreateNewStringFrom_c_to_u16(char16_t** u16Text, const char* const cText)
{
	auto size = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(cText), -1, NULL, 0);
	if (size == 0)
	{
		DebugMessage("CreateNewStringFrom_c_to_u16() ERROR : Wrong Text!");
		return;
	}
	*u16Text = new char16_t[size];
	if (size == MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCCH>(cText), -1, reinterpret_cast<LPWSTR>(*u16Text), size))
	{
		DebugMessage("CreateNewStringFrom_c_to_u16() ERROR : Failed Translate !");
		return;
	}
}
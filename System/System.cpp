#include"System.h"
#include<string>

#include<Windows.h>

bool System::StringEqual(const void* const _str1, const void* const _str2)
{
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

	// �t�@�C���p�X������ID
	int pathLastID = GetStringLength(filepath) - 1;

	for (int i = pathLastID; i >= 0; --i)
	{
		if (filepath[i] == '/')
		{
			// �t�@�C�����̍Ō�̃f�B���N�g�����܂ł̕�����
			// �Ō��'/'�܂ł̕����� + NULL�������@//
			const int dirPathLength = i + 2;

			dirpath = new char[dirPathLength] {'\0'};

			// ������NULL�����͎c���A����܂ł��R�s�[
			for (int j = 0; j < dirPathLength - 1; ++j)
			{
				dirpath[j] = filepath[j];
			}
			break;
		}
	}
}

// NULL�������܂ޕ�����̒�����Ԃ�	
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
	auto bytesize = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)u16Text, -1, NULL, 0, NULL, NULL);
	*cText = new char[bytesize] {};
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)u16Text, -1, (LPSTR)(*cText), bytesize, NULL, NULL);
}
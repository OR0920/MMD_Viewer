#include"System.h"
#include<string>

bool System::StringEqual(const void* _str1, const void* _str2)
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

	// �p�X�̒����������̂���1�n�܂�
	int pathLength = 1;
	// �f�B���N�g���̎擾
	int dirCount = 0;

	for (pathLength; filepath[pathLength - 1] != '\0'; ++pathLength)
	{
		if (filepath[pathLength] == '/') ++dirCount;
	}

	for (int i = 0; i < pathLength; ++i)
	{
		if (filepath[i] == '/') --dirCount;
		if (dirCount == 0)
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

// NULL�������܂ޒ�����Ԃ�	
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
#include"System.h"


void System::CopyDirectoryPath(char** _dirpath, const char* const filepath)
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
#include"System.h"


void System::CopyDirectoryPath(char** _dirpath, const char* const filepath)
{
	auto& dirpath = *_dirpath;
	if (dirpath != nullptr)
	{
		DebugMessage("The pointer is already used");
	}

	// パスの長さ文字数のため1始まり
	int pathLength = 1;
	// ディレクトリの取得
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
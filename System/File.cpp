#include<cassert>
#include"File.h"

#include"DebugMessage.h"


using namespace System;

//コンストラクタでファイルオープン
FileWriteBin::FileWriteBin(const char* filepath)
	:
	mFile(filepath, std::fstream::binary)
{
	if (IsFileOpenSuccsess() == false)
	{
		DebugMessage(filepath << " is can't open !");
		//assert(false && "FileWrite open failed ! ");
		return;
	}

	DebugMessage(filepath << " is opened !");
}

FileWriteBin::~FileWriteBin()
{
	mFile.close();
}

bool FileWriteBin::IsFileOpenSuccsess() const
{
	return !mFile.fail();
}

void FileWriteBin::Close()
{
	mFile.close();
}


//コンストラクタでファイルオープン
FileReadBin::FileReadBin(const char* filepath)
	:
	mFile(filepath, std::fstream::binary)
{
	if (IsFileOpenSuccsess() == false)
	{
		DebugMessage(filepath << " is can't open !");
		//assert(false && "FileRead Open Failed ! ");
		return;
	}

	DebugMessage(filepath << " is opened !");
}

FileReadBin::~FileReadBin()
{
	mFile.close();
}


bool FileReadBin::IsFileOpenSuccsess() const
{
	return !mFile.fail();
}

void FileReadBin::Close()
{
	mFile.close();
}

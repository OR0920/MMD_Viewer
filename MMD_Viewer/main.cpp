#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
using namespace std;

using namespace System;


const char* testModelFilePath[] =
{
	"Test/Model/PMD/MEIKO.pmd",
	"Test/Model/PMD/�J�C�g.pmd",
	"Test/Model/PMD/�_�~�[�{�[��.pmd",
	"Test/Model/PMD/��������.pmd",
	"Test/Model/PMD/��������.pmd",
	"Test/Model/PMD/�㉹�n�N.pmd",
	"Test/Model/PMD/�������J.pmd",
	"Test/Model/PMD/�����~�NVer2.pmd",
	"Test/Model/PMX/���΂񂿂��/���΂񂿂��/���΂񂿂��.pmx",
	"Test/Model/PMX/�n�V�r���R�E/�n�V�r���R�E.pmx",
	"Test/Model/PMX/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W���� ver.2.3.1.pmx",
	"Test/Model/PMX/�L���E�V���E�G���Aver1.0/�L���E�V���E�G���A/1�b�Q�[�g�̂�.pmx"
};

const auto kabanPath = "Test/Model/PMX/���΂񂿂��/���΂񂿂��/���΂񂿂��.pmx";
const auto mikuPath = "Test/Model/PMD/�����~�NVer2.pmd";

void LoadAndCout(const char* filepath)
{
	MMDsdk::PmdFile model(filepath);
}


int main()
{
	SET_JAPANESE_ENABLE;
	//MMDsdk::PmdFile model(testModelFilePath[0]);

	auto filepathCount = sizeof(testModelFilePath) / sizeof(testModelFilePath[0]);
	for (int i = 0; i < filepathCount; ++i)
	{
		LoadAndCout(testModelFilePath[i]);
	}

	MMDsdk::PmxFile kaban(kabanPath);

	for (int i = 0; i < kaban.GetTextureCount(); ++i)
	{
		char* assetpath = nullptr;
		NewArrayAndCopyAssetPath(&assetpath, &kaban.GetDirectoryPathStart(), GetTextMacro(kaban.GetTexturePath(i)));

		DebugMessage(assetpath);
		FileReadBin tex(assetpath);
		tex.Close();

		SafeDeleteArray(&assetpath);
	}
	const char* filepath = "Test/Motion/�V���v���E�H�[�N.vmd";

	MMDsdk::VmdFile walk(filepath);
}

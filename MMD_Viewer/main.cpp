#include<iostream>
#include<iomanip>

#include<array>
#include<string>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
using namespace std;

//using namespace System;


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

using namespace MathUtil;

int main()
{
	SET_JAPANESE_ENABLE;

	System::CheckMemoryLeak();

	if (System::GUI::MainWindow::Instance().Create(1280, 720) == System::Result::FAIL)
	{
		return -1;
	}

	auto& fc = System::GUI::FileCatcher::Instance();
	if (fc.Create(System::GUI::MainWindow::Instance()) == System::Result::FAIL)
	{
		return -1;
	}

	while (System::GUI::MainWindow::Instance().IsClose() == false)
	{
		if (fc.Update() == true)
		{
			DebugOutString(fc.GetPath());
			DebugOutParam(fc.GetDropPos().x);
			DebugOutParam(fc.GetDropPos().y);
		}
	}
}

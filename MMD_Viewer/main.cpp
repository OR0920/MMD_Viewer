#include<iostream>
#include<iomanip>

#include<array>

#include"MathUtil.h"
#include"System.h"
#include"MMDsdk.h"
using namespace std;

using namespace System;


const char* testModelFilePath[] =
{
	"Model/PMD/MEIKO.pmd",

	"Model/PMD/�J�C�g.pmd",
	"Model/PMD/�_�~�[�{�[��.pmd",
	"Model/PMD/��������.pmd",
	"Model/PMD/��������.pmd",
	"Model/PMD/�㉹�n�N.pmd",
	
	"Model/PMD/�������J.pmd",
	"D:/_3DModel/MikuMikuDance_v932x64/MikuMikuDance_v932x64/UserFile/Model/Test/�����~�NVer2.pmd",
	"D:/_3DModel/MikuMikuDance_v932x64/MikuMikuDance_v932x64/UserFile/Model/Test/�����~�NVer2NotEng.pmd",
	"Model/PMD/�����~�NVer2.pmd",
	"D:/_3DModel/���΂񂿂��/���΂񂿂��/���΂񂿂��.pmx",
	
	"D:/_3DModel/�n�V�r���R�E/�n�V�r���R�E.pmx",
	"D:/_3DModel/�L���E�V���E�G���Aver1.0/�L���E�V���E�G���A/1�b�Q�[�g�̂�.pmx",
	"C:/Users/onory/Downloads/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W���� ver.2.3.1.pmx"
};


void LoadAndCout(const char* filepath)
{
	MMDsdk::PmxFile model(filepath);
	model.GetHeader().DebugOut();
	//model.DebugOutAllBone();
	//model.DebugOutVertex(0);
}

int main()
{
	SET_JAPANESE_ENABLE;
	//MMDsdk::PmdFile model(testModelFilePath[0]);

	auto filepathCount = sizeof(testModelFilePath) / sizeof(testModelFilePath[0]);
	for (int i = 0; i < filepathCount; ++i)
	{
	//	LoadAndCout(testModelFilePath[i]);
	}

	DebugMessage("branchtest");

	//MMDsdk::PmxFile kaban(testModelFilePath[10]);
	//MMDsdk::PmxFile miku(testModelFilePath[13]);

	//kaban.DebugOutAllMorph(true);

	//DebugOutParam(kaban.GetMaterialCount());
	//kaban.DebugOutAllTexturePath();
	//kaban.DebugOutAllMaterial();

	//MMDsdk::PmdFile miku(testModelFilePath[9]);

	//auto& text = miku.GetHeader().modelInfoEng.modelName;
	//

	//MMDsdk::PmxFile model(testModelFilePath[11]);
	//bool bdef1 = false;
	//bool bdef2 = false;
	//bool bdef4 = false;
	//bool sdef = false;

	//model.GetHeader().DebugOut();
	//for (int i = 0; i < model.GetVertexCount(); ++i)
	//{
	//	if (model.GetVertex(i).weightType == MMDsdk::PmxFile::Vertex::BDEF1 && bdef1 == false) { model.DebugOutVertex(i); bdef1 = true; }
	//	if (model.GetVertex(i).weightType == MMDsdk::PmxFile::Vertex::BDEF2 && bdef2 == false) { model.DebugOutVertex(i);  bdef2 = true; }
	//	if (model.GetVertex(i).weightType == MMDsdk::PmxFile::Vertex::BDEF4 && bdef4 == false) { model.DebugOutVertex(i); bdef4 = true; }
	//	if (model.GetVertex(i).weightType == MMDsdk::PmxFile::Vertex::SDEF && sdef == false) { model.DebugOutVertex(i); sdef = true; }
	//}


	//MMDsdk::PmdFile miku(testModelFilePath[9]);
	//miku.GetHeader().DebugOut();

	//MMDsdk::PmxFile kaban(testModelFilePath[10]);
	//kaban.GetHeader().DebugOut();

	//miku.GetHeader().DebugOut();
}

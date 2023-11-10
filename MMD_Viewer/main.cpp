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

bool strCmpForTest(const char* _str1, const char* _str2)
{
	DebugOutParam(_str1);
	DebugOutParam(_str2);

	for (int i = 0; true; ++i)
	{
		DebugOutArrayBin(_str1, i, 8);
		DebugOutArrayBin(_str2, i, 8);
		if (_str1[i] == '\0' && _str2[i] == '\0')break;
		if (_str1[i] == '\0')
		{
			if (_str2[i + 1] == '\0')
			{
				break;
			}
		}
		else
		{
			if (_str1[i + 1] == '\0')
			{
				break;
			}
		}
		if (_str1[i] != _str2[i]) return false;
		DebugMessageNewLine()
	}
	return true;
}

void LoadAndCout(const char* filepath)
{
	MMDsdk::PmdFile model(filepath);
	
	assert(strCmpForTest(GetText(model.GetBoneNameForDisplay(0)), "�h�j") == true);


	//int slashCount = 0;
	//for (int i = 0; filepath[i] != '\0'; ++i)
	//{
	//	if (filepath[i] == '/') ++slashCount;
	//}

	//int dirPathSize = 0;
	//for (dirPathSize = 0; filepath[dirPathSize] != '\0'; ++dirPathSize)
	//{
	//	if (slashCount == 0)
	//	{
	//		break;
	//	}
	//	if (filepath[dirPathSize] == '/')
	//	{
	//		--slashCount;			
	//	}
	//}

	//for (int j = 0; j < model.GetTextureCount(); ++j)
	//{
	//	auto& textureFileName = model.GetTexturePath(j);
	//	auto texPathLength = (dirPathSize + textureFileName.GetLength());
	//	char* texPath = new char[texPathLength] {'\0'};

	//	for (int i = 0; i < dirPathSize; ++i)
	//	{
	//		texPath[i] = filepath[i];
	//	}
	//	for (int i = 0; i < textureFileName.GetLength(); ++i)
	//	{
	//		texPath[i + dirPathSize] = (GetText(textureFileName))[i];
	//	}

	//	DebugMessage(texPath);

	//	FileReadBin texture(texPath);

	//	SafeDeleteArray(&texPath);
	//}
	

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

	MMDsdk::PmxFile stage(testModelFilePath[filepathCount - 1]);
	//stage.DebugOutAllData();

	//MMDsdk::PmxFile kaban(kabanPath);
	//
	//kaban.DebugOutAllJoint();

	//MMDsdk::PmdFile miku(mikuPath);

	//miku.DebugOutAllData();

	//kaban.DebugOutAllDisplayFrame();

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

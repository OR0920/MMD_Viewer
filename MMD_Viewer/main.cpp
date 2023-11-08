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
	//"Test/Model/PMD/MEIKO.pmd",
	//"Test/Model/PMD/�J�C�g.pmd",
	//"Test/Model/PMD/�_�~�[�{�[��.pmd",
	//"Test/Model/PMD/��������.pmd",
	//"Test/Model/PMD/��������.pmd",
	//"Test/Model/PMD/�㉹�n�N.pmd",
	//"Test/Model/PMD/�������J.pmd",
	//"Test/Model/PMD/�����~�NVer2.pmd",
	"Test/Model/PMX/���΂񂿂��/���΂񂿂��/���΂񂿂��.pmx",
	"Test/Model/PMX/�n�V�r���R�E/�n�V�r���R�E.pmx",
	"Test/Model/PMX/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W���� ver.2.3.1.pmx",
	//"Model/PMX/�L���E�V���E�G���Aver1.0/�L���E�V���E�G���A/1�b�Q�[�g�̂�.pmx"
};


void LoadAndCout(const char* filepath)
{
	MMDsdk::PmxFile model(filepath);
	model.GetHeader().DebugOut();

	model.DebugOutAllMorph();

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
	//	auto texPathLength = dirPathSize + textureFileName.GetLength();
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

	MMDsdk::PmdFile a(testModelFilePath[0]);

	auto& b = a;

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

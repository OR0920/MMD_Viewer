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
	"Test/Model/PMD/MEIKO.pmd",
	"Test/Model/PMD/カイト.pmd",
	"Test/Model/PMD/ダミーボーン.pmd",
	"Test/Model/PMD/鏡音リン.pmd",
	"Test/Model/PMD/鏡音レン.pmd",
	"Test/Model/PMD/弱音ハク.pmd",
	"Test/Model/PMD/巡音ルカ.pmd",
	"Test/Model/PMD/初音ミクVer2.pmd",
	//"Test/Model/PMX/かばんちゃん/かばんちゃん/かばんちゃん.pmx",
	//"Test/Model/PMX/ハシビロコウ/ハシビロコウ.pmx",
	//"Test/Model/PMX/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン ver.2.3.1.pmx"
	//"Model/PMX/キョウシュウエリアver1.0/キョウシュウエリア/1話ゲートのみ.pmx"
};


void LoadAndCout(const char* filepath)
{
	MMDsdk::PmdFile model(filepath);
	//DebugMessage(GetText(model.GetToonTexturePath(0)));

	for (int k = 0; k < 10; ++k)
	{
		auto& texpath = model.GetToonTexturePath(k);
		
		DebugOutString(GetText(texpath));
		
		int slashCount = 0;
		for (int i = 0; filepath[i] != '\0'; ++i)
		{
			if (filepath[i] == '/')
			{
				++slashCount;
			}
		}

		int slashCount2 = 0;
		char* modelAssetPath = nullptr;
		
		for (int i = 0; filepath[i] != '\0'; ++i)
		{
			//DebugOutArray(filepath, i);
			if (filepath[i] == '/')
			{
				++slashCount2;
				if (slashCount == slashCount2)
				{
					// 添え字から文字列長さを求める。1始まりに修正
					int dirLength = i + 1;
					modelAssetPath = new char[dirLength + texpath.GetLength()];
					for (int j = 0; j < dirLength; ++j)
					{
						modelAssetPath[j] = filepath[j];
					}
					for (int j = 0; j < texpath.GetLength(); ++j)
					{
						modelAssetPath[j + dirLength] = (GetText(texpath))[j];
					}
					break;
				}
			}
		}
		
		DebugMessage(modelAssetPath);
		FileReadBin file(modelAssetPath);

		SafeDeleteArray(&modelAssetPath);
	}
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

	//MMDsdk::PmxFile kaban(testModelFilePath[10]);
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

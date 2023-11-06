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
	"../x64/Debug/Test/Model/PMD/MEIKO.pmd",

	"../x64/Debug/Test/Model/PMD/カイト.pmd",
	"../x64/Debug/Test/Model/PMD/ダミーボーン.pmd",
	"../x64/Debug/Test/Model/PMD/鏡音リン.pmd",
	"../x64/Debug/Test/Model/PMD/鏡音レン.pmd",
	"../x64/Debug/Test/Model/PMD/弱音ハク.pmd",
	"../x64/Debug/Test/Model/PMD/巡音ルカ.pmd",
	"../x64/Debug/Test/Model/PMD/初音ミクVer2.pmd",
	"../x64/Debug/Test/Model/PMX/かばんちゃん/かばんちゃん/かばんちゃん.pmx",
	"../x64/Debug/Test/Model/PMX/ハシビロコウ/ハシビロコウ.pmx",
	"../x64/Debug/Test/Model/PMX/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン ver.2.3.1.pmx",
	//"Model/PMX/キョウシュウエリアver1.0/キョウシュウエリア/1話ゲートのみ.pmx"
};


void LoadAndCout(const char* filepath)
{
	MMDsdk::PmxFile model(filepath);
	model.GetHeader().DebugOut();
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

	MMDsdk::PmxFile kaban(testModelFilePath[10]);
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

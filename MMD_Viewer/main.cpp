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
	"Test/Model/PMD/カイト.pmd",
	"Test/Model/PMD/ダミーボーン.pmd",
	"Test/Model/PMD/鏡音リン.pmd",
	"Test/Model/PMD/鏡音レン.pmd",
	"Test/Model/PMD/弱音ハク.pmd",
	"Test/Model/PMD/巡音ルカ.pmd",
	"Test/Model/PMD/初音ミクVer2.pmd",
	"Test/Model/PMX/かばんちゃん/かばんちゃん/かばんちゃん.pmx",
	"Test/Model/PMX/ハシビロコウ/ハシビロコウ.pmx",
	"Test/Model/PMX/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン/Appearance Miku_大人バージョン ver.2.3.1.pmx",
	"Test/Model/PMX/キョウシュウエリアver1.0/キョウシュウエリア/1話ゲートのみ.pmx"
};

const auto kabanPath = "Test/Model/PMX/かばんちゃん/かばんちゃん/かばんちゃん.pmx";
const auto mikuPath = "Test/Model/PMD/初音ミクVer2.pmd";

void LoadAndCout(const char* filepath)
{
	MMDsdk::PmdFile model(filepath);
}

using namespace MathUtil;

int main()
{
	SET_JAPANESE_ENABLE;

	float2 f = { 3.f, 4.f }, xf{};
	auto v = Vector::GenerateVectorNormalized(f);

	auto xv = DirectX::XMVector2Normalize(DirectX::XMVectorSet(3.f, 4.f, 0.f, 0.f));
	DirectX::XMStoreFloat2(&xf, xv);


	Vector v2 = f;
	v2.Vector2Normalize();

	DebugOutVector(v2);
	DebugOutFloat2(xf);
}

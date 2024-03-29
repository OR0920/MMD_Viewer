#include<cassert>

#include"MMDsdk.h"
#include"System.h"

#ifdef _DEBUG
// pmxファイル専用 頂点以外、-1を非参照値として記録する
#define ID_IS_NO_REF(i) if (i == -1)\
{\
	DebugMessage("index " << i << " is no ref");\
	return {};\
}
// データを持たない場合、初期化の値を返す
#define ARRAY_HAS_NO_DATA(dataPtr, count)\
if(dataPtr == nullptr || count == 0)\
{\
	DebugMessage(ToString(dataPtr) << " has No Data ! ");\
	return {};\
}
#define DebugOutFloat2(f)\
DebugOutParam(f.x);\
DebugOutParam(f.y);
#define DebugOutFloat3(f)\
DebugOutFloat2(f);\
DebugOutParam(f.z);
#define DebugOutFloat4(f)\
DebugOutFloat3(f);\
DebugOutParam(f.w);
#else
#define ID_IS_NO_REF(i)
#define ARRAY_HAS_NO_DATA(dataPtr, count)
#define DebugOutFloat2
#define DebugOutFloat3
#define DebugOutFloat4
#endif // _DEBUG

// 汎用ポインタから、ファイル読み込み型への変換
// ヘッダから型名を隠ぺいするため //
#define GetFile(_file) *reinterpret_cast<System::FileReadBin*>(_file);

using namespace MMDsdk;
using System::SafeDeleteArray;


//可変長テキストバッファの実装

TextBufferVariable::TextBufferVariable() : mLength(0), mStr(nullptr) {}
TextBufferVariable::~TextBufferVariable()
{
	System::SafeDeleteArray(&mStr);
}

void TextBufferVariable::Load(void* _file, EncodeType encode)
{
	// 2重にロードさせない
	if (mStr != nullptr)
	{
		DebugMessage("This Buffer is Already Loaded");
		return;
	}


	auto& file = GetFile(_file);
	int binaryLength = 0;
	file.Read(binaryLength);

	// ワイドNULL文字分
	char* s16 = new char[binaryLength + 2] { '\0' };
	file.ReadArray(s16, binaryLength);

	if (encode == EncodeType::UTF16)
	{
		System::newArray_CreateMultiByteStrFromWideCharStr(&mStr, reinterpret_cast<char16_t*>(s16));

		// apiベタ打ちなので後でラッピングする
		//auto bytesize = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)s16, -1, NULL, 0, NULL, NULL);
		//mStr = new char[bytesize] {};
		//WideCharToMultiByte(CP_ACP, 0, (LPWSTR)s16, -1, (LPSTR)mStr, bytesize, NULL, NULL);
		mLength = System::GetStringLength(mStr);
	}

	SafeDeleteArray(&s16);
}

const int TextBufferVariable::GetLength() const
{
	return mLength;
}

const char* const TextBufferVariable::GetText() const
{
	return mStr;
}

template <size_t size>
void LoadTextBufferFixed(System::FileReadBin& file, TextBufferFixed<size>& text)
{
	// 宣言時に長さが確定しているためテンプレートクラスにしたい
	// ヘッダーに他コンポーネントのインクルードを書きたくない
	// 文字列をconstにして安全にアクセスさせたい
	// 上記理由により const_cast を採用。
	file.ReadArray
	(
		const_cast<char*>(text.GetText()),
		text.GetLength()
	);
}


PmdFile::PmdFile(const char* const filepath)
	:
	isSuccess(false),
	mDirectoryPath(nullptr),
	mHeader(),
	mVertexCount(0),
	mVertex(nullptr),
	mIndexCount(0),
	mIndex(nullptr),
	mMaterialCount(0),
	mMaterial(nullptr),
	mBoneCount(0),
	mBone(nullptr),
	mIKCount(0),
	mIK(nullptr),
	mMorphCount(0),
	mMorph(nullptr),
	mMorphForDisplayCount(0),
	mMorphForDisplay(nullptr),
	mBoneNameForDisplayCount(0),
	mBoneNameForDisplay(nullptr),
	mBoneNameForDisplayEng(nullptr),
	mBoneForDisplayCount(0),
	mBoneForDisplay(nullptr),
	mToonTexturePath(),
	mRigitbodyCount(0),
	mRigitbody(nullptr),
	mJointCount(0),
	mJoint(nullptr)
{
	System::FileReadBin file(filepath);

	// ファイルパスそのものが間違い
	if (file.IsFileOpenSuccsess() == false)
	{
		file.Close();
		return;
	}

	// 以下読み込み 

	// ヘッダの読み込み

	// フォーマット情報
	// 外部からは使用しない //
	const char pmdFormat[3] = { 'P', 'm', 'd' };
	char format[3] = {};
	file.ReadArray(format, 3);
	if (
		format[0] != pmdFormat[0] ||
		format[1] != pmdFormat[1] ||
		format[2] != pmdFormat[2]
		)
	{
		// 間違ったファイル
		DebugMessage("Not Pmd File !");
		file.Close();
		return;
	}

	// ディレクトリのパスを取得する
	System::newArray_CopyDirPathFromFilePath(&mDirectoryPath, filepath);

	file.Read(mHeader.version);

	LoadTextBufferFixed(file, mHeader.modelInfoJP.modelName);
	LoadTextBufferFixed(file, mHeader.modelInfoJP.comment);

	//頂点の読み込み
	file.Read(mVertexCount);
	mVertex = new Vertex[mVertexCount]{};
	for (uint16_t i = 0; i < mVertexCount; ++i)
	{
		auto& v = mVertex[i];
		file.Read(v.position);
		file.Read(v.normal);
		file.Read(v.uv);
		v.LoadBoneID(&file);
		file.Read(v.weight);
		file.Read(v.edgeFlag);
	}

	//インデックス読み込み
	file.Read(mIndexCount);
	mIndex = new unsigned short[mIndexCount] {};
	file.ReadArray(mIndex, mIndexCount);

	//マテリアル読み込み
	file.Read(mMaterialCount);
	mMaterial = new Material[mMaterialCount]{};
	for (uint32_t i = 0; i < mMaterialCount; ++i)
	{
		auto& m = mMaterial[i];

		//4バイト単位なのでまとめて読み込む
		file.Read(m.diffuse, 44);

		//それぞれ1バイト単位
		file.Read(m.toonIndex);
		file.Read(m.edgeFlag);

		file.Read(m.vertexCount, 4);

		LoadTextBufferFixed(file, m.texturePath);
	}

	//ボーン読み込み
	file.Read(mBoneCount);
	mBone = new Bone[mBoneCount]{};
	for (uint32_t i = 0; i < mBoneCount; ++i)
	{
		auto& b = mBone[i];
		LoadTextBufferFixed(file, b.name);
		file.Read(b.parentIndex);
		file.Read(b.childIndex);
		file.Read(b.type);
		file.Read(b.ikParentIndex);
		file.Read(b.headPos);
	}

	//IKデータ読み込み
	file.Read(mIKCount);
	mIK = new IK_Data[mIKCount];
	for (uint32_t i = 0; i < mIKCount; ++i)
	{
		auto& ik = mIK[i];
		file.Read(ik.ikBoneIndex);
		file.Read(ik.ikTargetBoneIndex);
		file.Read(ik.ikChainCount);
		file.Read(ik.iterations);
		file.Read(ik.controlWeight);
		ik.LoadIkChildBoneID(&file);
	}

	// 表情データ読み込み
	file.Read(mMorphCount);
	mMorph = new Morph[mMorphCount]{};
	for (uint16_t i = 0; i < mMorphCount; ++i)
	{
		auto& mph = mMorph[i];

		LoadTextBufferFixed(file, mph.name);
		file.Read(mph.offsCount);
		file.Read(mph.type);

		mph.LoadSkinVertex(&file);
	}

	// モーフ用表示枠データ読み込み
	file.Read(mMorphForDisplayCount);
	mMorphForDisplay = new uint16_t[mMorphForDisplayCount]{};
	file.ReadArray(mMorphForDisplay, mMorphForDisplayCount);

	// 表示用ボーン名データ読み込み
	file.Read(mBoneNameForDisplayCount);
	mBoneNameForDisplay =
		new TextBufferFixed<50>[mBoneNameForDisplayCount] {};

	for (uint8_t i = 0; i < mBoneNameForDisplayCount; ++i)
	{
		LoadTextBufferFixed(file, mBoneNameForDisplay[i]);
	}

	// ボーン枠用枠名の読み込み
	file.Read(mBoneForDisplayCount);

	mBoneForDisplay = new BoneForDisplay[mBoneForDisplayCount]{};
	for (uint32_t i = 0; i < mBoneForDisplayCount; ++i)
	{
		file.Read(mBoneForDisplay[i].boneIndexForDisplay);
		file.Read(mBoneForDisplay[i].boneDisplayFrameIndex);
	}


	// 拡張機能 英名
	file.Read(mHeader.isEngAvailable);

	if (mHeader.isEngAvailable == 1)
	{
		// モデル情報 //
		LoadTextBufferFixed(file, mHeader.modelInfoEng.modelName);
		LoadTextBufferFixed(file, mHeader.modelInfoEng.comment);

		// ボーン名
		for (uint16_t i = 0; i < mBoneCount; ++i)
		{
			LoadTextBufferFixed(file, mBone[i].nameEng);
		}

		// 表情名(モーフ名)
		// baseは含まれないので添え字は1から
		for (uint16_t i = 1; i < mMorphCount; ++i)
		{
			LoadTextBufferFixed(file, mMorph[i].nameEng);
		}

		// 表示用ボーン名
		// センターは含まれない //
		mBoneNameForDisplayEng = new TextBufferFixed<50>[mBoneNameForDisplayCount] {};
		for (uint8_t i = 0; i < mBoneNameForDisplayCount; ++i)
		{
			LoadTextBufferFixed(file, mBoneNameForDisplayEng[i]);
		}
	}

	// Toonテクスチャパス読み込み
	for (int i = 0; i < 10; ++i)
	{
		LoadTextBufferFixed(file, mToonTexturePath[i]);
	}


	// 物理演算用剛体データ読み込み
	file.Read(mRigitbodyCount);
	mRigitbody = new Rigitbody[mRigitbodyCount]{};
	for (uint32_t i = 0; i < mRigitbodyCount; ++i)
	{
		auto& r = mRigitbody[i];
		LoadTextBufferFixed(file, r.name);
		file.Read(r.relationshipBoneID);
		file.Read(r.group);
		file.Read(r.groupTarget);
		file.Read(r.shapeType);
		file.Read(r.shapeW);
		file.Read(r.shapeH);
		file.Read(r.shapeD);
		file.Read(r.position);
		file.Read(r.rotation);
		file.Read(r.weight);
		file.Read(r.positionDim);
		file.Read(r.rotationDim);
		file.Read(r.recoil);
		file.Read(r.friction);
		file.Read(r.type);
	}

	// ジョイントデータ読み込み
	file.Read(mJointCount);
	mJoint = new Joint[mJointCount]{};
	for (uint32_t i = 0; i < mJointCount; ++i)
	{
		auto& j = mJoint[i];
		LoadTextBufferFixed(file, j.name);

		file.Read(j.rigitbodyIndexA);
		file.Read(j.rigitbodyIndexB);

		file.Read(j.position);
		file.Read(j.rotation);

		file.Read(j.posLowerLimit);
		file.Read(j.posUpperLimit);

		file.Read(j.rotLowerLimit);
		file.Read(j.rotUpperLimit);

		file.Read(j.springPos);
		file.Read(j.springRot);
	}
	file.Close();

	isSuccess = true;
}


PmdFile::~PmdFile()
{
	SafeDeleteArray(&mJoint);
	SafeDeleteArray(&mRigitbody);
	SafeDeleteArray(&mBoneNameForDisplayEng);
	SafeDeleteArray(&mBoneForDisplay);
	SafeDeleteArray(&mBoneNameForDisplay);

	SafeDeleteArray(&mMorphForDisplay);
	SafeDeleteArray(&mMorph);
	SafeDeleteArray(&mIK);
	SafeDeleteArray(&mBone);
	SafeDeleteArray(&mMaterial);

	SafeDeleteArray(&mIndex);
	SafeDeleteArray(&mVertex);

	SafeDeleteArray(&mDirectoryPath);
}

bool PmdFile::IsSuccessLoad() const
{
	return isSuccess;
}

const char* const PmdFile::GetDirectoryPath() const
{
	return mDirectoryPath;
}

PmdFile::Header::ModelInfo::ModelInfo() {}
PmdFile::Header::ModelInfo::~ModelInfo() {}

// コンソール出力関数、内部にif, forなどが入る場合は、デバッグ時のみ実装
void PmdFile::Header::DebugOut() const
{
#ifdef _DEBUG
	DebugMessage("PMDFile /////////////////////////////////////////");
	DebugMessage(modelInfoJP.modelName.GetText());
	DebugMessageNewLine();
	DebugMessage(modelInfoJP.comment.GetText());
	DebugMessageNewLine();
	if (isEngAvailable == 1)
	{
		DebugMessage(modelInfoEng.modelName.GetText());
		DebugMessage(modelInfoEng.comment.GetText());
	}
	DebugMessageNewLine();
	DebugOutParam(version);
	DebugMessage("/////////////////////////////////////////////////");
	DebugMessageNewLine();
#endif // _DEBUG
}

PmdFile::Header::Header() {}
PmdFile::Header::~Header() {}

const PmdFile::Header& PmdFile::GetHeader() const
{
	return mHeader;
}

void PmdFile::DebugOutHeader() const
{
#ifdef _DEBUG
	if(IsSuccessLoad() == true)	mHeader.DebugOut();
#endif // _DEBUG
}

const uint32_t& PmdFile::GetVertexCount() const
{
	return mVertexCount;
}

const uint16_t& PmdFile::Vertex::GetBoneID(const int i) const
{
	ARRAY_HAS_NO_DATA(boneID, 2);
	IS_OUT_OF_RANGE(boneID, i, 2);
	return boneID[i];
}

void PmdFile::Vertex::LoadBoneID(void* _file)
{
	auto& file = GetFile(_file);
	file.ReadArray(boneID, 2);
}

void PmdFile::Vertex::DebugOut() const
{
	DebugOutFloat3(position);
	DebugOutFloat3(normal);
	DebugOutFloat2(uv);
	DebugOutArrayI(boneID, 0);
	DebugOutArrayI(boneID, 1);
	DebugOutParamI(weight);
	DebugOutParamI(edgeFlag);
	DebugMessageNewLine();
}

PmdFile::Vertex::Vertex() {}
PmdFile::Vertex::~Vertex() {}

const PmdFile::Vertex& PmdFile::GetVertex(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mVertex, mVertexCount);
	IS_OUT_OF_RANGE(mVertex, i, mVertexCount);
	return mVertex[i];
}

const uint32_t PmdFile::GetLastVertexID() const
{
	return mVertexCount - 1;
}

void PmdFile::DebugOutVertex(const uint32_t i) const
{
	DebugMessage("Vertex [" << i << "]");
	GetVertex(i).DebugOut();
}

void PmdFile::DebugOutAllVertex() const
{
#ifdef _DEBUG
	for (uint32_t i = 0; i < mVertexCount; ++i)
	{
		DebugOutVertex(i);
	}
#endif // _DEBUG
}

const uint32_t& PmdFile::GetIndexCount() const
{
	return mIndexCount;
}

const uint16_t& PmdFile::GetIndex(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mIndex, mIndexCount);
	IS_OUT_OF_RANGE(mIndex, i, mIndexCount);
	return mIndex[i];
}

const uint32_t PmdFile::GetLastIndexID() const
{
	return mIndexCount - 1;
}

void PmdFile::DebugOutIndexData(const uint32_t i) const
{
	DebugMessage("Index [" << i << "]");
	DebugOutParamI(GetIndex(i));
	DebugMessageNewLine();
}

void PmdFile::DebugOutAllIndexData() const
{
#ifdef _DEBUG
	for (uint32_t i = 0; i < mIndexCount; ++i)
	{
		DebugOutIndexData(i);
	}
#endif // _DEBUG
}

const uint32_t& PmdFile::GetMaterialCount() const
{
	return mMaterialCount;
}

void PmdFile::Material::DebugOut() const
{
	DebugOutFloat4(diffuse);
	DebugOutParam(specularity);
	DebugOutFloat3(specular);
	DebugOutFloat3(ambient);
	DebugOutParamI(toonIndex);
	DebugOutParamI(edgeFlag);
	DebugOutParamI(vertexCount);
	DebugOutString(texturePath.GetText());
	DebugMessageNewLine();
}

PmdFile::Material::Material() {}
PmdFile::Material::~Material() {}

const PmdFile::Material& PmdFile::GetMaterial(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mMaterial, mMaterialCount);
	IS_OUT_OF_RANGE(mMaterial, i, mMaterialCount);
	return mMaterial[i];
}

const uint32_t PmdFile::GetLastMaterialID() const
{
	return mMaterialCount - 1;
}

void PmdFile::DebugOutMaterial(const uint32_t i) const
{
	DebugMessage("Material[" << i << "]");
	GetMaterial(i).DebugOut();
}

void PmdFile::DebugOutAllMaterial() const
{
#ifdef _DEBUG
	for (uint32_t i = 0; i < mMaterialCount; ++i)
	{
		DebugOutMaterial(i);
	}
#endif // _DEBUG
}

const uint16_t& PmdFile::GetBoneCount() const
{
	return mBoneCount;
}

void PmdFile::Bone::DebugOut() const
{
	DebugOutString(name.GetText());
	DebugOutString(nameEng.GetText());
	DebugOutParamI(parentIndex);
	DebugOutParamI(childIndex);
	DebugOutParamI(type);
	DebugOutParamI(ikParentIndex);
	DebugOutFloat3(headPos);
	DebugMessageNewLine();
}

PmdFile::Bone::Bone() {}
PmdFile::Bone::~Bone() {}

const PmdFile::Bone& PmdFile::GetBone(const uint16_t i) const
{
	ARRAY_HAS_NO_DATA(mBone, mBoneCount);
	IS_OUT_OF_RANGE(mBone, i, mBoneCount);
	return mBone[i];
}

const uint32_t PmdFile::GetLastBoneID() const
{
	return mBoneCount - 1;
}

void PmdFile::DebugOutBone(const uint16_t i) const
{
	DebugMessage("Bone[" << i << "]");
	GetBone(i).DebugOut();
}

void PmdFile::DebugOutAllBone() const
{
#ifdef _DEBUG
	for (int i = 0; i < mBoneCount; ++i)
	{
		DebugOutBone(i);
	}
#endif // _DEBUG
}

const uint16_t& PmdFile::GetIKCount() const
{
	return mIKCount;
}

const uint16_t& PmdFile::IK_Data::GetIkChildBoneID(const int i) const
{
	ARRAY_HAS_NO_DATA(ikChildBoneIndexArray, ikChainCount);
	IS_OUT_OF_RANGE(ikChildBoneIndexArray, i, ikChainCount);
	return ikChildBoneIndexArray[i];
}

void PmdFile::IK_Data::LoadIkChildBoneID(void* _file)
{
	// 汎用ポインタをキャスト
	auto& file = GetFile(_file);
	// 配列データ読み込み
	ikChildBoneIndexArray = new uint16_t[ikChainCount];
	file.ReadArray(ikChildBoneIndexArray, ikChainCount);
}

void PmdFile::IK_Data::DebugOut() const
{
	DebugOutParamI(ikBoneIndex);
	DebugOutParamI(ikTargetBoneIndex);
	DebugOutParamI(ikChainCount);
	DebugOutParamI(iterations);
	DebugOutParam(controlWeight);
	for (int i = 0; i < ikChainCount; ++i)
	{
		DebugOutArray(ikChildBoneIndexArray, i);
	}
	DebugMessageNewLine();
}

PmdFile::IK_Data::IK_Data() {}
PmdFile::IK_Data::~IK_Data()
{
	SafeDeleteArray(&ikChildBoneIndexArray);
}

const PmdFile::IK_Data& PmdFile::GetIKData(const uint16_t i) const
{
	ARRAY_HAS_NO_DATA(mIK, mIKCount);
	IS_OUT_OF_RANGE(mIK, i, mIKCount);
	return mIK[i];
}

const uint16_t PmdFile::GetLastIKDataID() const
{
	return mIKCount - 1;
}

void PmdFile::DebugOutIKData(const uint16_t i) const
{
	DebugMessage("IKData [" << i << "]");
	GetIKData(i).DebugOut();
}

void PmdFile::DebugOutAllIK() const
{
#ifdef _DEBUG
	for (uint16_t i = 0; i < mIKCount; ++i)
	{
		DebugOutIKData(i);
	}
#endif // _DEBUG
}

const uint16_t& PmdFile::GetMorphCount() const
{
	return mMorphCount;
}

const uint16_t PmdFile::GetMorphCountNotIncludeBase() const
{
	return mMorphCount - 1;
}

PmdFile::Morph::MorphOffsData::MorphOffsData() {}
PmdFile::Morph::MorphOffsData::~MorphOffsData() {}
PmdFile::Morph::MorphBaseData::MorphBaseData() {}
PmdFile::Morph::MorphBaseData::~MorphBaseData() {}

PmdFile::Morph::MorphData::MorphData() {}
PmdFile::Morph::MorphData::~MorphData() {}

const PmdFile::Morph::MorphBaseData& PmdFile::Morph::GetMorphBaseData(const int i) const
{
	ARRAY_HAS_NO_DATA(morphData, offsCount);
	IS_OUT_OF_RANGE(morphData, i, offsCount);
	return morphData[i].base;
}

const PmdFile::Morph::MorphOffsData& PmdFile::Morph::GetMorphOffsData(const int i) const
{
	ARRAY_HAS_NO_DATA(morphData, offsCount);
	IS_OUT_OF_RANGE(morphData, i, offsCount);
	return morphData[i].offs;
}


void PmdFile::Morph::LoadSkinVertex(void* _file)
{
	auto& file = GetFile(_file);
	morphData = new MorphData[offsCount]{};
	file.ReadArray(morphData, offsCount);
}

void PmdFile::Morph::DebugOut(bool isOutVertexData) const
{
#ifdef _DEBUG
	DebugOutString(name.GetText());
	DebugOutString(nameEng.GetText());
	DebugOutParamI(offsCount);
	DebugOutParamI(type);
	if (isOutVertexData == true)
	{
		for (uint32_t i = 1; i < offsCount; ++i)
		{
			auto& sv = morphData[i];
			DebugMessage(ToString(morphData) << "[" << i << "]");
			if (type == MorphType::MT_BASE)
			{
				DebugOutParamI(sv.base.index);
				DebugOutFloat3(sv.base.positon);
			}
			else
			{
				DebugOutParamI(sv.offs.baseIndex);
				DebugOutFloat3(sv.offs.offsPosition);
			}
			DebugMessageNewLine();
		}
	}
	DebugMessageNewLine();
#endif // _DEBUG
}

PmdFile::Morph::Morph() {}
PmdFile::Morph::~Morph()
{
	SafeDeleteArray(&morphData);
}

const PmdFile::Morph& PmdFile::GetMorph(const uint16_t i) const
{
	ARRAY_HAS_NO_DATA(mMorph, mMorphCount);
	IS_OUT_OF_RANGE(mMorph, i, mMorphCount);
	return mMorph[i];
}

const uint16_t PmdFile::GetLastMorphID() const
{
	return mMorphCount - 1;
}

void PmdFile::DebugOutMorph(const uint16_t i, bool isOutVertexData) const
{
	DebugMessage("Morph [" << i << "]");
	GetMorph(i).DebugOut(isOutVertexData);
}

void PmdFile::DebugOutAllMorph(bool isOutVertexData) const
{
#ifdef _DEBUG
	for (uint16_t i = 0; i < mMorphCount; ++i)
	{
		DebugOutMorph(i, isOutVertexData);
	}
#endif // _DEBUG
}

const uint8_t& PmdFile::GetMorphIndexForDisplayCount() const
{
	return mMorphForDisplayCount;
}

const uint16_t& PmdFile::GetMorphIndexForDisplay(const uint8_t i) const
{
	ARRAY_HAS_NO_DATA(mMorphForDisplay, mMorphForDisplayCount);
	IS_OUT_OF_RANGE(mMorphForDisplay, i, mMorphForDisplayCount);
	return mMorphForDisplay[i];
}

const uint8_t PmdFile::GetLastMorphIndexForDisplayID() const
{
	return mMorphForDisplayCount - 1;
}

void PmdFile::DebugOutMorphIndexForDisplay(const uint8_t i) const
{
	DebugMessage
	(
		ToString(mMorphForDisplay) << "[" << static_cast<int>(i) << "] = " << GetMorphIndexForDisplay(i)
	);

	DebugMessage
	(
		" -> " <<
		GetMorph(GetMorphIndexForDisplay(i)).name.GetText()		
	);
	DebugMessageNewLine();
}

void PmdFile::DebugOutAllMorphIndexForDisplay() const
{
#ifdef _DEBUG
	for (uint8_t i = 0; i < mMorphForDisplayCount; ++i)
	{
		DebugOutMorphIndexForDisplay(i);
	}
#endif // _DEBUG
}

const uint8_t& PmdFile::GetBoneNameForDisplayCount() const
{
	return mBoneNameForDisplayCount;
}

const TextBufferFixed<50>& PmdFile::GetBoneNameForDisplay(const uint8_t i) const
{
	ARRAY_HAS_NO_DATA(mBoneNameForDisplay, mBoneNameForDisplayCount);
	IS_OUT_OF_RANGE(mBoneNameForDisplay, i, mBoneNameForDisplayCount);
	return mBoneNameForDisplay[i];
}

const uint8_t PmdFile::GetLastBoneNameForDisplayID() const
{
	return GetBoneNameForDisplayCount() - 1;
}

const TextBufferFixed<50>& PmdFile::GetBoneNameForDisplayEng(const uint8_t i) const
{
	ARRAY_HAS_NO_DATA(mBoneNameForDisplayEng, mBoneNameForDisplayCount);
	IS_OUT_OF_RANGE(mBoneNameForDisplayEng, i, mBoneNameForDisplayCount);
	return mBoneNameForDisplayEng[i];
}


void PmdFile::DebugOutBoneNameForDisplay(const uint8_t i) const
{
	DebugMessage(ToString(mBoneNameForDisplay) << "[" << static_cast<int>(i) << "] = " << GetBoneNameForDisplay(i).GetText());
	DebugMessage(ToString(mBoneNameForDisplayEng) << "[" << static_cast<int>(i) << "] = " << GetBoneNameForDisplayEng(i).GetText());
	DebugMessageNewLine();
}

void PmdFile::DebugOutAllBoneNameForDisplay() const
{
#ifdef _DEBUG
	for (uint8_t i = 0; i < mBoneNameForDisplayCount; ++i)
	{
		DebugOutBoneNameForDisplay(i);
	}
#endif // _DEBUG
}

const uint32_t& PmdFile::GetBoneForDisplayCount() const
{
	return mBoneForDisplayCount;
}

void PmdFile::BoneForDisplay::DebugOut() const
{
	DebugOutParamI(boneIndexForDisplay);
	DebugOutParamI(boneDisplayFrameIndex);
	DebugMessageNewLine();
}

PmdFile::BoneForDisplay::BoneForDisplay() {}
PmdFile::BoneForDisplay::~BoneForDisplay() {}

const PmdFile::BoneForDisplay& PmdFile::GetBoneForDisplay(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mBoneForDisplay, mBoneForDisplayCount);
	IS_OUT_OF_RANGE(mBoneForDisplay, i, mBoneForDisplayCount);
	return mBoneForDisplay[i];
}

const uint32_t PmdFile::GetLastBoneForDisplayID() const
{
	return mBoneForDisplayCount - 1;
}

void PmdFile::DebugOutBoneForDisplay(const uint32_t i) const
{
	DebugMessage(ToString(mBoneForDisplay) << "[" << static_cast<int>(i) << "]");
	GetBoneForDisplay(i).DebugOut();
}

void PmdFile::DebugOutAllBoneForDisplay() const
{
#ifdef _DEBUG
	for (uint16_t i = 0; i < mBoneForDisplayCount; ++i)
	{
		DebugOutBoneForDisplay(i);
	}
#endif // _DEBUG
}

const TextBufferFixed<100>& PmdFile::GetToonTexturePath(const uint8_t i) const
{
	IS_OUT_OF_RANGE(mToonTexturePath, i, mToonTexturePath[i].GetLength());
	return mToonTexturePath[i];
}

void PmdFile::DebugOutToonTexturePath(const uint8_t i) const
{
	DebugMessage(ToString(mToonTexturePath) << "[" << static_cast<int>(i) << "] = " << GetToonTexturePath(i).GetText());
	DebugMessageNewLine();
}

void PmdFile::DebugOutAllToonTexturePath() const
{
#ifdef _DEBUG
	for (auto i = 0; i < 10; ++i)
	{
		DebugOutToonTexturePath(i);
	}
#endif // _DEBUG
}

const uint32_t& PmdFile::GetRigitbodyCount() const
{
	return mRigitbodyCount;
}


#define Rad2Deg 360 / (3.14159265358979f * 2)

inline const float3 RadianToDegreeFloat3(float3 rot)
{
	float3 ret{};

	ret.x = rot.x * Rad2Deg;
	ret.y = rot.y * Rad2Deg;
	ret.z = rot.z * Rad2Deg;

	return ret;
}

inline const float3 DegreeToRagianFloat3(float3 f3)
{
	float3 ret{};

	ret.x = f3.x / Rad2Deg;
	ret.y = f3.y / Rad2Deg;
	ret.z = f3.z / Rad2Deg;

	return ret;
}

void PmdFile::Rigitbody::DebugOut() const
{
	DebugOutString(name.GetText());
	DebugOutParamI(relationshipBoneID);
	DebugOutParamI(group);
	DebugOutParamBin(groupTarget, 16);
	DebugOutParamI(shapeType);
	DebugOutParam(shapeW);
	DebugOutParam(shapeH);
	DebugOutParam(shapeD);
	DebugOutFloat3(position);
	DebugOutFloat3(RadianToDegreeFloat3(rotation));
	DebugOutParam(weight);
	DebugOutParam(positionDim);
	DebugOutParam(rotationDim);
	DebugOutParam(recoil);
	DebugOutParam(friction);
	DebugOutParamI(type);
	DebugMessageNewLine();
}

PmdFile::Rigitbody::Rigitbody() {}
PmdFile::Rigitbody::~Rigitbody() {}

const PmdFile::Rigitbody& PmdFile::GetRigitbody(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mRigitbody, mRigitbodyCount);
	IS_OUT_OF_RANGE(mRigitbody, i, mRigitbodyCount);
	return mRigitbody[i];
}

const uint32_t PmdFile::GetLastRigitbodyID() const
{
	return mRigitbodyCount - 1;
}

void PmdFile::DebugOutRigitbody(const uint32_t i) const
{
	DebugMessage("Rigtbody [" << i << "]");
	GetRigitbody(i).DebugOut();
}

void PmdFile::DebugOutAllRigitbody() const
{
#ifdef _DEBUG
	for (uint32_t i = 0; i < mRigitbodyCount; ++i)
	{
		DebugOutRigitbody(i);
	}
#endif // _DEBUG
}

const uint32_t& PmdFile::GetJointCount() const
{
	return mJointCount;
}


void PmdFile::Joint::DebugOut() const
{
	DebugOutString(name.GetText());

	DebugOutParamI(rigitbodyIndexA);
	DebugOutParamI(rigitbodyIndexB);

	DebugOutFloat3(position);
	DebugOutFloat3(rotation);

	DebugOutFloat3(posLowerLimit);
	DebugOutFloat3(posUpperLimit);

	DebugOutFloat3(RadianToDegreeFloat3(rotLowerLimit));
	DebugOutFloat3(RadianToDegreeFloat3(rotUpperLimit));

	DebugOutFloat3(springPos);
	DebugOutFloat3(springRot);


	DebugMessageNewLine();
}

PmdFile::Joint::Joint() {}
PmdFile::Joint::~Joint() {}

const PmdFile::Joint& PmdFile::GetJoint(const uint32_t i) const
{
	ARRAY_HAS_NO_DATA(mJoint, mJointCount);
	IS_OUT_OF_RANGE(mJoint, i, mJointCount);
	return mJoint[i];
}

const uint32_t PmdFile::GetLastJointID() const
{
	return mJointCount - 1;
}

void PmdFile::DebugOutJoint(const uint32_t i) const
{
	DebugMessage("Joint [" << i << "]");
	GetJoint(i).DebugOut();
}

void PmdFile::DebugOutAllJoint() const
{
#ifdef _DEBUG
	for (uint32_t i = 0; i < mJointCount; ++i)
	{
		DebugOutJoint(i);
	}
#endif // _DEBUG
}

void PmdFile::DebugOutAllData() const
{
	GetHeader().DebugOut();
	DebugOutParamI(mVertexCount);
	DebugOutAllVertex();
	DebugOutParamI(mIndexCount);
	DebugOutAllIndexData();
	DebugOutParamI(mMaterialCount);
	DebugOutAllMaterial();
	DebugOutParamI(mBoneCount);
	DebugOutAllBone();
	DebugOutParamI(mIKCount);
	DebugOutAllIK();
	DebugOutParamI(mMorphCount);
	DebugOutAllMorph(true);
	DebugOutParamI(mMorphForDisplayCount);
	DebugOutAllMorphIndexForDisplay();
	DebugOutParamI(mBoneNameForDisplayCount);
	DebugOutAllBoneNameForDisplay();
	DebugOutParamI(mBoneForDisplayCount);
	DebugOutAllBoneForDisplay();
	DebugOutAllToonTexturePath();
	DebugOutParamI(mRigitbodyCount);
	DebugOutAllRigitbody();
	DebugOutParamI(mJointCount);
	DebugOutAllJoint();
}


//pmx////////////////////////////////////////////////////////////

// インデックス系のデータはエディタ側で1, 2, 4バイトで最適化されるため
// バイト数に応じて、読み込み方を変える
// 全て4バイトの整数型で保持する 
void LoadID_AsInt32(System::FileReadBin& file, int32_t& buf, const size_t idByteSize)
{
	buf = 0;
	switch (idByteSize)
	{
	case 1:
	{
		int8_t t = 0;
		file.Read(t);
		buf = t;
		break;
	}
	case 2:
	{
		int16_t t = 0;
		file.Read(t);
		buf = t;
		break;
	}
	case 4:
	{
		int32_t t = 0;
		file.Read(t);
		buf = t;
		break;
	}
	default:
		break;
	}
}

PmxFile::PmxFile(const char* const filepath)
	:
	mIsSuccess(false),
	mDirectoryPath(nullptr),
	mHeader(),
	mVertexCount(0),
	mVertex(nullptr),
	mIndexCount(0),
	mIndex(nullptr),
	mTextureCount(0),
	mTexturePath(nullptr),
	mMaterialCount(0),
	mMaterial(nullptr),
	mBoneCount(0),
	mBone(nullptr),
	mMorphCount(0),
	mMorph(nullptr),
	mDisplayFrameCount(0),
	mDisplayFrame(nullptr),
	mRigitbodyCount(0),
	mRigitbody(nullptr),
	mJointCount(0),
	mJoint(nullptr)
{
	System::FileReadBin file(filepath);

	if (file.IsFileOpenSuccsess() == false)
	{
		file.Close();
		return;
	}

	// ヘッダ情報の読み込み

	uint8_t format[4] = {};
	uint8_t pmxFormat[4] = { 'P', 'M', 'X', ' ' };
	file.ReadArray(format, 4);

	if (format[0] == 'P' && format[1] == 'm' && format[2] == 'x' && format[3] == ' ') {}
	else if (format[0] == 'P' && format[1] == 'M' && format[2] == 'X' && format[3] == ' ') {}
	else
	{
		// 間違ったファイル
		DebugMessage("Not Pmx File !");
		file.Close();
		return;
	}

	System::newArray_CopyDirPathFromFilePath(&mDirectoryPath, filepath);

	file.Read(mHeader.version, 4);
	file.Read(mHeader.fileConfigLength);
	file.Read(mHeader.encode, mHeader.fileConfigLength);

	mHeader.modelInfoJP.modelName.Load(&file, mHeader.encode);
	mHeader.modelInfoEng.modelName.Load(&file, mHeader.encode);
	mHeader.modelInfoJP.comment.Load(&file, mHeader.encode);
	mHeader.modelInfoEng.comment.Load(&file, mHeader.encode);

	// 頂点読み込み
	file.Read(mVertexCount);
	mVertex = new Vertex[mVertexCount]{};
	for (int i = 0; i < mVertexCount; ++i)
	{
		auto& v = mVertex[i];
		file.Read(v.position);
		file.Read(v.normal);
		file.Read(v.uv);
		v.LoadAddtionalUV(&file, mHeader.additionalUVcount);
		file.Read(v.weightType);
		v.LoadBoneIDAndWeight(&file, mHeader.boneID_Size);
		file.Read(v.edgeRate);
	}

	// インデックス読み込み
	file.Read(mIndexCount);
	mIndex = new int32_t[mIndexCount]{};
	file.ReadArray(mIndex, mIndexCount, mHeader.vertexID_Size);

	// テクスチャ読み込み
	file.Read(mTextureCount);
	mTexturePath = new TextBufferVariable[mTextureCount];

	for (int i = 0; i < mTextureCount; ++i)
	{
		mTexturePath[i].Load(&file, mHeader.encode);
	}

	//DebugOutAllTexturePath();

	// マテリアル読み込み
	file.Read(mMaterialCount);
	mMaterial = new Material[mMaterialCount]{};
	for (int i = 0; i < mMaterialCount; ++i)
	{
		auto& m = mMaterial[i];
		m.name.Load(&file, mHeader.encode);
		m.nameEng.Load(&file, mHeader.encode);
		file.Read(m.diffuse);
		file.Read(m.specular);
		file.Read(m.specularity);
		file.Read(m.ambient);
		file.Read(m.drawConfig);
		file.Read(m.edgeColor);
		file.Read(m.edgeSize);
		LoadID_AsInt32(file, m.textureID, mHeader.textureID_Size);
		LoadID_AsInt32(file, m.sphereTextureID, mHeader.textureID_Size);
		file.Read(m.sphereMode);
		file.Read(m.toonMode);
		LoadID_AsInt32(file, m.toonTextureID, mHeader.textureID_Size);
		m.memo.Load(&file, mHeader.encode);
		file.Read(m.vertexCount);
	}

	// ボーン読み込み
	file.Read(mBoneCount);
	mBone = new Bone[mBoneCount]{};
	for (int i = 0; i < mBoneCount; ++i)
	{
		auto& b = mBone[i];
		b.name.Load(&file, mHeader.encode);
		b.nameEng.Load(&file, mHeader.encode);
		file.Read(b.position);
		LoadID_AsInt32(file, b.parentBoneID, mHeader.boneID_Size);
		file.Read(b.transformHierarchy);
		file.Read(b.bc);
		if (b.GetBoneConfig(Bone::BoneConfig::BC_IS_LINK_DEST_BY_BONE) == true)
		{
			LoadID_AsInt32(file, b.linkDestBoneID, mHeader.boneID_Size);
		}
		else
		{
			file.Read(b.positionOffs);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_ADD_ROT) == true)
		{
			LoadID_AsInt32
			(
				file,
				b.addRot.addPalentBoneID,
				mHeader.boneID_Size
			);
			file.Read(b.addRot.addRatio);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_ADD_MOV) == true)
		{
			LoadID_AsInt32
			(
				file,
				b.addMov.addPalentBoneID,
				mHeader.boneID_Size
			);
			file.Read(b.addMov.addRatio);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_LOCK_AXIS) == true)
		{
			file.Read(b.axisDirection);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_LOCAL_AXIS) == true)
		{
			file.Read(b.localAxisX_Direction);
			file.Read(b.localAxisZ_Direction);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_OUT_PARENT_TRANSFORM) == true)
		{
			file.Read(b.outParentTransformKey);
		}
		if (b.GetBoneConfig(Bone::BoneConfig::BC_IK) == true)
		{
			LoadID_AsInt32(file, b.ikTargetBoneID, mHeader.boneID_Size);
			file.Read(b.ikLoopCount);
			file.Read(b.ikRotateLimit);
			file.Read(b.ikLinkCount);
			b.LoadIK_Link(&file, mHeader.boneID_Size);
		}
	}

	// モーフ読み込み
	file.Read(mMorphCount);
	mMorph = new Morph[mMorphCount]{};
	for (int i = 0; i < mMorphCount; ++i)
	{
		auto& mph = mMorph[i];
		mph.name.Load(&file, mHeader.encode);
		mph.nameEng.Load(&file, mHeader.encode);
		file.Read(mph.type);
		file.Read(mph.typeEX);
		file.Read(mph.offsCount);
		// モーフの種類によってLoadOffsData()の第2引数が変わる
		// このクラスはあくまでファイル構造をそのまま公開するという
		// 意図で設計しているため、内部構造体で継承は使用しない //
		switch (mph.typeEX)
		{
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_GROUP:
			mph.LoadOffsData(&file, mHeader.morphID_Size);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_VERTEX:
			mph.LoadOffsData(&file, mHeader.vertexID_Size);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_BONE:
			mph.LoadOffsData(&file, mHeader.boneID_Size);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_UV:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_1:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_2:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_3:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_4:
			mph.LoadOffsData(&file, mHeader.vertexID_Size);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_MATERIAL:
			mph.LoadOffsData(&file, mHeader.materialID_Size);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_NONE:
			break;
		default:
			break;
		}
	}


	// 表示枠データの読み込み
	file.Read(mDisplayFrameCount);
	mDisplayFrame = new DisplayFrame[mDisplayFrameCount]{};
	for (int i = 0; i < mDisplayFrameCount; ++i)
	{
		auto& d = mDisplayFrame[i];

		d.name.Load(&file, mHeader.encode);
		d.nameEng.Load(&file, mHeader.encode);
		file.Read(d.type);
		file.Read(d.frameElementCount);
		d.LoadFrameElement(&file, mHeader.boneID_Size, mHeader.morphID_Size);
	}

	// 剛体の読み込み
	file.Read(mRigitbodyCount);
	mRigitbody = new Rigitbody[mRigitbodyCount]{};
	for (int i = 0; i < mRigitbodyCount; ++i)
	{
		auto& r = mRigitbody[i];

		r.name.Load(&file, mHeader.encode);
		r.nameEng.Load(&file, mHeader.encode);
		LoadID_AsInt32(file, r.relationshipBoneID, mHeader.boneID_Size);
		file.Read(r.group);
		file.Read(r.groupTarget);
		file.Read(r.shapeType);
		file.Read(r.shapeW);
		file.Read(r.shapeH);
		file.Read(r.shapeD);
		file.Read(r.position);
		file.Read(r.rotation);
		file.Read(r.weight);
		file.Read(r.positionDim);
		file.Read(r.rotationDim);
		file.Read(r.recoil);
		file.Read(r.friction);
		file.Read(r.type);
	}

	file.Read(mJointCount);
	mJoint = new Joint[mJointCount]{};
	for (int i = 0; i < mJointCount; ++i)
	{
		auto& j = mJoint[i];
		j.name.Load(&file, mHeader.encode);
		j.nameEng.Load(&file, mHeader.encode);
		file.Read(j.type);
		LoadID_AsInt32(file, j.rigitbodyIndexA, mHeader.rigitbodyID_Size);
		LoadID_AsInt32(file, j.rigitbodyIndexB, mHeader.rigitbodyID_Size);
		file.Read(j.position);
		file.Read(j.rotation);
		file.Read(j.posLowerLimit);
		file.Read(j.posUpperLimit);
		file.Read(j.rotLowerLimit);
		file.Read(j.rotUpperLimit);
		file.Read(j.springPos);
		file.Read(j.springRot);
	}

	file.Close();
	mIsSuccess = true;
}

PmxFile::~PmxFile()
{
	SafeDeleteArray(&mJoint);
	SafeDeleteArray(&mRigitbody);
	SafeDeleteArray(&mDisplayFrame);
	SafeDeleteArray(&mMorph);
	SafeDeleteArray(&mBone);
	SafeDeleteArray(&mMaterial);
	SafeDeleteArray(&mTexturePath);
	SafeDeleteArray(&mIndex);
	SafeDeleteArray(&mVertex);
	SafeDeleteArray(&mDirectoryPath);
}

bool PmxFile::IsSuccessLoad() const
{
	return mIsSuccess;
}

const char* const PmxFile::GetDirectoryPath() const
{
	return mDirectoryPath;
}

PmxFile::Header::ModelInfo::ModelInfo() {}
PmxFile::Header::ModelInfo::~ModelInfo() {}

void PmxFile::Header::DebugOut() const
{
	DebugMessage("PMXFile /////////////////////////////////////////");
	DebugMessage("<<<<ModelName>>>");
	DebugMessage(modelInfoJP.modelName.GetText());
	DebugMessage("<<<<ModelName Eng>>>");
	DebugMessage(modelInfoEng.modelName.GetText());
	DebugMessageNewLine();
	DebugMessage("<<<<Comment>>>");
	DebugMessage(modelInfoJP.comment.GetText());
	DebugMessageNewLine();
	DebugMessage("<<<<Comment Eng>>>");
	DebugMessage(modelInfoEng.comment.GetText());
	DebugMessageNewLine();
	DebugOutParam(version);
	DebugOutParamI(encode);
	DebugOutParamI(additionalUVcount);
	DebugOutParamI(vertexID_Size);
	DebugOutParamI(textureID_Size);
	DebugOutParamI(materialID_Size);
	DebugOutParamI(boneID_Size);
	DebugOutParamI(morphID_Size);
	DebugOutParamI(rigitbodyID_Size);
	DebugMessageNewLine();
	DebugMessage("/////////////////////////////////////////////////");
	DebugMessageNewLine();
}

PmxFile::Header::Header() {}
PmxFile::Header::~Header() {}

const PmxFile::Header& PmxFile::GetHeader() const
{
	return mHeader;
}

void PmxFile::DebugOutHeader() const
{
#ifdef _DEBUG
	if (IsSuccessLoad() == true)
	{
		mHeader.DebugOut();
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetVertexCount() const
{
	return mVertexCount;
}

const float4& PmxFile::Vertex::GetAddtionalUV(const int32_t i) const
{
	IS_OUT_OF_RANGE(addtionalUV, i, 4);
	return addtionalUV[i];
}


const int32_t& PmxFile::Vertex::GetBoneID(const int32_t i) const
{
	IS_OUT_OF_RANGE(boneID, i, 4);
	return boneID[i];
}

const float& PmxFile::Vertex::GetWeight(const int32_t i) const
{
	IS_OUT_OF_RANGE(weight, i, 4);
	return weight[i];
}


void PmxFile::Vertex::LoadAddtionalUV(void* _file, const int32_t addtionalUVCount)
{
	auto& file = GetFile(_file);
	file.ReadArray(addtionalUV, addtionalUVCount);
}

void PmxFile::Vertex::LoadBoneIDAndWeight(void* _file, const uint8_t boneID_Size)
{
	auto& file = GetFile(_file);
	switch (weightType)
	{
	case MMDsdk::PmxFile::Vertex::BDEF1:
		LoadID_AsInt32(file, boneID[0], boneID_Size);
		//file.Read(boneID[0], boneID_Size);
		weight[0] = 1.f;
		break;
	case MMDsdk::PmxFile::Vertex::BDEF2:
		LoadID_AsInt32(file, boneID[0], boneID_Size);
		LoadID_AsInt32(file, boneID[1], boneID_Size);
		file.Read(weight[0]);
		weight[1] = 1 - weight[0];
		break;
	case MMDsdk::PmxFile::Vertex::BDEF4:
		for (int i = 0; i < 4; ++i)	LoadID_AsInt32(file, boneID[i], boneID_Size);
		for (int i = 0; i < 4; ++i)	file.Read(weight[i]);
		break;
	case MMDsdk::PmxFile::Vertex::SDEF:
		LoadID_AsInt32(file, boneID[0], boneID_Size);
		LoadID_AsInt32(file, boneID[1], boneID_Size);
		file.Read(weight[0]);
		weight[1] = 1 - weight[0];
		file.Read(sdefC);
		file.Read(sdefR0);
		file.Read(sdefR1);
		break;
	case MMDsdk::PmxFile::Vertex::WEIGHT_TYPE:
		break;
	default:
		break;
	}
}

void PmxFile::Vertex::DebugOut() const
{
	DebugOutFloat3(position);
	DebugOutFloat3(normal);
	DebugOutFloat2(uv);
	DebugOutParamI(weightType);
	for (int i = 0; i < 4; ++i)
	{
		DebugOutArrayI(boneID, i);
		DebugOutArray(weight, i);
	}
	if (weightType == SDEF)
	{
		DebugOutFloat3(sdefC);
		DebugOutFloat3(sdefR0);
		DebugOutFloat3(sdefR1);
	}
	DebugOutParam(edgeRate);

	DebugMessageNewLine();
}

PmxFile::Vertex::Vertex() {}
PmxFile::Vertex::~Vertex() {}

const PmxFile::Vertex& PmxFile::GetVertex(const int32_t i) const
{
	ARRAY_HAS_NO_DATA(mVertex, mVertexCount);
	IS_OUT_OF_RANGE(mVertex, i, mVertexCount);
	return mVertex[i];
}

const int32_t PmxFile::GetLastVertexID() const
{
	return mVertexCount - 1;
}

void PmxFile::DebugOutVertex(const int32_t i) const
{
	DebugMessage("Vertex [" << i << "]");
	GetVertex(i).DebugOut();
}

void PmxFile::DebugOutAllVertex() const
{
#ifdef _DEBUG
	for (int i = 0; i < mVertexCount; ++i)
	{
		DebugOutVertex(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetIndexCount() const
{
	return mIndexCount;
}

const int32_t& PmxFile::GetIndex(const int32_t i) const
{
	ARRAY_HAS_NO_DATA(mIndex, mIndexCount);
	IS_OUT_OF_RANGE(mIndex, i, mIndexCount);
	return mIndex[i];
}

const int32_t PmxFile::GetLastIndexID() const
{
	return mIndexCount - 1;
}

void PmxFile::DebugOutIndexData(const int32_t i) const
{
	DebugMessage("Index [" << i << "]");
	DebugOutParamI(GetIndex(i));
	DebugMessageNewLine();
}

void PmxFile::DebugOutAllIndexData() const
{
#ifdef _DEBUG
	for (int i = 0; i < mIndexCount; ++i)
	{
		DebugOutIndexData(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetTextureCount() const
{
	return mTextureCount;
}

const TextBufferVariable& PmxFile::GetTexturePath(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mTexturePath, mTextureCount);
	IS_OUT_OF_RANGE(mTexturePath, i, mTextureCount);
	return mTexturePath[i];
}

const int32_t PmxFile::GetLastTextureID() const
{
	return mTextureCount - 1;
}

void PmxFile::DebugOutTexturePath(const int32_t i) const
{
	DebugMessage("Texture [" << i << "]");
	DebugMessage(GetTexturePath(i).GetText());
	DebugMessageNewLine();
}

void PmxFile::DebugOutAllTexturePath() const
{
#ifdef _DEBUG
	for (int i = 0; i < mTextureCount; ++i)
	{
		DebugOutTexturePath(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetMaterialCount() const
{
	return mMaterialCount;
}

bool PmxFile::Material::GetDrawConfig(DrawConfig bitflag) const
{
	return drawConfig & bitflag;
}

void PmxFile::Material::DebugOut() const
{
	DebugMessage(name.GetText());
	DebugMessage(nameEng.GetText());
	DebugOutFloat4(diffuse);
	DebugOutFloat3(specular);
	DebugOutParam(specularity);
	DebugOutFloat3(ambient);
	DebugOutParamBin(drawConfig, 8);
	DebugOutFloat4(edgeColor);
	DebugOutParam(edgeSize);
	DebugOutParamI(textureID);
	DebugOutParamI(sphereTextureID);
	DebugOutParamI(sphereMode);
	DebugOutParamI(toonMode);
	DebugOutParamI(toonTextureID);
	DebugMessage("<<< memo >>>");
	DebugMessage(memo.GetText());
	DebugOutParamI(vertexCount / 3);
	DebugMessageNewLine();
}

PmxFile::Material::Material() {}
PmxFile::Material::~Material() {}

const PmxFile::Material& PmxFile::GetMaterial(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mMaterial, mMaterialCount);
	IS_OUT_OF_RANGE(mMaterial, i, mMaterialCount);
	return mMaterial[i];
}

const int32_t PmxFile::GetLastMaterialID() const
{
	return mMaterialCount - 1;
}


void PmxFile::DebugOutMaterial(const int32_t i) const
{
	DebugMessage("Material[" << i << "]");
	GetMaterial(i).DebugOut();
}

void PmxFile::DebugOutAllMaterial() const
{
#ifdef _DEBUG
	for (int i = 0; i < mMaterialCount; ++i)
	{
		DebugOutMaterial(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetBoneCount() const
{
	return mBoneCount;
}

PmxFile::Bone::AddData::AddData() {}
PmxFile::Bone::AddData::~AddData() {}

PmxFile::Bone::IK_Link::IK_Link() {}
PmxFile::Bone::IK_Link::~IK_Link() {}

bool PmxFile::Bone::GetBoneConfig(BoneConfig bitFlag) const
{
	return bc & bitFlag;
}

const PmxFile::Bone::IK_Link& PmxFile::Bone::GetIK_Link(const int32_t i) const
{
	ARRAY_HAS_NO_DATA(ikLink, ikLinkCount);
	IS_OUT_OF_RANGE(ikLink, i, ikLinkCount);
	return ikLink[i];
}

void PmxFile::Bone::LoadIK_Link(void* _file, const size_t boneID_Size)
{
	auto& file = GetFile(_file);
	ikLink = new IK_Link[ikLinkCount]{};
	for (int i = 0; i < ikLinkCount; ++i)
	{
		auto& ikl = ikLink[i];
		LoadID_AsInt32(file, ikl.linkBoneID, boneID_Size);
		file.Read(ikl.rotLimitConfig);
		if (ikl.rotLimitConfig == IK_Link::IK_RLC_ON)
		{
			file.Read(ikl.lowerLimit);
			file.Read(ikl.upperLimit);
		}
	}
}

void PmxFile::Bone::DebugOut() const
{
#ifdef _DEBUG
	DebugMessage(name.GetText());
	DebugMessage(nameEng.GetText());
	DebugOutFloat3(position);
	DebugOutParamI(parentBoneID);
	DebugOutParamI(transformHierarchy);
	DebugOutParamBin(bc, 16);
	if (GetBoneConfig(BC_IS_LINK_DEST_BY_BONE) == true)
	{
		DebugOutParamI(linkDestBoneID);
	}
	else
	{
		DebugOutFloat3(positionOffs);
	}
	DebugOutParamI(addRot.addPalentBoneID);
	DebugOutParam(addRot.addRatio);
	DebugOutParamI(addMov.addPalentBoneID);
	DebugOutParam(addMov.addRatio);
	DebugOutFloat3(axisDirection);
	DebugOutFloat3(localAxisX_Direction);
	DebugOutFloat3(localAxisZ_Direction);
	DebugOutParamI(outParentTransformKey);
	DebugOutParamI(ikTargetBoneID);
	DebugOutParamI(ikLoopCount);
	DebugOutParam(Rad2Deg * ikRotateLimit);
	DebugOutParamI(ikLinkCount);
	for (int i = 0; i < ikLinkCount; ++i)
	{
		auto& ikl = GetIK_Link(i);
		DebugOutParamI(ikl.linkBoneID);
		DebugOutParamI(ikl.rotLimitConfig);
		DebugOutFloat3(ikl.lowerLimit);
		DebugOutFloat3(ikl.upperLimit);
	}
	DebugMessageNewLine();
#endif // _DEBUG
}

PmxFile::Bone::Bone() {}

PmxFile::Bone::~Bone()
{
	//DebugMessage("pmx bone deleted");
	SafeDeleteArray(&ikLink);
}

const PmxFile::Bone& PmxFile::GetBone(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mBone, mBoneCount);
	IS_OUT_OF_RANGE(mBone, i, mBoneCount);
	return mBone[i];
}


const int32_t PmxFile::GetLastBoneID() const
{
	return mBoneCount - 1;
}


void PmxFile::DebugOutBone(const int32_t i) const
{
	DebugMessage("Bone[" << i << "]");
	GetBone(i).DebugOut();
}

void PmxFile::DebugOutAllBone() const
{
#ifdef _DEBUG
	for (int i = 0; i < mBoneCount; ++i)
	{
		DebugOutBone(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetMorphCount() const
{
	return mMorphCount;
}

void PmxFile::Morph::GroupOffs::DebugOut() const
{
	DebugOutParamI(morphID);
	DebugOutParam(morphRatio);
}

PmxFile::Morph::GroupOffs::GroupOffs() {}
PmxFile::Morph::GroupOffs::~GroupOffs() {}

void PmxFile::Morph::VertexOffs::DebugOut() const
{
	DebugOutParamI(vertexID);
	DebugOutFloat3(offsPos);
}

PmxFile::Morph::VertexOffs::VertexOffs() {}
PmxFile::Morph::VertexOffs::~VertexOffs() {}

void PmxFile::Morph::BoneOffs::DebugOut() const
{
	DebugOutParamI(boneID);
	DebugOutFloat3(offsPos);
	DebugOutFloat4(offsRotQ);
}

PmxFile::Morph::BoneOffs::BoneOffs() {}
PmxFile::Morph::BoneOffs::~BoneOffs() {}

void PmxFile::Morph::UV_Offs::DebugOut() const
{
	DebugOutParamI(vertexID);
	DebugOutFloat4(uv);
}

PmxFile::Morph::UV_Offs::UV_Offs() {}
PmxFile::Morph::UV_Offs::~UV_Offs() {}

void PmxFile::Morph::MaterialOffs::DebugOut() const
{
	DebugOutParamI(materialID);
	switch (offsType)
	{
	case MMDsdk::PmxFile::Morph::MaterialOffs::OT_MUL:
		DebugOutString("OT_MUL");
		break;
	case MMDsdk::PmxFile::Morph::MaterialOffs::OT_ADD:
		DebugOutString("OT_ADD");
		break;
	case MMDsdk::PmxFile::Morph::MaterialOffs::OT_NONE:
		DebugOutString("OT_NONE");
		break;
	default:
		break;
	}
	DebugOutFloat4(diffuse);
	DebugOutFloat3(specular);
	DebugOutParam(specularity);
	DebugOutFloat3(ambient);
	DebugOutFloat4(edgeColor);
	DebugOutParam(edgeSize);
	DebugOutFloat4(textureScale);
	DebugOutFloat4(sphereTextureScale);
	DebugOutFloat4(toonTextureScale);
}

PmxFile::Morph::MaterialOffs::MaterialOffs() {}
PmxFile::Morph::MaterialOffs::~MaterialOffs() {}

PmxFile::Morph::MorphOffsData::MorphOffsData() {}
PmxFile::Morph::MorphOffsData::~MorphOffsData() {}

const PmxFile::Morph::MorphOffsData& PmxFile::Morph::GetMorphOffsData(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(morphOffsData, offsCount);
	IS_OUT_OF_RANGE(morphOffsData, i, offsCount);
	return morphOffsData[i];
}

void PmxFile::Morph::LoadOffsData(void* _file, const size_t idByteSize)
{
	auto& file = GetFile(_file);
	morphOffsData = new MorphOffsData[offsCount]{};

	// モーフの種類に応じて各自読み込む
	// インターフェイスをシンプルにするため関数では分けない。
	for (int i = 0; i < offsCount; ++i)
	{
		auto& mpo = morphOffsData[i];
		switch (typeEX)
		{
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_GROUP:
			LoadID_AsInt32(file, mpo.groupOffs.morphID, idByteSize);
			file.Read(mpo.groupOffs.morphRatio);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_VERTEX:
			LoadID_AsInt32(file, mpo.vertexOffs.vertexID, idByteSize);
			file.Read(mpo.vertexOffs.offsPos);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_BONE:
			LoadID_AsInt32(file, mpo.boneOffs.boneID, idByteSize);
			mpo.boneOffs.offsPos = { 8.f };
			file.Read(mpo.boneOffs.offsPos);
			file.Read(mpo.boneOffs.offsRotQ);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_UV:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_1:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_2:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_3:
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_ADDTIONAL_UV_4:
			LoadID_AsInt32(file, mpo.uvOffs.vertexID, idByteSize);
			file.Read(mpo.uvOffs.uv);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_MATERIAL:
			LoadID_AsInt32(file, mpo.materialOffs.materialID, idByteSize);
			file.Read(mpo.materialOffs.offsType);
			file.Read(mpo.materialOffs.diffuse);
			file.Read(mpo.materialOffs.specular);
			file.Read(mpo.materialOffs.specularity);
			file.Read(mpo.materialOffs.ambient);
			file.Read(mpo.materialOffs.edgeColor);
			file.Read(mpo.materialOffs.edgeSize);
			file.Read(mpo.materialOffs.textureScale);
			file.Read(mpo.materialOffs.sphereTextureScale);
			file.Read(mpo.materialOffs.toonTextureScale);
			break;
		case MMDsdk::PmxFile::Morph::MorphTypeEX::MTEX_NONE:
			break;
		default:
			break;
		}
	}
}



void PmxFile::Morph::DebugOut(bool isOutOffsData) const
{
#ifdef _DEBUG
	DebugMessage(name.GetText());
	DebugMessage(nameEng.GetText());
	switch (type)
	{
	case MorphType::MT_BASE:
		DebugMessage(ToString(type) << " = MT_BASE");
		break;
	case MorphType::MT_EYEBROW:
		DebugMessage(ToString(type) << " = MT_EYEBROW");
		break;
	case MorphType::MT_EYE:
		DebugMessage(ToString(type) << " = MT_EYE");
		break;
	case MorphType::MT_LIP:
		DebugMessage(ToString(type) << " = MT_LIP");
		break;
	case MorphType::MT_OTHER:
		DebugMessage(ToString(type) << " = MT_OTHER");
		break;
	case MorphType::MT_NONE:
		DebugMessage(ToString(type) << " = MT_NONE");
		break;
	default:
		break;
	}
	switch (typeEX)
	{
	case MMDsdk::PmxFile::Morph::MTEX_GROUP:
		DebugMessage(ToString(typeEX) << " = MTEX_GROUP");
		break;
	case MMDsdk::PmxFile::Morph::MTEX_VERTEX:
		DebugMessage(ToString(typeEX) << " = MTEX_VERTEX");
		break;
	case MMDsdk::PmxFile::Morph::MTEX_BONE:
		DebugMessage(ToString(typeEX) << " = MTEX_BONE");
		break;
	case MMDsdk::PmxFile::Morph::MTEX_UV:
	case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_1:
	case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_2:
	case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_3:
	case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_4:
		DebugMessage(ToString(typeEX) << " = MTEX_UV");
		break;
	case MMDsdk::PmxFile::Morph::MTEX_MATERIAL:
		DebugMessage(ToString(typeEX) << " = MTEX_MATERIAL");
		break;
	case MMDsdk::PmxFile::Morph::MTEX_NONE:
		DebugMessage(ToString(typeEX) << " = MTEX_NONE");
		break;
	default:
		break;
	}
	DebugOutParamI(offsCount);
	if (isOutOffsData == true)
	{
		for (int i = 0; i < offsCount; ++i)
		{
			auto& mod = GetMorphOffsData(i);
			switch (typeEX)
			{
			case MMDsdk::PmxFile::Morph::MTEX_GROUP:
				mod.groupOffs.DebugOut();
				break;
			case MMDsdk::PmxFile::Morph::MTEX_VERTEX:
				mod.vertexOffs.DebugOut();
				break;
			case MMDsdk::PmxFile::Morph::MTEX_BONE:
				mod.boneOffs.DebugOut();
				break;
			case MMDsdk::PmxFile::Morph::MTEX_UV:
			case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_1:
			case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_2:
			case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_3:
			case MMDsdk::PmxFile::Morph::MTEX_ADDTIONAL_UV_4:
				mod.uvOffs.DebugOut();
				break;
			case MMDsdk::PmxFile::Morph::MTEX_MATERIAL:
				mod.materialOffs.DebugOut();
				break;
			case MMDsdk::PmxFile::Morph::MTEX_NONE:
				break;
			default:
				break;
			}
		}
	}
#endif // _DEBUG
}


PmxFile::Morph::Morph() {}
PmxFile::Morph::~Morph()
{
	SafeDeleteArray(&morphOffsData);
}


const PmxFile::Morph& PmxFile::GetMorph(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mMorph, mMorphCount);
	IS_OUT_OF_RANGE(mMorph, i, mMorphCount);
	return mMorph[i];
}

const int32_t PmxFile::GetLastMorphID() const
{
	return mMorphCount - 1;
}

void PmxFile::DebugOutMorph(const int32_t i, bool isOutVertexData) const
{
	DebugMessage("Morph [" << i << "]");
	GetMorph(i).DebugOut(isOutVertexData);
}

void PmxFile::DebugOutAllMorph(bool isOutVertexData) const
{
#ifdef _DEBUG
	for (int i = 0; i < mMorphCount; ++i)
	{
		DebugOutMorph(i, isOutVertexData);
	}
#endif // _DEBUG
}


const int32_t& PmxFile::GetDisplayFrameCount() const
{
	return mDisplayFrameCount;
}

PmxFile::DisplayFrame::FrameElement::FrameElement() {}
PmxFile::DisplayFrame::FrameElement::~FrameElement() {}

const PmxFile::DisplayFrame::FrameElement& PmxFile::DisplayFrame::GetFrameElement(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(frameElement, frameElementCount);
	IS_OUT_OF_RANGE(frameElement, i, frameElementCount);
	return frameElement[i];
}

void PmxFile::DisplayFrame::LoadFrameElement(void* _file, const size_t boneID_Size, const size_t morphID_Size)
{
	auto& file = GetFile(_file);
	frameElement = new FrameElement[frameElementCount]{};
	for (int i = 0; i < frameElementCount; ++i)
	{
		auto& fe = frameElement[i];
		file.Read(fe.elementType);
		switch (fe.elementType)
		{
		case FrameElement::FrameElementType::FET_BONE:
			LoadID_AsInt32(file, fe.objectID, boneID_Size);
			break;
		case FrameElement::FrameElementType::FET_MORPH:
			LoadID_AsInt32(file, fe.objectID, morphID_Size);
			break;
		default:
			break;
		}

	}
}

void PmxFile::DisplayFrame::DebugOut() const
{
#ifdef _DEBUG
	DebugMessage(name.GetText());
	DebugMessage(nameEng.GetText());
	switch (type)
	{
	case MMDsdk::PmxFile::DisplayFrame::DFT_NORMAL:
		DebugMessage("type = DFT_NORMAL");
		break;
	case MMDsdk::PmxFile::DisplayFrame::DFT_SPECIAL:
		DebugMessage("type = DFT_SPECIAL");
		break;
	case MMDsdk::PmxFile::DisplayFrame::DFT_NONE:
		break;
	default:
		break;
	}
	for (int i = 0; i < frameElementCount; ++i)
	{
		auto& fe = GetFrameElement(i);
		DebugMessage("FrameElement [" << i << "]");
		switch (fe.elementType)
		{
		case FrameElement::FrameElementType::FET_BONE:
			DebugMessage("elementType = FET_BONE");
			DebugOutParamI(fe.objectID);
			break;
		case FrameElement::FrameElementType::FET_MORPH:
			DebugMessage("elementType = FET_MORPH");
			DebugOutParamI(fe.objectID);
			break;
		default:
			break;
		}
	}
#endif // _DEBUG
}

PmxFile::DisplayFrame::DisplayFrame() {}
PmxFile::DisplayFrame::~DisplayFrame()
{
	SafeDeleteArray(&frameElement);
}

const PmxFile::DisplayFrame& PmxFile::GetDisplayFrame(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mDisplayFrame, mDisplayFrameCount);
	IS_OUT_OF_RANGE(mDisplayFrame, i, mDisplayFrameCount);
	return mDisplayFrame[i];
}

const int32_t PmxFile::GetLastDisplayFrameID() const
{
	return mDisplayFrameCount - 1;
}


void PmxFile::DebugOutDisplayFrame(const int32_t i) const
{
	DebugMessage("DisplayFrame [" << i << "]");
	GetDisplayFrame(i).DebugOut();
}

void PmxFile::DebugOutAllDisplayFrame() const
{
#ifdef _DEBUG
	for (int i = 0; i < mDisplayFrameCount; ++i)
	{
		DebugOutDisplayFrame(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetRigitbodyCount() const
{
	return mRigitbodyCount;
}

void PmxFile::Rigitbody::DebugOut() const
{
	DebugOutString(name.GetText());
	DebugOutString(nameEng.GetText());
	DebugOutParamI(relationshipBoneID);
	DebugOutParamI(group);
	DebugOutParamBin(groupTarget, 16);
	DebugOutParamI(shapeType);
	DebugOutParam(shapeW);
	DebugOutParam(shapeH);
	DebugOutParam(shapeD);
	DebugOutFloat3(position);
	DebugOutFloat3(RadianToDegreeFloat3(rotation));
	DebugOutParam(weight);
	DebugOutParam(positionDim);
	DebugOutParam(rotationDim);
	DebugOutParam(recoil);
	DebugOutParam(friction);
	DebugOutParamI(type);
	DebugMessageNewLine();

}

PmxFile::Rigitbody::Rigitbody() {}
PmxFile::Rigitbody::~Rigitbody() {}

const PmxFile::Rigitbody& PmxFile::GetRigitbody(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mRigitbody, mRigitbodyCount);
	IS_OUT_OF_RANGE(mRigitbody, i, mRigitbodyCount);
	return mRigitbody[i];
}

const int32_t PmxFile::GetLastRigitbodyID() const
{
	return mRigitbodyCount - 1;
}

void PmxFile::DebugOutRigitbody(const int32_t i) const
{
	DebugMessage("Rigtbody [" << i << "]");
	GetRigitbody(i).DebugOut();
}

void PmxFile::DebugOutAllRigitbody() const
{
#ifdef _DEBUG
	for (int32_t i = 0; i < mRigitbodyCount; ++i)
	{
		DebugOutRigitbody(i);
	}
#endif // _DEBUG
}

const int32_t& PmxFile::GetJointCount() const
{
	return mJointCount;
}

void PmxFile::Joint::DebugOut() const
{
#ifdef _DEBUG
	DebugOutString(name.GetText());
	DebugOutString(nameEng.GetText());

	switch (type)
	{
	case MMDsdk::PmxFile::Joint::JT_SPRING_6_DOF:
		DebugMessage("type = JointType::JT_SPRING_6_DOF");
		break;
	case MMDsdk::PmxFile::Joint::JT_NONE:
		break;
	default:
		break;
	}

	DebugOutParamI(rigitbodyIndexA);
	DebugOutParamI(rigitbodyIndexB);

	DebugOutFloat3(position);
	DebugOutFloat3(rotation);

	DebugOutFloat3(posLowerLimit);
	DebugOutFloat3(posUpperLimit);

	DebugOutFloat3(RadianToDegreeFloat3(rotLowerLimit));
	DebugOutFloat3(RadianToDegreeFloat3(rotUpperLimit));

	DebugOutFloat3(springPos);
	DebugOutFloat3(springRot);

	DebugMessageNewLine();
#endif // _DEBUG
}

PmxFile::Joint::Joint() {};
PmxFile::Joint::~Joint() {};

const PmxFile::Joint& PmxFile::GetJoint(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mJoint, mJointCount);
	IS_OUT_OF_RANGE(mJoint, i, mJointCount);
	return mJoint[i];
}

const int32_t PmxFile::GetLastJointID() const
{
	return mJointCount - 1;
}


void PmxFile::DebugOutJoint(const int32_t i) const
{
	DebugMessage("Joint [" << i << "]");
	GetJoint(i).DebugOut();
}

void PmxFile::DebugOutAllJoint() const
{
#ifdef _DEBUG
	for (int32_t i = 0; i < mJointCount; ++i)
	{
		DebugOutJoint(i);
	}
#endif // _DEBUG
}


void PmxFile::DebugOutAllData() const
{
	GetHeader().DebugOut();
	DebugOutParamI(mVertexCount);
	DebugOutAllVertex();
	DebugOutParamI(mIndexCount);
	DebugOutAllIndexData();
	DebugOutParamI(mMaterialCount);
	DebugOutAllMaterial();
	DebugOutParamI(mBoneCount);
	DebugOutAllBone();
	DebugOutParamI(mMorphCount);
	DebugOutAllMorph(true);
	DebugOutParamI(mRigitbodyCount);
	DebugOutAllRigitbody();
	DebugOutParamI(mJointCount);
	DebugOutAllJoint();
}

VmdFile::VmdFile(const char* const filepath)
	:
	mHeader(),
	mMortionCount(0),
	mMortion(nullptr)
	//last
{
	System::FileReadBin file(filepath);

	LoadTextBufferFixed(file, mHeader.sigunature);
	LoadTextBufferFixed(file, mHeader.defaultModelName);

	file.Read(mMortionCount);

	mMortion = new Mortion[mMortionCount]{};
	for (int i = 0; i < mMortionCount; ++i)
	{
		auto& m = mMortion[i];
		LoadTextBufferFixed(file, m.name);
		file.Read(m.frameNumber);
		file.Read(m.position);
		file.Read(m.rotation);

		m.LoadBezierParam(&file);
	}
	//last
}

VmdFile::~VmdFile()
{
	SafeDeleteArray(&mMortion);
}

VmdFile::Header::Header() {}
VmdFile::Header::~Header() {}

const VmdFile::Header& VmdFile::GetHeader() const
{
	return mHeader;
}

const int32_t& VmdFile::GetMortionCount() const
{
	return mMortionCount;
}

const char& VmdFile::Mortion::GetBezierParam(const int32_t i) const
{
	IS_OUT_OF_RANGE(bezierParam, i, 64);
	return bezierParam[i];
}

void VmdFile::Mortion::LoadBezierParam(void* _file)
{
	auto& file = GetFile(_file);
	file.ReadArray(bezierParam, 64);
}

VmdFile::Mortion::Mortion() {}
VmdFile::Mortion::~Mortion() {}

void VmdFile::Mortion::DebugOut() const
{
#ifdef _DEBUG
	DebugOutString(name.GetText());
	DebugOutParamI(frameNumber);
	DebugOutFloat3(position);
	DebugOutFloat4(rotation);
#endif // _DEBUG
}

const VmdFile::Mortion& VmdFile::GetMortion(const int32_t i) const
{
	ID_IS_NO_REF(i);
	ARRAY_HAS_NO_DATA(mMortion, mMortionCount);
	IS_OUT_OF_RANGE(mMortion, i, mMortionCount);

	return mMortion[i];
}

const int32_t VmdFile::GetLastMortionID() const
{
	return mMortionCount - 1;
}

void VmdFile::DebugOutMortion(const int32_t i) const
{
	DebugMessage("Mortion [" << i << "]");
	GetMortion(i).DebugOut();
}

void VmdFile::DebugOutAllMortion() const
{
#ifdef _DEBUG
	for (int32_t i = 0; i < mMortionCount; ++i)
	{
		DebugOutMortion(i);
	}
#endif // _DEBUG
}

//last


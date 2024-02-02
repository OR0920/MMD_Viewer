#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"OutlineVS.h"
#include"OutlinePS.h"

#include<memory>

#define ReturnIfFailed(func) if (func == GUI::Result::FAIL) return GUI::Result::FAIL;

void Model::ModelVertex::Load(const MMDsdk::PmdFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
	if (data.edgeFlag == MMDsdk::PmdFile::Vertex::EdgeEnable::VEE_ENABLE)
	{
		edgeRate = 1.f;
	}
	else
	{
		edgeRate = 0.f;
	}
}

void Model::ModelVertex::Load(const MMDsdk::PmxFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
	edgeRate = data.edgeRate;
}

void Model::Material::Load(const MMDsdk::PmdFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
	edgeColor = { 0.f, 0.f, 0.f, 1.f };
	edgeSize = 1.f;
}

void Model::Material::Load(const MMDsdk::PmxFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
	edgeColor = System::strong_cast<MathUtil::float4>(data.edgeColor);
	edgeSize = data.edgeSize;
}

const wchar_t* Model::mToonPath[] =
{
	L"DefaultTexture/toon01.bmp",
	L"DefaultTexture/toon02.bmp",
	L"DefaultTexture/toon03.bmp",
	L"DefaultTexture/toon04.bmp",
	L"DefaultTexture/toon05.bmp",
	L"DefaultTexture/toon06.bmp",
	L"DefaultTexture/toon07.bmp",
	L"DefaultTexture/toon08.bmp",
	L"DefaultTexture/toon09.bmp",
	L"DefaultTexture/toon10.bmp",
};

Model::Model(GUI::Graphics::Device& device)
	:
	mDevice(device),
	mVertexBuffer(),
	mIndexBuffer(),
	mDescriptorCount(0),
	mHeap(),
	mTransformBuffer(),
	mPS_DataBuffer(),
	mMaterialBuffer(),
	mMaterialInfo(),
	mMaterialCount(0),
	mInputLayout({}),
	mPipeline({}),
	mRootSignature({}),
	mOutlinePipeline({}),
	mUniqueTexture(),
	mDefaultTextureWhite({}),
	mDefaultTextureBlack({}),
	mDefaultTextureToon()
{
	// 頂点レイアウトの設定
	mInputLayout.SetElementCount(4);
	mInputLayout.SetDefaultPositionDesc();
	mInputLayout.SetDefaultNormalDesc();
	mInputLayout.SetDefaultUV_Desc();
	mInputLayout.SetFloatParam("EDGE_RATE");

	//　ルートシグネチャの作成
	mRootSignature.SetParameterCount(7);

	// param[0] : b0 : 行列類
	// param[1] : b1 : シーンのデータ
	// param[2] : b2 : マテリアルのデータ
	mRootSignature.SetParamForCBV(0, 0);
	mRootSignature.SetParamForCBV(1, 1);
	mRootSignature.SetParamForCBV(2, 2);

	// param[3] : t0 : 通常テクスチャ  
	// param[4] : t1 : 乗算スフィアテクスチャ  
	// param[5] : t2 : 加算スフィアテクスチャ  
	// param[6] : t3 : Toonテクスチャ  
	GUI::Graphics::DescriptorRange range;
	range.SetRangeCount(1);
	range.SetRangeForSRV(0, 0, 1);
	mRootSignature.SetParamForDescriptorTable(3, range);

	GUI::Graphics::DescriptorRange sphRange;
	sphRange.SetRangeCount(1);
	sphRange.SetRangeForSRV(0, 1, 1);
	mRootSignature.SetParamForDescriptorTable(4, sphRange);

	GUI::Graphics::DescriptorRange spaRange;
	spaRange.SetRangeCount(1);
	spaRange.SetRangeForSRV(0, 2, 1);
	mRootSignature.SetParamForDescriptorTable(5, spaRange);

	GUI::Graphics::DescriptorRange toonRange;
	toonRange.SetRangeCount(1);
	toonRange.SetRangeForSRV(0, 3, 1);
	mRootSignature.SetParamForDescriptorTable(6, toonRange);

	// s0 : 通常、乗算、加算テクスチャ用のサンプラー
	// s1 : Toon用サンプラー　座標がループしない
	mRootSignature.SetStaticSamplerCount(2);
	mRootSignature.SetSamplerDefault(0, 0);
	mRootSignature.SetSamplerUV_Clamp(1, 1);

	mDevice.CreateRootSignature(mRootSignature);

	// パイプライン設定
	mPipeline.SetRootSignature(mRootSignature);
	mPipeline.SetAlphaEnable(); // 透過ON
	mPipeline.SetDepthEnable(); // 深度有効
	mPipeline.SetInputLayout(mInputLayout);
	mPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	mDevice.CreateGraphicsPipeline(mPipeline);

	// モデルによらず固定されたディスクリプターの数を設定
	mDescriptorCount = sDefaultTextureCount + sDefaultToonTextureCount + sSceneDataCount;


	// 輪郭線用の設定

	mOutlinePipeline.SetInputLayout(mInputLayout);
	mOutlinePipeline.SetFrontCullEnable();
	mOutlinePipeline.SetRootSignature(mRootSignature);
	mOutlinePipeline.SetDepthEnable();
	mOutlinePipeline.SetAlphaEnable();
	mOutlinePipeline.SetVertexShader(SetShader(gOutlineVS));
	mOutlinePipeline.SetPixelShader(SetShader(gOutlinePS));
	mDevice.CreateGraphicsPipeline(mOutlinePipeline);
}

Model::~Model()
{
}

GUI::Result Model::Load(const char* const filepath)
{
	// すべての形式で読み込んでみる
	if (LoadPMX(filepath) == GUI::Result::SUCCESS)
	{
		DebugMessage("Load pmx file !");
	}
	else if (LoadPMD(filepath) == GUI::Result::SUCCESS)
	{
		DebugMessage("Load pmd file !");
	}
	else
	{
		// 全フォーマットで読み込み失敗したらリターン
		return GUI::Result::FAIL;
	}

	DebugMessage("Model Load Succeeded !");

	ReturnIfFailed(this->CreateDefaultBufferData());

	DebugMessage("Default Buffer Created !");

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateDefaultBufferData()
{
	// シーン情報の初期化
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform))
	);
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData))
	);

	// デフォルトのテクスチャを作る
	ReturnIfFailed(mDefaultTextureWhite.LoadFromFile(L"DefaultTexture/White.png"));
	ReturnIfFailed(mDevice.CreateTexture2D(mDefaultTextureWhite, mHeap));

	ReturnIfFailed(mDefaultTextureBlack.LoadFromFile(L"DefaultTexture/Black.png"));
	ReturnIfFailed(mDevice.CreateTexture2D(mDefaultTextureBlack, mHeap));

	for (int i = 0; i < 10; ++i)
	{
		auto& t = mDefaultTextureToon[i];

		ReturnIfFailed(t.LoadFromFile(mToonPath[i]));

		ReturnIfFailed(mDevice.CreateTexture2D(t, mHeap));
	}
}

void Model::Update()
{
	// 回転角計算
	static int frameCount = 0;
	static float rotUnit = 1.f;
	frameCount++;
	frameCount %= static_cast<int>(3600.f / (rotUnit * 10.f));
	float rotation = rotUnit * frameCount;

	// モデルを回転させる行列を書き込む
	ModelTransform* transform = nullptr;
	mTransformBuffer.Map(reinterpret_cast<void**>(&transform));
	transform->world = MathUtil::Matrix::GenerateMatrixRotationY(MathUtil::DegreeToRadian(rotation));
	mTransformBuffer.Unmap();
}

void Model::Draw(GUI::Graphics::GraphicsCommand& command)
{
	// モデル描画用のパイプラインとルートシグネチャをセット
	command.SetGraphicsPipeline(mPipeline);
	command.SetGraphicsRootSignature(mRootSignature);

	// ビューを用いてシーンのデータをバインド
	command.SetDescriptorHeap(mHeap);
	command.SetConstantBuffer(mTransformBuffer, 0);
	command.SetConstantBuffer(mPS_DataBuffer, 1);

	// 頂点バッファ、インデックスバッファをセット
	command.SetVertexBuffer(mVertexBuffer, mIndexBuffer);

	
	// マテリアル毎にメッシュを描画
	int indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		auto& info = mMaterialInfo[i];
		auto indexCount = info.materialIndexCount;

		// マテリアルセット
		command.SetConstantBuffer(mMaterialBuffer, 2, i);

		// 各テクスチャをセット
		// 固有テクスチャを持たない場合デフォルトのテクスチャを渡す
		if (info.texID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.texID], 3);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 3);
		}

		if (info.sphID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.sphID], 4);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 4);
		}

		if (info.spaID != -1)
		{
			command.SetDescriptorTable(mUniqueTexture[info.spaID], 5);
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureBlack, 5);
		}

		// トゥーンは
		// ・固有テクスチャを持つもの(PMXのみの機能)
		// ・共有テクスチャを持つもの
		// ・トゥーンテクスチャを持たないもの
		// がある
		if (info.toonID != -1)
		{
			if (info.isShared == true)
			{
				command.SetDescriptorTable(mDefaultTextureToon[info.toonID], 6);
			}
			else
			{
				command.SetDescriptorTable(mUniqueTexture[info.toonID], 6);
			}
		}
		else
		{
			command.SetDescriptorTable(mDefaultTextureWhite, 6);
		}

		// 前のマテリアルの続きから、メッシュを描画
		command.DrawTriangleList(indexCount, indexOffs);
		indexOffs += indexCount;
	}


	// 輪郭線描画
	command.SetGraphicsPipeline(mOutlinePipeline);

	indexOffs = 0;
	for (int i = 0; i < mMaterialCount; ++i)
	{
		if (mMaterialInfo[i].isEdgeEnable == false)
		{
			indexOffs += mMaterialInfo[i].materialIndexCount;
			continue;
		}

		command.SetConstantBuffer(mMaterialBuffer, 2, i);

		command.DrawTriangleList(mMaterialInfo[i].materialIndexCount, indexOffs);
		indexOffs += mMaterialInfo[i].materialIndexCount;
	}
}

// PMDから読みこむ
void Model::MaterialInfo::Load(const MMDsdk::PmdFile::Material& data)
{
	materialIndexCount = data.vertexCount;
	toonID = static_cast<int>(data.toonIndex);
	isShared = true;
	if (data.edgeFlag == MMDsdk::PmdFile::Material::EdgeEnable::MEE_ENABLE)
	{
		isEdgeEnable = true;
	}
	else
	{
		isEdgeEnable = false;
	}
}

// PMXから読みこむ
void Model::MaterialInfo::Load(const MMDsdk::PmxFile::Material& data)
{
	texID = data.textureID;
	if (data.sphereMode == MMDsdk::PmxFile::Material::SphereMode::SM_SPH)
	{
		sphID = data.sphereTextureID;
	}
	else if (data.sphereMode == MMDsdk::PmxFile::Material::SphereMode::SM_SPA)
	{
		spaID = data.sphereTextureID;
	}

	toonID = data.toonTextureID;
	if (data.toonMode == MMDsdk::PmxFile::Material::ToonMode::TM_SHARED)
	{
		isShared = true;
	}
	else
	{
		isShared = false;
	}

	materialIndexCount = data.vertexCount;

	isEdgeEnable = data.GetDrawConfig(MMDsdk::PmxFile::Material::DC_DRAW_EDGE);
}

GUI::Result Model::LoadPMD(const char* const filepath)
{
	// PMDとして開けなければ失敗
	MMDsdk::PmdFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}

	// 頂点データを読み込み、バッファを作る
	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		System::varray<ModelVertex> mesh(vCount);
		//ModelVertex* mesh = new ModelVertex[vCount];
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		ReturnIfFailed(CreateVertexBuffer(mesh.GetStart(), vCount));
	}

	// インデックスデータを読み込み、バッファを作る
	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		System::varray<int> index(iCount);
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		ReturnIfFailed(CreateIndexBuffer(index.GetStart(), iCount));
	}

	// マテリアル数を取得
	mMaterialCount = file.GetMaterialCount();

	// マテリアルが0なら、固有テクスチャも0なので、ヒープを作りリターン
	if (mMaterialCount == 0)
	{
		ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));
		return GUI::Result::SUCCESS;
	}

	// PMDの場合、テクスチャファイル名が
	// tex1.bmp*tex2.sph　のように結合された状態で
	// 各マテリアルに記録されている。
	// 
	// 種類ごとに分割し記録しておく
	// pmdのパスの長さは20固定
	auto& pmdTexLength = MMDsdk::PmdFile::TexPathLength;

	struct TexPaths
	{
		char tex[pmdTexLength] = { '\0' };
		char sph[pmdTexLength] = { '\0' };
		char spa[pmdTexLength] = { '\0' };
	};


	// テクスチャ名を分割する際に使う構造体

	// テクスチャ名の長さと開始位置
	struct SubString
	{
		const char* start = nullptr;
		int length = 0;
	};
	System::varray<TexPaths> texPathPerMaterial(mMaterialCount);

	// テクスチャ名を分割して記録する構造体
	// それぞれのパス名の開始位置を記録する
	struct TexPathSignature
	{
		SubString path[2];
		int pathCount = 0;
		void Load(const char* const str, const int length)
		{
			// テクスチャ一つと仮定して初期化
			path[0].start = &str[0];
			path[0].length = length;

			// 1つ目カウント
			pathCount++;

			// 分割文字'*'を探す
			for (int i = 0; i < length; ++i)
			{
				if (str[i] == '*')
				{
					// 各パスの長さと開始位置を記録しておく
					path[1].start = &str[i + 1];

					path[0].length = i;
					path[1].length = length - i;

					// 2つ目カウント
					pathCount++;
					break;
				}
				if (str[i] == '\0')
				{
					// 末端にたどり着いたら一つだけ
					return;
				}
			}
		}
	};

	System::varray<TexPathSignature> pathSignature(mMaterialCount);

	// テクスチャ数
	int tCount = 0;

	if (mMaterialCount != 0)
	{
		// シェーダーレジスタに置くマテリアル情報
		System::varray<Material> material(mMaterialCount);
		// シェーダーレジスタに置かないマテリアル情報
		mMaterialInfo.Init(mMaterialCount);

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);
			material[i].Load(m);
			mMaterialInfo[i].Load(m);

			// テクスチャを取得
			auto tp = m.texturePath.GetText();

			// テクスチャを持たないマテリアルなので次
			if (tp[0] == '\0') continue;

			// パスを解析、分割
			auto tpLength = m.texturePath.GetLength();
			pathSignature[i].Load(tp, tpLength);

			// 各テクスチャを種類ごとにコピー
			for (int j = 0; j < pathSignature[i].pathCount; ++j)
			{
				// Tex1.bmp*Tex2.sph　というように
				// 一つ目にはNULL文字が存在しないため、一回バッファにコピーする
				char path[pmdTexLength] = { '\0' };
				auto subStr = pathSignature[i].path[j];

				memcpy_s(&path[0], pmdTexLength, subStr.start, subStr.length);

				// 拡張子で振り分け　
				// PMDの仕様では、拡張子でテクスチャの機能が変わる
				auto* ext = System::GetExt(path);

				auto& p = texPathPerMaterial[i];
				auto& mifo = mMaterialInfo[i];
				if (System::StringEqual(ext, ".sph") == true)
				{
					memcpy_s(p.sph, pmdTexLength, path, pmdTexLength);
					// テクスチャを読み込んだ順番とIDを対応させる
					mifo.sphID = tCount;
					tCount++;// テクスチャ数を数える
				}
				else if (System::StringEqual(ext, ".spa") == true)
				{
					memcpy_s(p.spa, pmdTexLength, path, pmdTexLength);
					mifo.spaID = tCount;
					tCount++;
				}
				else
				{
					//　上記以外の拡張子はすべて通常テクスチャとする
					memcpy_s(p.tex, pmdTexLength, path, pmdTexLength);
					mifo.texID = tCount;
					tCount++;
				}
			}
		}

		// ディスクリプタの数を更新し、ヒープ作成
		mDescriptorCount += mMaterialCount + tCount;
		ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));

		// マテリアルの定数バッファを作成
		ReturnIfFailed(CreateMaterialBuffer(material.GetStart(), mMaterialCount));
	}

	// テクスチャを読み込む
	// とりあえずシンプルにしておきたいので、同じテクスチャだろうがそのまま読み込む
	if (tCount != 0)
	{
		mUniqueTexture.Init(tCount);

		int texID = 0;
		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto p = texPathPerMaterial[i];

			if (p.tex[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.tex, texID));
				texID++;
			}

			if (p.sph[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.sph, texID));
				texID++;
			}

			if (p.spa[0] != '\0')
			{
				ReturnIfFailed(CreateTexture(file.GetDirectoryPath(), p.spa, texID));
				texID++;
			}
		}
	}

	return GUI::Result::SUCCESS;
}


GUI::Result Model::LoadPMX(const char* const filepath)
{
	MMDsdk::PmxFile file(filepath);
	if (file.IsSuccessLoad() == false)
	{
		return GUI::Result::FAIL;
	}

	// 頂点データを読み込みバッファを作成
	auto vCount = file.GetVertexCount();
	if (vCount != 0)
	{
		System::varray<ModelVertex> mesh(vCount);
		for (int i = 0; i < vCount; ++i)
		{
			mesh[i].Load(file.GetVertex(i));
		}

		ReturnIfFailed(CreateVertexBuffer(mesh.GetStart(), vCount));
	}

	// インデックスデータを読み込みバッファを作成
	auto iCount = file.GetIndexCount();
	if (iCount != 0)
	{
		System::varray<int> index(iCount);
		for (int i = 0; i < iCount; ++i)
		{
			index[i] = file.GetIndex(i);
		}

		ReturnIfFailed(CreateIndexBuffer(index.GetStart(), iCount));
	}

	// マテリアルとテクスチャを読み込む

	// PMDと異なり、テクスチャとマテリアルが分離しているため
	// この段階でディスクリプタ数が確定する
	mMaterialCount = file.GetMaterialCount();
	auto tCount = file.GetTextureCount();

	mDescriptorCount += mMaterialCount + tCount;
	ReturnIfFailed(mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount));

	// マテリアルを読み込みバッファを作成
	if (mMaterialCount != 0)
	{
		System::varray<Material> material(mMaterialCount);
		mMaterialInfo.Init(mMaterialCount);

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);

			material[i].Load(m);
			mMaterialInfo[i].Load(m);
		}

		ReturnIfFailed(CreateMaterialBuffer(material.GetStart(), mMaterialCount));
	}

	// テクスチャを読み込みバッファを作成
	if (tCount != 0)
	{
		mUniqueTexture.Init(tCount);

		for (int i = 0; i < tCount; ++i)
		{
			ReturnIfFailed
			(
				CreateTexture
				(
					file.GetDirectoryPath(),
					file.GetTexturePath(i).GetText(),
					i
				)
			);
		}
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount)
{
	// バッファを作ってコピー
	ReturnIfFailed(mDevice.CreateVertexBuffer(mVertexBuffer, sizeof(ModelVertex), vertexCount));
	ReturnIfFailed(mVertexBuffer.Copy(vertex));

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateIndexBuffer(const int index[], const int indexCount)
{
	// バッファを作ってコピー
	ReturnIfFailed(mDevice.CreateIndexBuffer(mIndexBuffer, sizeof(int), indexCount));
	ReturnIfFailed(mIndexBuffer.Copy(index));

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateMaterialBuffer(const Material material[], const int materialCount)
{
	// バッファを作ってマップしてコピー
	ReturnIfFailed
	(
		mDevice.CreateConstantBuffer
		(
			mMaterialBuffer,
			mHeap,
			sizeof(Material),
			materialCount
		)
	);

	unsigned char* mappedMaterial = nullptr;
	ReturnIfFailed(mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)));

	for (int i = 0; i < materialCount; ++i)
	{
		*reinterpret_cast<Material*>(mappedMaterial) = material[i];
		// 256バイトにアラインメントされているため、sizeof(Material)ではない
		mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();
	}

	mMaterialBuffer.Unmap();

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateTexture(const char* const dirPath, const char* const filename, const int texID)
{
	// ディレクトリがnullptrはありえない
	if (dirPath == nullptr)	return GUI::Result::FAIL;

	// テクスチャがない場合は、失敗ではないので成功として返す
	if (filename == nullptr) return GUI::Result::SUCCESS;
	if (texID == -1) return GUI::Result::SUCCESS;

	// パスを結合
	char* filepath = nullptr;
	System::newArray_CopyAssetPath(&filepath, dirPath, filename);

	// テクスチャを読み込む
	auto result = mUniqueTexture[texID].LoadFromFile(filepath);

	System::SafeDeleteArray(&filepath);

	ReturnIfFailed(result);

	// テクスチャを作成
	ReturnIfFailed(mDevice.CreateTexture2D(mUniqueTexture[texID], mHeap))

		return GUI::Result::SUCCESS;
}


GUI::Result Model::SetDefaultSceneData(const float aspectRatio)
{
	// 行列データ、シーンデータをマップしコピー
	// データは一つだけなので、そのままクラスのポインタを使用する。
	ModelTransform* mappedTransform = nullptr;
	ReturnIfFailed(mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)));
	// 視点
	auto eye = MathUtil::Vector(0.f, 10.f, -30.f);
	mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
	mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
	(
		eye,
		MathUtil::Vector::basicZ,
		MathUtil::Vector::basicY
	);
	mappedTransform->proj = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
	(
		MathUtil::PI_DIV4,
		aspectRatio,
		0.1f,
		1000.f
	);
	mappedTransform->eye = eye.GetFloat3();
	mTransformBuffer.Unmap();

	PixelShaderData* mappedPS_Data = nullptr;
	ReturnIfFailed(mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)));

	// ライトの方向
	mappedPS_Data->lightDir = MathUtil::Vector(-1.f, -1.f, 1.f).GetFloat3();
	mPS_DataBuffer.Unmap();


	return GUI::Result::SUCCESS;
}

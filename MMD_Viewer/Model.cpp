#include"Model.h"

#include"MMDsdk.h"
#include"System.h"

#include"MMD_VertexShasder.h"
#include"MMD_PixelShader.h"

#include"OutlineVS.h"
#include"OutlinePS.h"

void Model::ModelVertex::Load(const MMDsdk::PmdFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
}

void Model::ModelVertex::Load(const MMDsdk::PmxFile::Vertex& data)
{
	position = System::strong_cast<MathUtil::float3>(data.position);
	normal = System::strong_cast<MathUtil::float3>(data.normal);
	uv = System::strong_cast<MathUtil::float2>(data.uv);
}

void Model::Material::Load(const MMDsdk::PmdFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
}

void Model::Material::Load(const MMDsdk::PmxFile::Material& data)
{
	diffuse = System::strong_cast<MathUtil::float4>(data.diffuse);
	specular = System::strong_cast<MathUtil::float3>(data.specular);
	specularity = data.specularity;
	ambient = System::strong_cast<MathUtil::float3>(data.ambient);
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
	mVB(),
	mIB(),
	mDescriptorCount(0),
	mHeap(),
	mTransformBuffer(),
	mPS_DataBuffer(),
	mMaterialBuffer(),
	mMaterialInfo(nullptr),
	mMaterialCount(0),
	inputLayout({}),
	mPipeline({}),
	mRootSignature({}),
	isSuccessLoad(GUI::Result::FAIL),
	mUniqueTexture(nullptr),
	mDefaultTextureWhite({}),
	mDefaultTextureBlack({}),
	mDefaultTextureToon()
{
	// 頂点レイアウトの設定
	inputLayout.SetElementCount(3);
	inputLayout.SetDefaultPositionDesc();
	inputLayout.SetDefaultNormalDesc();
	inputLayout.SetDefaultUV_Desc();

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
	mPipeline.SetInputLayout(inputLayout);
	mPipeline.SetVertexShader(gMMD_VS, _countof(gMMD_VS));
	mPipeline.SetPixelShader(gMMD_PS, _countof(gMMD_PS));
	mDevice.CreateGraphicsPipeline(mPipeline);

	// モデルによらず固定されたディスクリプターの数を設定
	mDescriptorCount = sDefaultTextureCount + sDefaultToonTextureCount + sSceneDataCount;


	// アウトライン用の設定
	mOutlineSignature.SetParameterCount(1);
	mOutlineSignature.SetParamForCBV(0, 0);
	mDevice.CreateRootSignature(mOutlineSignature);
	
	mOutlinePipeline.SetInputLayout(inputLayout);
	mOutlinePipeline.SetFrontCullEnable();
	mOutlinePipeline.SetRootSignature(mOutlineSignature);
	mOutlinePipeline.SetDepthEnable();
	mOutlinePipeline.SetAlphaEnable();
	mOutlinePipeline.SetVertexShader(SetShader(gOutlineVS));
	mOutlinePipeline.SetPixelShader(SetShader(gOutlinePS));
	mDevice.CreateGraphicsPipeline(mOutlinePipeline);
}

Model::~Model()
{
	System::SafeDeleteArray(&mMaterialInfo);
	System::SafeDeleteArray(&mUniqueTexture);
}

GUI::Result Model::Load(const char* const filepath)
{
	isSuccessLoad = GUI::Result::FAIL;

	// すべての形式で読み込んでみる
	if (LoadPMD(filepath) == GUI::Result::SUCCESS) { isSuccessLoad = GUI::Result::SUCCESS; }
	else if (LoadPMX(filepath) == GUI::Result::SUCCESS) { isSuccessLoad = GUI::Result::SUCCESS; }

	// 全フォーマットで読み込み失敗したらリターン
	if (isSuccessLoad == GUI::Result::FAIL) { return GUI::Result::FAIL; }

	DebugMessage("Model Load Succeeded !");

	// シーン情報の初期化
	if (mDevice.CreateConstantBuffer(mTransformBuffer, mHeap, sizeof(ModelTransform)) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mDevice.CreateConstantBuffer(mPS_DataBuffer, mHeap, sizeof(PixelShaderData)) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	// デフォルトのテクスチャを作る
	if (mDefaultTextureWhite.LoadFromFile(L"DefaultTexture/White.png") == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}
	if (mDevice.CreateTexture2D(mDefaultTextureWhite, mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}
	if (mDefaultTextureBlack.LoadFromFile(L"DefaultTexture/Black.png") == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}
	if (mDevice.CreateTexture2D(mDefaultTextureBlack, mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}
	for (int i = 0; i < 10; ++i)
	{
		auto& t = mDefaultTextureToon[i];

		if (t.LoadFromFile(mToonPath[i]) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		if (mDevice.CreateTexture2D(t, mHeap) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::IsSuccessLoad() const
{
	return isSuccessLoad;
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
	command.SetVertexBuffer(mVB, mIB);

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

	command.SetGraphicsPipeline(mOutlinePipeline);
	command.SetGraphicsRootSignature(mOutlineSignature);

	command.DrawTriangleList(mIB.GetIndexCount(), 0);
}

// PMDから読みこむ
void Model::MaterialInfo::Load(const MMDsdk::PmdFile::Material& data)
{
	materialIndexCount = data.vertexCount;
	toonID = static_cast<int>(data.toonIndex);
	isShared = true;
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

		if (CreateVertexBuffer(mesh.GetStart(), vCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
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

		if (CreateIndexBuffer(index.GetStart(), iCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	// マテリアル数を取得
	mMaterialCount = file.GetMaterialCount();

	// マテリアルが0なら、固有テクスチャも0なので、ヒープを作りリターン
	if (mMaterialCount == 0)
	{
		if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	// PMDの場合、テクスチャファイル名が
	// tex1.bmp*tex2.sph　のように結合された状態で
	// 各マテリアルに記録されている。
	// 
	// 種類ごとに分割し記録しておく
	// pmdのパスの長さは20固定
	struct TexPaths
	{
		char tex[20] = { '\0' };
		char sph[20] = { '\0' };
		char spa[20] = { '\0' };
	};


	// テクスチャを分割する際に使う構造体群
	struct SubString
	{
		// テクスチャ名の長さと開始位置
		const char* start = nullptr;
		int length = 0;
	};
	System::varray<TexPaths> texPathPerMaterial(mMaterialCount);

	// テクスチャ名を分割して記録する構造体
	struct TexPathSignature
	{
		SubString path[2];
		int pathCount = 0;
		void Load(const char* const str, const int length)
		{
			// テクスチャ一つと仮定する
			path[0].start = &str[0];
			path[0].length = length;

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

					// テクスチャは2つ
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
		// シェーダーに置くマテリアル情報
		System::varray<Material> material(mMaterialCount);
		// シェーダーに置かないマテリアル情報
		mMaterialInfo = new MaterialInfo[mMaterialCount];

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);
			material[i].Load(m);
			mMaterialInfo[i].Load(m);

			// テクスチャを取得
			auto tp = m.texturePath.GetText();

			// テクスチャを持たないマテリアル
			if (tp[0] == '\0') continue;

			auto tpLength = m.texturePath.GetLength();
			pathSignature[i].Load(tp, tpLength);

			// 各テクスチャを種類ごとにコピー
			for (int j = 0; j < pathSignature[i].pathCount; ++j)
			{
				// Tex1.bmp*Tex2.sph　というように
				// 一つ目にはNULL文字が存在しないため、一回バッファにコピーする
				char path[20] = { '\0' };
				auto subStr = pathSignature[i].path[j];

				memcpy(&path[0], subStr.start, subStr.length);

				// 拡張子で振り分け　
				// PMDの仕様では、拡張子でテクスチャの機能が変わる
				auto* ext = System::GetExt(path);

				auto& p = texPathPerMaterial[i];
				auto& mifo = mMaterialInfo[i];
				if (System::StringEqual(ext, ".sph") == true)
				{
					memcpy(p.sph, path, 20);
					// テクスチャを読み込んだ順番とIDを対応させる
					mifo.sphID = tCount;
					tCount++;// テクスチャ数を数える
				}
				else if (System::StringEqual(ext, ".spa") == true)
				{
					memcpy(p.spa, path, 20);
					mifo.spaID = tCount;
					tCount++;
				}
				else
				{
					//　上記以外の拡張子はすべて通常テクスチャとする
					memcpy(p.tex, path, 20);
					mifo.texID = tCount;
					tCount++;
				}
			}
		}

		// ディスクリプタの数を更新し、ヒープ作成
		mDescriptorCount += mMaterialCount + tCount;
		if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

		// マテリアルの定数バッファを作成
		if (CreateMaterialBuffer(material.GetStart(), mMaterialCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}

	}

	// テクスチャを読み込む
	// とりあえずシンプルにしておきたいので、同じテクスチャだろうがそのまま読み込む
	if (tCount != 0)
	{
		mUniqueTexture = new GUI::Graphics::Texture2D[tCount];

		int texID = 0;
		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto p = texPathPerMaterial[i];

			if (p.tex[0] != '\0')
			{
				if (CreateTexture(file.GetDirectoryPath(), p.tex, texID) == GUI::Result::FAIL)
				{
					return GUI::Result::FAIL;
				}
				texID++;
			}

			if (p.sph[0] != '\0')
			{
				if (CreateTexture(file.GetDirectoryPath(), p.sph, texID) == GUI::Result::FAIL)
				{
					return GUI::Result::FAIL;
				}
				texID++;
			}

			if (p.spa[0] != '\0')
			{
				if (CreateTexture(file.GetDirectoryPath(), p.spa, texID) == GUI::Result::FAIL)
				{
					return GUI::Result::FAIL;
				}
				texID++;
			}
		}
	}

	////　デフォルトでカリングをオフにしておく
	//mPipeline.SetCullDisable();
	//if (mDevice.CreateGraphicsPipeline(mPipeline) == GUI::Result::FAIL)
	//{
	//	return GUI::Result::FAIL;
	//}

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

		if (CreateVertexBuffer(mesh.GetStart(), vCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
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

		if (CreateIndexBuffer(index.GetStart(), iCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	// マテリアルとテクスチャを読み込む

	// PMDと異なり、テクスチャとマテリアルが分離しているため
	// この段階でディスクリプタ数が確定する
	mMaterialCount = file.GetMaterialCount();
	auto tCount = file.GetTextureCount();

	mDescriptorCount += mMaterialCount + tCount;

	if (mDevice.CreateDescriptorHeap(mHeap, mDescriptorCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	// マテリアルを読み込みバッファを作成
	if (mMaterialCount != 0)
	{
		System::varray<Material> material(mMaterialCount);
		mMaterialInfo = new MaterialInfo[mMaterialCount];

		for (int i = 0; i < mMaterialCount; ++i)
		{
			auto& m = file.GetMaterial(i);

			material[i].Load(m);
			mMaterialInfo[i].Load(m);
		}

		if (CreateMaterialBuffer(material.GetStart(), mMaterialCount) == GUI::Result::FAIL)
		{
			return GUI::Result::FAIL;
		}
	}

	// テクスチャを読み込みバッファを作成
	if (tCount != 0)
	{
		mUniqueTexture = new GUI::Graphics::Texture2D[tCount];

		for (int i = 0; i < tCount; ++i)
		{
			if (CreateTexture(file.GetDirectoryPath(), file.GetTexturePath(i).GetText(), i) == GUI::Result::FAIL)
			{
				return GUI::Result::FAIL;
			}
		}
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateVertexBuffer(const ModelVertex vertex[], const int vertexCount)
{
	// バッファを作ってコピー
	if (mDevice.CreateVertexBuffer(mVB, sizeof(ModelVertex), vertexCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	};

	if (mVB.Copy(vertex) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateIndexBuffer(const int index[], const int indexCount)
{
	// バッファを作ってコピー
	if (mDevice.CreateIndexBuffer(mIB, sizeof(int), indexCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	if (mIB.Copy(index) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;
}

GUI::Result Model::CreateMaterialBuffer(const Material material[], const int materialCount)
{
	// バッファを作ってマップしてコピー
	if (mDevice.CreateConstantBuffer(mMaterialBuffer, mHeap, sizeof(Material), materialCount) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	unsigned char* mappedMaterial = nullptr;
	if (mMaterialBuffer.Map(reinterpret_cast<void**>(&mappedMaterial)) == GUI::Result::SUCCESS)
	{
		for (int i = 0; i < materialCount; ++i)
		{
			*reinterpret_cast<Material*>(mappedMaterial) = material[i];
			// 256バイトにアラインメントされているため、sizeof(Material)ではない
			mappedMaterial += mMaterialBuffer.GetBufferIncrementSize();
		}

		return GUI::Result::SUCCESS;
	}

	return GUI::Result::FAIL;
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

	if (result == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	// テクスチャを作成
	if (mDevice.CreateTexture2D(mUniqueTexture[texID], mHeap) == GUI::Result::FAIL)
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;
}


GUI::Result Model::SetDefaultSceneData(const float aspectRatio)
{
	// 行列データ、シーンデータをマップしコピー
	// データは一つだけなので、そのままクラスのポインタを使用する。
	ModelTransform* mappedTransform = nullptr;
	if (mTransformBuffer.Map(reinterpret_cast<void**>(&mappedTransform)) == GUI::Result::SUCCESS)
	{
		// 視点
		auto eye = MathUtil::Vector(0.f, 10.f, -30.f);
		mappedTransform->world = MathUtil::Matrix::GenerateMatrixIdentity();
		mappedTransform->view = MathUtil::Matrix::GenerateMatrixLookToLH
		(
			eye,
			MathUtil::Vector::basicZ,
			MathUtil::Vector::basicY
		);
		DebugOutParam(aspectRatio);
		mappedTransform->proj = MathUtil::Matrix::GenerateMatrixPerspectiveFovLH
		(
			MathUtil::PI_DIV4,
			aspectRatio,
			0.1f,
			1000.f
		);
		mappedTransform->eye = eye.GetFloat3();
		mTransformBuffer.Unmap();
	}
	else
	{
		return GUI::Result::FAIL;
	}

	PixelShaderData* mappedPS_Data = nullptr;
	if (mPS_DataBuffer.Map(reinterpret_cast<void**>(&mappedPS_Data)) == GUI::Result::SUCCESS)
	{
		// ライトの方向
		mappedPS_Data->lightDir = MathUtil::Vector(-1.f, -1.f, 1.f).GetFloat3();
		mPS_DataBuffer.Unmap();
	}
	else
	{
		return GUI::Result::FAIL;
	}

	return GUI::Result::SUCCESS;

}

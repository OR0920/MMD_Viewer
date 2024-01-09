//
// MMDモデルを読み込むモジュール
//

#ifndef _MMD_SDK_H_
#define _MMD_SDK_H_

#include<cstdint>

namespace MMDsdk
{
	// ジャンプ用
	class PmdFile;
	class PmxFile;


	// モデルファイル内のデータを格納する構造体
	// MathUtil::float3とは役割が明確に異なるため、
	// 別で用意する //
	struct float2
	{
		float x = 0.f;
		float y = 0.f;
	};
	struct float3
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
	};
	struct float4
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		float w = 0.f;
	};

	// 内部文字列のエンコード方式
	enum EncodeType : uint8_t
	{
		UTF16 = 0,
		UTF8 = 1,
		INIT = 0xff
	};

	//　.pmd, .pmx　共通の列挙子
	
	// 0：base、1：まゆ、2：目、3：リップ、4：その他
	enum MorphType : uint8_t
	{
		MT_BASE = 0,
		MT_EYEBROW = 1,
		MT_EYE = 2,
		MT_LIP = 3,
		MT_OTHER = 4,
		MT_NONE
	};

	enum RigitbodyShapeType : uint8_t
	{
		RST_SPHERE = 0,
		RST_BOX = 1,
		RST_CAPSULE = 2,
		RST_NONE,
	};

	enum RigitbodyType : uint8_t
	{
		RT_BONE_FOLLOW = 0,
		RT_RIGITBODY = 1,
		RT_RIGITBODY_WITH_BONE_OFFS = 2,
		RT_NONE,
	};

	// テキスト取得マクロ
	// 旧　GetText　設計ミスの名残
	// 互換性保持のため一応残している
#define GetTextMacro(textBuffer) textBuffer.GetText()

	// テキストを格納、扱うクラス
	// 可変長 //
	class TextBufferVariable
	{
	public:
		TextBufferVariable(); ~TextBufferVariable();

		void Load(void* _file, EncodeType encode);

		const int GetLength() const;
		// 文字列取得 
		const char* const GetText() const;
	private:
		// 内部でnewを行うため隔離
		TextBufferVariable(const TextBufferVariable&);
		const TextBufferVariable& operator=(const TextBufferVariable&) const;

		int mLength;
		char* mStr;
	};

	// 固定長文字列
	// 宣言時に長さが決まっている場合はこちらを使用する
	// newを行わない分、多少速いはず //
	template<size_t size>
	class TextBufferFixed
	{
	public:
		TextBufferFixed() {} ~TextBufferFixed() {}

		const int GetLength() const
		{
			return mLength;
		}

		// 文字列取得 
		const char* const GetText() const
		{
			return mStr;
		}
	private:
		TextBufferFixed(TextBufferFixed&);
		const TextBufferFixed& operator=(const TextBufferFixed&) const;

		const int mLength = size;
		char mStr[size]{ '\0' };
	};

	// pmdファイルを読み込み、内部構造のまま安全に公開するクラス
	// より上位のクラス（例：Modelクラスなど）がファイルのデータを得るためのクラス
	// 
	// intなどの型を使用すると、将来的に変わってしまう可能性があるため、
	// ビット数が明記された型名(int32_t等)を使用する
	// 
	// 配列は固定長、可変長関わらず隔離している
	// アクセス関数を使用すること
	// 
	class PmdFile
	{
	public:
		// ファイルを指定してインスタンスを生成すること
		// コンストラクタの処理で一気にファイルを読み込み閉じる
		PmdFile(const char* const filepath);  ~PmdFile();

		bool IsSuccessLoad () const;

		// ディレクトリのパスを取得する
		// テクスチャなどのファイルはすべて同じディレクトリに入れること //
		const char* const GetDirectoryPath() const;

		// 以下データの構造体とその取得関数をペアで記述する
		// 内部に配列を持つ場合は専用のロード関数を用意している
		// struct __		: 構造体
		// Get__Count()		: データ数取得
		// Get__(i)			: データ取得
		// GetLast__ID()	: 最後尾のインデックスを取得する
		// DebugOut__(i)	: 要素単位でのコンソール出力
		// DebugOutAll__()	: 全要素の出力

		struct Header;
		struct Vertex;
		struct Material;
		struct Bone;
		struct IK_Data;
		struct Morph;
		struct Rigitbody;
		struct Joint;

		// 基本情報
		struct Header
		{
			float version = 0.f;

			struct ModelInfo
			{
				TextBufferFixed<20> modelName = {};
				TextBufferFixed<256> comment = {};

				ModelInfo(); ~ModelInfo();
			private:
				ModelInfo(ModelInfo&);
				const ModelInfo& operator=(ModelInfo&) const;

			} modelInfoJP = {};

			// 拡張機能(1)　英語対応状況　英語モデル名
			// 実際はファイル末尾に存在 //
			uint8_t isEngAvailable = 0;
			ModelInfo modelInfoEng = {};

			// ライブラリが使用する関数
			void DebugOut() const;
			Header(); ~Header();
		private:
			// コピー禁止
			Header(const Header&);
			const Header& operator=(const Header&) const;
		};
		const Header& GetHeader() const;
		void DebugOutHeader() const;

		// 頂点データ
		const uint32_t& GetVertexCount() const;
		struct Vertex
		{
			float3 position = {};
			float3 normal = {};
			// uv.x : u, uv.y : v
			float2 uv = {};
		private:
			uint16_t boneID[2] = {};
		public:
			uint8_t weight = 0;

			// アウトラインを描画するか
			enum EdgeEnable : uint8_t
			{
				VEE_ENABLE = 0,
				VEE_DISABLE = 1,
				VEE_NONE,
			} edgeFlag = VEE_NONE;

			// ボーンID取得関数　範囲外チェック付き
			const uint16_t& GetBoneID(const int i) const;

			// 配列読み込み用関数 
			// ライブラリが使用する関数
			void LoadBoneID(void* _file);
			void DebugOut() const;
			Vertex(); ~Vertex();
		private:
			Vertex(const Vertex&);
			const Vertex& operator=(const Vertex&) const;
		};
		const Vertex& GetVertex(const uint32_t i) const;
		const uint32_t GetLastVertexID() const;
		void DebugOutVertex(const uint32_t i) const;
		void DebugOutAllVertex() const;

		//インデックスデータ
		const uint32_t& GetIndexCount() const;
		const uint16_t& GetIndex(const uint32_t i) const;
		const uint32_t GetLastIndexID() const;
		void DebugOutIndexData(const uint32_t i) const;
		void DebugOutAllIndexData() const;

		//マテリアルデータ
		const uint32_t& GetMaterialCount() const;

		struct Material
		{
			//color r g b a : x y z w
			float4 diffuse = {};
			float specularity = 0.f;
			float3 specular = {};
			float3 ambient = {};

			// 共有テクスチャのID
			int8_t toonIndex = 0;

			// 輪郭を表示するか 
			// なぜか頂点データとフラグが逆。何故？ //
			enum EdgeEnable : uint8_t
			{
				MEE_ENABLE = 1,
				MEE_DISABLE = 0,
				MEE_NONE = 99,
			} edgeFlag = MEE_NONE;

			uint32_t vertexCount = 0;
			TextBufferFixed<20> texturePath = {};

			// ライブラリが使用する関数
			void DebugOut() const;
			Material(); ~Material();
		private:
			Material(const Material&);
			const Material& operator=(const Material&) const;
		};
		const Material& GetMaterial(const uint32_t i) const;
		const uint32_t GetLastMaterialID() const;
		void DebugOutMaterial(const uint32_t i) const;
		void DebugOutAllMaterial() const;

		//ボーンデータ
		const uint16_t& GetBoneCount() const;
		struct Bone
		{
			TextBufferFixed<20>	name = {};
			uint16_t parentIndex = 0;
			uint16_t childIndex = 0;

			// 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 
			// 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動
			// 英訳は適当
			// 内部仕様が公式に公開されていないため 3:不明 は本当に不明
			enum BoneType : uint8_t
			{
				BT_ROTATE = 0,
				BT_ROTATE_AND_TRANSRATE = 1,
				BT_IK = 2,
				BT_UNKNOWN = 3,
				BT_IK_INFULENCED = 4,
				BT_ROTATE_INFULUENCED = 5,
				BT_IK_CONECT = 6,
				BT_UNVISIBLE = 7,
				BT_TWIST = 8,
				BT_ROTATION_MOVE = 9,
				BT_NONE
			} type = BT_NONE;

			uint16_t ikParentIndex = 0;
			float3 headPos = {};
			// 拡張機能(2)
			TextBufferFixed<20> nameEng = {};

			// ライブラリが使用する関数
			void DebugOut() const;
			Bone(); ~Bone();
		private:
			Bone(const Bone&);
			const Bone& operator=(const Bone&) const;
		};
		const Bone& GetBone(const uint16_t i) const;
		const uint32_t GetLastBoneID() const;
		void DebugOutBone(const uint16_t i) const;
		void DebugOutAllBone() const;

		//ikデータ
		const uint16_t& GetIKCount() const;
		struct IK_Data
		{
			uint16_t ikBoneIndex = 0;
			uint16_t ikTargetBoneIndex = 0;
			uint8_t ikChainCount = 0;
			uint16_t iterations = 0;
			float controlWeight = 0.f;
		private:
			// 可変長配列　:　要素数　ikChainLength
			// 念のため隔離
			uint16_t* ikChildBoneIndexArray = 0;
		public:
			// 可変長配列を取得する
			const uint16_t& GetIkChildBoneID(const int i) const;



			// ライブラリが使用する関数

			// 可変長配列のロード用関数、コンストラクタ　デストラクタ
			// ユーザー側から呼び出すことはない
			void LoadIkChildBoneID(void* _file);
			void DebugOut() const;
			IK_Data(); ~IK_Data();
		private:
			IK_Data(const IK_Data&);
			const IK_Data& operator=(const IK_Data&) const;
		};
		const IK_Data& GetIKData(const uint16_t i) const;
		const uint16_t GetLastIKDataID() const;
		void DebugOutIKData(const uint16_t i) const;
		void DebugOutAllIK() const;

		//表情データ(pmd後継のpmxでは、モーフと呼ばれているため、そちらに準拠する)
		const uint16_t& GetMorphCount() const;
		//Baseを除いた表情数(pmdエディタ上で表示されている表情数)
		const uint16_t GetMorphCountNotIncludeBase() const;
		struct Morph
		{
			TextBufferFixed<20> name = {};
			uint32_t offsCount = 0;

			MorphType type = MorphType::MT_NONE;

			// typeで変数の役割が異なる
			// 構造は同じ //
			struct MorphOffsData
			{
				uint32_t baseIndex = 0;
				float3 offsPosition = {};

				MorphOffsData(); ~MorphOffsData();
			private:
				MorphOffsData(MorphOffsData&);
				const MorphOffsData& operator=(const MorphOffsData&) const;
			};
			struct MorphBaseData
			{
				uint32_t index = 0;
				float3 positon = {};

				MorphBaseData(); ~MorphBaseData();
			private:
				MorphBaseData(MorphBaseData&);
				const MorphBaseData& operator=(const MorphBaseData&) const;
			};
		private:
			//可変長配列　:　要素数　vertexCount
			union MorphData
			{
				MorphBaseData base = {};
				MorphOffsData offs;

				MorphData(); ~MorphData();
			private:
				MorphData(const MorphData&);
				const MorphData& operator=(const MorphData&) const;
			} *morphData = nullptr;

		public:
			// 拡張機能(3)
			TextBufferFixed<20> nameEng = {};

			// baseとして頂点データを取得
			const MorphBaseData& GetMorphBaseData(const int i) const;
			// 通常の頂点データとして取得
			const MorphOffsData& GetMorphOffsData(const int i) const;

			// ライブラリが使用する関数
			// 可変長配列のロード用関数
			void LoadSkinVertex(void* _file);

			// デバッグ出力　内部に長めの可変長配列を持つため、表示するかを選ぶ
			void DebugOut(bool isOutVertexData = false) const;
			Morph(); ~Morph();
		private:
			Morph(const Morph&);
			const Morph& operator=(const Morph&) const;
		};
		//i = 0はbase
		const Morph& GetMorph(const uint16_t i) const;
		const uint16_t GetLastMorphID() const;
		void DebugOutMorph(const uint16_t i, bool isOutVertexData = false) const;
		void DebugOutAllMorph(bool isOutVertexData = false) const;

		// モーフ用表示枠データ
		// モーフ(表情)データをMMDエディタ上に表示する際に
		// 使用されるデータだと思われる
		// pmdエディタ上では、
		// baseを除いたインデックスが表示されており、
		// 格納されているデータはbaseを含めた
		// インデックスとなっている //
		const uint8_t& GetMorphIndexForDisplayCount() const;
		// baseを含めたインデックスを返す //
		const uint16_t& GetMorphIndexForDisplay(const uint8_t i) const;
		const uint8_t GetLastMorphIndexForDisplayID() const;
		void DebugOutMorphIndexForDisplay(const uint8_t i) const;
		void DebugOutAllMorphIndexForDisplay() const;

		// 表示用ボーン名データ //
		const uint8_t& GetBoneNameForDisplayCount() const;
		const TextBufferFixed<50>& GetBoneNameForDisplay(const uint8_t i) const;
		const TextBufferFixed<50>& GetBoneNameForDisplayEng(const uint8_t i) const;
		const uint8_t GetLastBoneNameForDisplayID() const;
		void DebugOutBoneNameForDisplay(const uint8_t i) const;
		void DebugOutAllBoneNameForDisplay() const;

		// ボーン枠用枠名の読み込み
		const uint32_t& GetBoneForDisplayCount() const;
		struct BoneForDisplay
		{
			uint16_t boneIndexForDisplay = 0;
			uint8_t boneDisplayFrameIndex = 0;

			void DebugOut() const;
			BoneForDisplay(); ~BoneForDisplay();
		private:
			BoneForDisplay(const BoneForDisplay&);
			const BoneForDisplay& operator=(const BoneForDisplay&) const;
		};
		const BoneForDisplay& GetBoneForDisplay(const uint32_t i) const;
		const uint32_t GetLastBoneForDisplayID() const;
		void DebugOutBoneForDisplay(const uint32_t i) const;
		void DebugOutAllBoneForDisplay() const;

		const TextBufferFixed<100>& GetToonTexturePath(const uint8_t i) const;
		void DebugOutToonTexturePath(const uint8_t i) const;
		void DebugOutAllToonTexturePath() const;

		// 以下、物理演算用のデータ
		// 勉強中のため、不正確なネーミングになっている可能性あり
		// 物理演算を実装する際に名前を変える予定。 //

		// 剛体データ 
		const uint32_t& GetRigitbodyCount() const;

		struct Rigitbody
		{
			TextBufferFixed<20> name = {};
			uint16_t relationshipBoneID = 0;
			// グループの番号　グループ1 = 0, グループ16 = 15,
			uint8_t group = 0;
			// 衝突対象のオブジェクト
			// エディタ上では「非衝突対象」を選択するチェックボックスが表示されている
			// 16~5,4,3,2,1 の順に並んでおり、
			// 例えばエディタ上で6番が非衝突としてチェックされている場合は
			// 0x1111_1111_1101_1111 が格納される//
			uint16_t groupTarget = 0;

			RigitbodyShapeType shapeType = RigitbodyShapeType::RST_NONE;

			float shapeW = 0;
			float shapeH = 0;
			float shapeD = 0;
			// ファイル内の値は、関連ボーンの座標に対する
			// オフセット値
			// pmdエディタ上の値はボーン座標に
			// オフセットを加算した実際の座標 //
			float3 position = {};
			// 単位　ラジアン
			float3 rotation = {};
			float weight = 0.f;
			float positionDim = 0.f;
			float rotationDim = 0.f;
			float recoil = 0.f;
			float friction = 0.f;

			RigitbodyType type = RigitbodyType::RT_NONE;

			void DebugOut() const;
			Rigitbody(); ~Rigitbody();
		private:
			Rigitbody(const Rigitbody&);
			const Rigitbody& operator=(const Rigitbody&) const;
		};
		const Rigitbody& GetRigitbody(const uint32_t i) const;
		const uint32_t GetLastRigitbodyID() const;
		void DebugOutRigitbody(const uint32_t i) const;
		void DebugOutAllRigitbody() const;

		const uint32_t& GetJointCount() const;
		struct Joint
		{
			TextBufferFixed<20> name = {};

			uint32_t rigitbodyIndexA = 0;
			uint32_t rigitbodyIndexB = 0;

			float3 position = {};
			float3 rotation = {};

			float3 posLowerLimit = {};
			float3 posUpperLimit = {};

			// 単位: ラジアン
			float3 rotLowerLimit = {};
			float3 rotUpperLimit = {};

			float3 springPos = {};
			// 単位不明
			// 上記の回転制限と異なり、変換しなくてもpmdエディタ上と
			// 同じ値となるため、度、あるいは比率など異なる単位の
			// パラメータか。詳細不明 //
			float3 springRot = {};


			void DebugOut() const;
			Joint(); ~Joint();
		private:
			Joint(const Joint&);
			const Joint& operator=(const Joint&) const;
		};
		const Joint& GetJoint(const uint32_t i) const;
		const uint32_t GetLastJointID() const;
		void DebugOutJoint(const uint32_t i) const;
		void DebugOutAllJoint() const;

		//　全データを表示する
		void DebugOutAllData() const;

	private:
		//既定のコンストラクタ、コピーコンストラクタ呼び出し禁止
		//必ずファイル名を指定して生成すること
		PmdFile(); PmdFile(const PmdFile& copy);
		const PmdFile& operator=(const PmdFile&) const;

		bool isSuccess;

		char* mDirectoryPath;

		//　各種データ実態
		Header mHeader;

		uint32_t mVertexCount;
		Vertex* mVertex;

		uint32_t mIndexCount;
		uint16_t* mIndex;

		uint32_t mMaterialCount;
		Material* mMaterial;

		uint16_t mBoneCount;
		Bone* mBone;

		uint16_t mIKCount;
		IK_Data* mIK;

		uint16_t mMorphCount;
		Morph* mMorph;

		uint8_t mMorphForDisplayCount;
		uint16_t* mMorphForDisplay;

		uint8_t mBoneNameForDisplayCount;
		// 日本語表示用ボーン名は改行文字('\n' : 0x0a)で終わる　
		TextBufferFixed<50>* mBoneNameForDisplay;

		// 拡張機能(4)
		// 表示用ボーン名英語 //
		TextBufferFixed<50>* mBoneNameForDisplayEng;

		uint32_t mBoneForDisplayCount;
		BoneForDisplay* mBoneForDisplay;

		TextBufferFixed<100> mToonTexturePath[10];

		uint32_t mRigitbodyCount;
		Rigitbody* mRigitbody;

		uint32_t mJointCount;
		Joint* mJoint;
	};

	// pmxファイルを読み込み、
	// ファイル構造のまま公開するクラス
	// 基本的なインターフェイスはpmdと統一する　//
	class PmxFile
	{
	public:
		PmxFile(const char* const filepath); ~PmxFile();

		bool IsSuccessLoad() const;

		const char* const GetDirectoryPath() const;

		struct Header;
		struct Vertex;
		struct Material;
		struct Bone;
		struct Morph;
		struct Rigitbody;
		struct Joint;

		// ヘッダ情報
		struct Header
		{
			float version = 0.f;
			uint8_t fileConfigLength = 0;
			EncodeType encode = EncodeType::INIT;
			uint8_t additionalUVcount = 0;
			uint8_t vertexID_Size = 0;
			uint8_t textureID_Size = 0;
			uint8_t materialID_Size = 0;
			uint8_t boneID_Size = 0;
			uint8_t morphID_Size = 0;
			uint8_t rigitbodyID_Size = 0;

			struct ModelInfo
			{
				TextBufferVariable modelName = {};
				TextBufferVariable comment = {};

				ModelInfo(); ~ModelInfo();
			private:
				ModelInfo(const ModelInfo&);
				const ModelInfo& operator=(const ModelInfo&) const;
			} modelInfoJP = {};

			ModelInfo modelInfoEng = {};

			void DebugOut() const;
			Header(); ~Header();
		private:
			Header(const Header&);
			const Header& operator=(const Header&) const;
		};
		const Header& GetHeader() const;
		void DebugOutHeader() const;

		// 頂点情報
		const int32_t& GetVertexCount() const;
		struct Vertex
		{
			float3 position = {};
			float3 normal = {};
			float2 uv = {};
		private:
			float4 addtionalUV[4] = { {} };
		public:
			// BDEF1	:1つのボーンによる変形
			// BDEF2	:2つのボーンによる変形
			// BDEF4	:4つのボーンによる変形
			// SDEF		:スフィリカルデフォーム
			//			 対応しない場合はBDEF2としても利用可  //
			enum WeightType : uint8_t
			{
				BDEF1 = 0,
				BDEF2 = 1,
				BDEF4 = 2,
				SDEF = 3,
				WEIGHT_TYPE = 4,
			} weightType = WEIGHT_TYPE;
		private:
			int32_t boneID[4] = {};
			float weight[4] = {};
		public:
			float3 sdefC = {};
			float3 sdefR0 = {};
			float3 sdefR1 = {};
			float edgeRate = 0.f;

			// 隔離した配列のアクセス関数
			const float4& GetAddtionalUV(const int32_t i) const;
			const int32_t& GetBoneID(const int32_t i) const;
			const float& GetWeight(const int32_t i) const;

			// ライブラリ側が呼び出す関数
			void LoadAddtionalUV(void* _file, const int32_t addtionalUVCount);
			void LoadBoneIDAndWeight(void* _file, const uint8_t boneID_Size);
			void DebugOut() const;
			Vertex(); ~Vertex();
		private:
			Vertex(const Vertex&);
			const Vertex& operator=(const Vertex&) const;
		};
		const Vertex& GetVertex(const int32_t i) const;
		const int32_t GetLastVertexID() const;
		void DebugOutVertex(const int32_t i) const;
		void DebugOutAllVertex() const;


		// インデックスデータ
		const int32_t& GetIndexCount() const;
		const int32_t& GetIndex(const int32_t i) const;
		const int32_t GetLastIndexID() const;
		void DebugOutIndexData(const int32_t i) const;
		void DebugOutAllIndexData() const;

		// テクスチャデータ
		const int32_t& GetTextureCount() const;
		const TextBufferVariable& GetTexturePath(const int32_t) const;
		const int32_t GetLastTextureID() const;
		void DebugOutTexturePath(const int32_t i) const;
		void DebugOutAllTexturePath() const;

		// マテリアルデータ
		const int32_t& GetMaterialCount() const;
		struct Material
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};

			float4 diffuse = {};
			float3 specular = {};
			float specularity = 0.f;
			float3 ambient = {};
			// 背面を描画するか
			// 地面影を描画するか
			// セルフシャドウマップへの描画を行うか
			// セルフ車道を描画するか
			// エッジを描画するか　//
			enum DrawConfig : uint8_t
			{
				DC_NOT_CULLING = 0b0000'0001,
				DC_DRAW_SHADOW = 0b0000'0010,
				DC_DRAW_TO_SELF_SHADOW_MAP = 0b0000'0100,
				DC_DRAW_SELF_SHADOW = 0b0000'1000,
				DC_DRAW_EDGE = 0b0001'0000,
				DC_NONE = 0b0000'0000
			} drawConfig = DC_NONE;
			float4 edgeColor = {};
			float edgeSize = 0.f;
			int32_t textureID = 0;
			int32_t sphereTextureID = 0;
			enum SphereMode : uint8_t
			{
				SM_DISABLE = 0,
				SM_SPH = 1,
				SM_SPA = 2,
				SM_SUB_TEXTURE = 3,
				SM_NONE,
			} sphereMode = SM_NONE;
			// toonテクスチャが共有のものかどうか
			// 特にテクスチャを持たない場合は、TM_UNIQUE // 
			enum ToonMode : uint8_t
			{
				TM_UNIQUE = 0,
				TM_SHARED = 1,
				TM_NONE,
			} toonMode = TM_NONE;
			int32_t toonTextureID = 0;
			TextBufferVariable memo = {};
			int32_t vertexCount = 0;

			// 描画設定を取得する関数
			// DrawConfigの列挙子を指定し、有効であれば　trueが返る。
			bool GetDrawConfig(DrawConfig bitflag) const;

			// ライブラリ側が使用する関数
			void DebugOut() const;
			Material(); ~Material();
		private:
			Material(const Material&);
			const Material& operator=(const Material&) const;
		};
		const Material& GetMaterial(const int32_t i) const;
		const int32_t GetLastMaterialID() const;
		void DebugOutMaterial(const int32_t i) const;
		void DebugOutAllMaterial() const;


		// ボーンデータ
		const int32_t& GetBoneCount() const;
		struct Bone
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			float3 position = {};
			int32_t parentBoneID = 0;
			int32_t transformHierarchy = 0;
			/*
			一部名前はDxLibを参考にした
			0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
			0x0002  : 回転可能
			0x0004  : 移動可能
			0x0008  : 表示
			0x0010  : 操作可
			0x0020  : IK
			0x0080  : ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
			0x0100  : 回転付与
			0x0200  : 移動付与
			0x0400  : 軸固定
			0x0800  : ローカル軸
			0x1000  : 物理後変形
			0x2000  : 外部親変形
			*/
			enum BoneConfig : uint16_t
			{
				BC_IS_LINK_DEST_BY_BONE = 0x0001,
				BC_ENABLE_ROTATE = 0x0002,
				BC_ENABLE_MOVE = 0x0004,
				BC_DISPLAY = 0x0008,
				BC_ENABLE_CONTROLL = 0x0010,
				BC_IK = 0x0020,
				BC_IS_ADD_LOCAL_BY_PARENT = 0x0080,
				BC_ADD_ROT = 0x0100,
				BC_ADD_MOV = 0x0200,
				BC_LOCK_AXIS = 0x0400,
				BC_LOCAL_AXIS = 0x0800,
				BC_AFTER_PHISICS_TRANSFORM = 0x1000,
				BC_OUT_PARENT_TRANSFORM = 0x2000,
				BC_NONE = 0x0000
			} bc = BC_NONE;

			union
			{
				// 接続先が座標オフセットの場合　0x0001 == true
				float3 positionOffs = {};
				// 接続先がボーンの場合 0x0001 == false
				int32_t linkDestBoneID;
			};

			struct AddData
			{
				int32_t addPalentBoneID = -1;
				float addRatio = 0.f;

				AddData(); ~AddData();
			private:
				AddData(const AddData&);
				const AddData& operator=(const AddData&) const;
			} addRot = {}, addMov = {};

			float3 axisDirection = {};

			float3 localAxisX_Direction = {};
			float3 localAxisZ_Direction = {};

			int32_t outParentTransformKey = 0;

			int32_t ikTargetBoneID = -1;
			int32_t ikLoopCount = 0;
			float ikRotateLimit = 0.f;

			int32_t ikLinkCount = 0;

			struct IK_Link
			{
				int32_t linkBoneID = -1;
				enum RotLimitConfig : uint8_t
				{
					IK_RLC_OFF = 0,
					IK_RLC_ON = 1,
					IK_RLC_NONE
				} rotLimitConfig = IK_RLC_NONE;
				float3 lowerLimit = {};
				float3 upperLimit = {};

				IK_Link(); ~IK_Link();
			private:
				IK_Link(const IK_Link&);
				const IK_Link& operator=(const IK_Link&) const;
			};
		private:
			// IKリンクデータ本体の可変長配列
			IK_Link* ikLink = nullptr;//[ikLinkCount]
		public:

			// ボーンの設定を取得する
			// BoneConfigの列挙子を指定すると、
			// その項目が有効かどうかが返る
			// 有効　:true　無効　:false //
			bool GetBoneConfig(BoneConfig bitFlag) const;
			// IKリンクデータを取得する関数
			const IK_Link& GetIK_Link(const int32_t i) const;



			// ライブラリ側が使用する関数
			void LoadIK_Link(void* file, const size_t boneID_Size);

			void DebugOut() const;
			Bone(); ~Bone();
		private:
			Bone(const Bone&);
			const Bone& operator=(const Bone&) const;
		};
		const Bone& GetBone(const int32_t i) const;
		const int32_t GetLastBoneID() const;
		void DebugOutBone(const int32_t i) const;
		void DebugOutAllBone() const;

		// モーフデータ
		const int32_t& GetMorphCount() const;
		struct Morph
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			// pmd と共通
			MorphType type = MorphType::MT_NONE;
			// pmx で追加
			// グループモーフ　:2つ以上のモーフを同時行うモーフ
			// 頂点モーフ　:頂点位置を変更するモーフpmdのモーフはすべてこれ
			// ボーンモーフ　:ボーン位置が変更されることによるモーフ
			// UVモーフ　:テクスチャの切り出し位置を変更するモーフ
			// マテリアルモーフ　:マテリアルを変化させるモーフ //
			enum MorphTypeEX : uint8_t
			{
				MTEX_GROUP = 0,
				MTEX_VERTEX = 1,
				MTEX_BONE = 2,
				MTEX_UV = 3,
				MTEX_ADDTIONAL_UV_1 = 4,
				MTEX_ADDTIONAL_UV_2 = 5,
				MTEX_ADDTIONAL_UV_3 = 6,
				MTEX_ADDTIONAL_UV_4 = 7,
				MTEX_MATERIAL = 8,
				MTEX_NONE,
			} typeEX = MTEX_NONE;

			// モーフのオフセットデータの数
			int32_t offsCount = 0;

			// グループモーフのオフセットデータ
			struct GroupOffs
			{
				int32_t morphID = -1;
				float morphRatio = 0.f;
				void DebugOut() const;

				GroupOffs(); ~GroupOffs();
			private:
				GroupOffs(const GroupOffs&);
				const GroupOffs& operator=(const GroupOffs&) const;
			};

			// 頂点モーフのオフセットデータ
			struct VertexOffs
			{
				int32_t vertexID = -1;
				float3 offsPos = {};
				void DebugOut() const;

				VertexOffs(); ~VertexOffs();
			private:
				VertexOffs(const VertexOffs&);
				const VertexOffs& operator=(const VertexOffs&) const;
			};

			// ボーンモーフのオフセットデータ
			struct BoneOffs
			{
				int32_t boneID = -1;
				float3 offsPos = {};
				float4 offsRotQ = {};
				void DebugOut() const;

				BoneOffs(); ~BoneOffs();
			private:
				BoneOffs(const BoneOffs&);
				const BoneOffs& operator=(const BoneOffs&) const;
			};

			// UVモーフのオフセットデータ
			struct UV_Offs
			{
				int32_t vertexID = -1;
				// 通常のUVの場合、z, w成分は使用しないが、データ上存在
				float4 uv = {};
				void DebugOut() const;

				UV_Offs(); ~UV_Offs();
			private:
				UV_Offs(const UV_Offs&);
				const UV_Offs& operator=(const UV_Offs) const;
			};

			// マテリアルモーフのオフセットデータ
			struct MaterialOffs
			{
				int32_t materialID = -1;
				enum OffsType : uint8_t
				{
					OT_MUL = 0,
					OT_ADD = 1,
					OT_NONE
				} offsType = OT_NONE;
				float4 diffuse = {};
				float3 specular = {};
				float specularity = 0;
				float3 ambient = {};
				float4 edgeColor = {};
				float edgeSize = 0;
				float4 textureScale = {};
				float4 sphereTextureScale = {};
				float4 toonTextureScale = {};

				void DebugOut() const;

				MaterialOffs(); ~MaterialOffs();
			private:
				MaterialOffs(const MaterialOffs&);
				const MaterialOffs& operator=(const MaterialOffs&) const;
			};

			// オフセットデータを格納する共用体
			// 1つのモーフデータに複数種のオフセットが格納されることはない //
			union MorphOffsData
			{
				GroupOffs groupOffs;
				VertexOffs vertexOffs;
				BoneOffs boneOffs;
				UV_Offs uvOffs;
				MaterialOffs materialOffs = {};

				MorphOffsData(); ~MorphOffsData();
			private:
				MorphOffsData(const MorphOffsData&);
				const MorphOffsData& operator=(const MorphOffsData&) const;
			};

		private:
			// オフセットデータ本体　可変長配列
			MorphOffsData* morphOffsData = nullptr;//[offsCount]
		public:

			// オフセットデータを取得する関数　出力先で、使用するデータを選択する
			const MorphOffsData& GetMorphOffsData(const int32_t i) const;


			// ライブラリ側が使用する関数
			void LoadOffsData(void* _file, const size_t idByteSize);
			void DebugOut(bool isOutOffsData) const;

			Morph(); ~Morph();
		private:
			Morph(const Morph&);
			const Morph& operator=(const Morph&) const;
		};
		const Morph& GetMorph(const int32_t i) const;
		const int32_t GetLastMorphID() const;
		void DebugOutMorph(const int32_t i, bool isOutOffsData = false) const;
		void DebugOutAllMorph(bool isOutOffsData = false) const;


		// 表示枠データ
		const int32_t& GetDisplayFrameCount() const;
		struct DisplayFrame
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			enum DisplayFrameType : uint8_t
			{
				DFT_NORMAL = 0,
				DFT_SPECIAL = 1,
				DFT_NONE
			} type = DFT_NONE;
			int32_t frameElementCount = 0;
			struct FrameElement
			{
				enum FrameElementType : uint8_t
				{
					FET_BONE = 0,
					FET_MORPH = 1,
					FET_NONE
				} elementType = FET_NONE;
				int32_t objectID = -1;

				FrameElement(); ~FrameElement();
			private:
				FrameElement(FrameElement&);
				const FrameElement& operator=(const FrameElement&) const;
			};
		private:
			FrameElement* frameElement = nullptr;
		public:

			const FrameElement& GetFrameElement(const int32_t i) const;

			// ライブラリが使用する関数
			void LoadFrameElement(void* _file, const size_t boneID_Size, const size_t morphID_Size);
			void DebugOut() const;
			DisplayFrame(); ~DisplayFrame();
		private:
			DisplayFrame(const DisplayFrame&);
			const DisplayFrame& operator=(const DisplayFrame&) const;
		};
		const DisplayFrame& GetDisplayFrame(const int32_t i) const;
		const int32_t GetLastDisplayFrameID() const;
		void DebugOutDisplayFrame(const int32_t i) const;
		void DebugOutAllDisplayFrame() const;

		// 剛体データ
		const int32_t& GetRigitbodyCount() const;
		struct Rigitbody
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			int32_t relationshipBoneID = -1;
			uint8_t group = 0;
			// 衝突対象のオブジェクト
			// エディタ上では「非衝突対象」を選択するチェックボックスが表示されている
			// 16~5,4,3,2,1 の順に並んでおり、
			// 例えばエディタ上で6番が非衝突としてチェックされている場合は
			// 0x1111_1111_1101_1111 が格納される//
			uint16_t groupTarget = 0;

			RigitbodyShapeType shapeType = RigitbodyShapeType::RST_NONE;

			float shapeW = 0.f;
			float shapeH = 0.f;
			float shapeD = 0.f;
			// pmdと異なり、pmxエディタ上の値と同じ値が格納されている
			float3 position = {};
			float3 rotation = { 4.f };
			float weight = 0.f;
			float positionDim = 0.f;
			float rotationDim = 0.f;
			float recoil = 0.f;
			float friction = 0.f;

			RigitbodyType type = RigitbodyType::RT_NONE;

			void DebugOut() const;
			Rigitbody(); ~Rigitbody();
		private:
			Rigitbody(const Rigitbody&);
			const Rigitbody& operator=(const Rigitbody&) const;
		};
		const Rigitbody& GetRigitbody(const int32_t i) const;
		const int32_t GetLastRigitbodyID() const;
		void DebugOutRigitbody(const int32_t) const;
		void DebugOutAllRigitbody() const;

		// ジョイントデータ
		const int32_t& GetJointCount() const;
		struct Joint
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};

			enum JointType : uint8_t
			{
				// PMX2.0では1つのみ
				JT_SPRING_6_DOF = 0,
				JT_NONE
			} type = JT_NONE;

			int32_t rigitbodyIndexA = 0;
			int32_t rigitbodyIndexB = 0;

			float3 position = {};
			float3 rotation = {};
			float3 posLowerLimit = {};
			float3 posUpperLimit = {};
			float3 rotLowerLimit = {};
			float3 rotUpperLimit = {};
			float3 springPos = {};
			float3 springRot = {};

			void DebugOut() const;
			Joint(); ~Joint();
		private:
			Joint(const Joint&);
			const Joint& operator=(const Joint&) const;
		};
		const Joint& GetJoint(const int32_t i) const;
		const int32_t GetLastJointID() const;
		void DebugOutJoint(const int32_t i) const;
		void DebugOutAllJoint() const;

		void DebugOutAllData() const;

		//last

	private:
		// デフォルト、コピー禁止
		PmxFile(); PmxFile(const PmxFile& copy);
		const PmxFile& operator=(const PmxFile&) const;

		bool mIsSuccess;

		char* mDirectoryPath;

		Header mHeader;

		int32_t mVertexCount;
		Vertex* mVertex;

		int32_t mIndexCount;
		int32_t* mIndex;

		int32_t mTextureCount;
		TextBufferVariable* mTexturePath;

		int32_t mMaterialCount;
		Material* mMaterial;

		int32_t mBoneCount;
		Bone* mBone;

		int32_t mMorphCount;
		Morph* mMorph;

		int32_t mDisplayFrameCount;
		DisplayFrame* mDisplayFrame;

		int32_t mRigitbodyCount;
		Rigitbody* mRigitbody;

		int32_t mJointCount;
		Joint* mJoint;

	};

	// モーションファイル(実装中)
	class VmdFile
	{
	public:
		VmdFile(const char* const filepath); ~VmdFile();

		struct Header
		{
			TextBufferFixed<30> sigunature = {};
			TextBufferFixed<20> defaultModelName = {};

			Header(); ~Header();
		private:
			Header(const Header&);
			const Header& operator=(const Header&) const;
		};
		const Header& GetHeader() const;

		const int32_t& GetMortionCount() const;
		struct Mortion
		{
			TextBufferFixed<15> name = {};
			int frameNumber = 0;
			float3 position = { 0.f };
			float4 rotation = { 0.f };

		private:
			char bezierParam[64] = {};
		public:

			const char& GetBezierParam(const int32_t i) const;

			void LoadBezierParam(void* _file);
			Mortion(); ~Mortion();
			void DebugOut()const;
		private:
			Mortion(const Mortion&);
			const Mortion& operator=(const Mortion&) const;
		};
		const Mortion& GetMortion(const int32_t i) const;
		const int32_t GetLastMortionID() const;
		void DebugOutMortion(const int32_t i) const;
		void DebugOutAllMortion() const;

		
	private:
		VmdFile(); VmdFile(VmdFile&);
		const VmdFile& operator=(const VmdFile&) const;

		Header mHeader;

		int32_t mMortionCount;
		Mortion* mMortion;
	};

	//last

}

#endif // !_MMD_SDK_H_

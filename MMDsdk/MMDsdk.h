//
// MMD���f����ǂݍ��ރ��W���[��
//

#ifndef _MMD_SDK_H_
#define _MMD_SDK_H_

#include<cstdint>

namespace MMDsdk
{
	// �W�����v�p
	class PmdFile;
	class PmxFile;


	// ���f���t�@�C�����̃f�[�^���i�[����\����
	// MathUtil::float3�Ƃ͖��������m�ɈقȂ邽�߁A
	// �ʂŗp�ӂ��� //
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

	// ����������̃G���R�[�h����
	enum EncodeType : uint8_t
	{
		UTF16 = 0,
		UTF8 = 1,
		INIT = 0xff
	};

	//�@.pmd, .pmx�@���ʂ̗񋓎q
	
	// 0�Fbase�A1�F�܂�A2�F�ځA3�F���b�v�A4�F���̑�
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

	// �e�L�X�g�擾�}�N��
	// ���@GetText�@�݌v�~�X�̖��c
	// �݊����ێ��̂��߈ꉞ�c���Ă���
#define GetTextMacro(textBuffer) textBuffer.GetText()

	// �e�L�X�g���i�[�A�����N���X
	// �ϒ� //
	class TextBufferVariable
	{
	public:
		TextBufferVariable(); ~TextBufferVariable();

		void Load(void* _file, EncodeType encode);

		const int GetLength() const;
		// ������擾 
		const char* const GetText() const;
	private:
		// ������new���s�����ߊu��
		TextBufferVariable(const TextBufferVariable&);
		const TextBufferVariable& operator=(const TextBufferVariable&) const;

		int mLength;
		char* mStr;
	};

	// �Œ蒷������
	// �錾���ɒ��������܂��Ă���ꍇ�͂�������g�p����
	// new���s��Ȃ����A���������͂� //
	template<size_t size>
	class TextBufferFixed
	{
	public:
		TextBufferFixed() {} ~TextBufferFixed() {}

		const int GetLength() const
		{
			return mLength;
		}

		// ������擾 
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

	// pmd�t�@�C����ǂݍ��݁A�����\���̂܂܈��S�Ɍ��J����N���X
	// ����ʂ̃N���X�i��FModel�N���X�Ȃǁj���t�@�C���̃f�[�^�𓾂邽�߂̃N���X
	// 
	// int�Ȃǂ̌^���g�p����ƁA�����I�ɕς���Ă��܂��\�������邽�߁A
	// �r�b�g�������L���ꂽ�^��(int32_t��)���g�p����
	// 
	// �z��͌Œ蒷�A�ϒ��ւ�炸�u�����Ă���
	// �A�N�Z�X�֐����g�p���邱��
	// 
	class PmdFile
	{
	public:
		// �t�@�C�����w�肵�ăC���X�^���X�𐶐����邱��
		// �R���X�g���N�^�̏����ň�C�Ƀt�@�C����ǂݍ��ݕ���
		PmdFile(const char* const filepath);  ~PmdFile();

		bool IsSuccessLoad () const;

		// �f�B���N�g���̃p�X���擾����
		// �e�N�X�`���Ȃǂ̃t�@�C���͂��ׂē����f�B���N�g���ɓ���邱�� //
		const char* const GetDirectoryPath() const;

		// �ȉ��f�[�^�̍\���̂Ƃ��̎擾�֐����y�A�ŋL�q����
		// �����ɔz������ꍇ�͐�p�̃��[�h�֐���p�ӂ��Ă���
		// struct __		: �\����
		// Get__Count()		: �f�[�^���擾
		// Get__(i)			: �f�[�^�擾
		// GetLast__ID()	: �Ō���̃C���f�b�N�X���擾����
		// DebugOut__(i)	: �v�f�P�ʂł̃R���\�[���o��
		// DebugOutAll__()	: �S�v�f�̏o��

		struct Header;
		struct Vertex;
		struct Material;
		struct Bone;
		struct IK_Data;
		struct Morph;
		struct Rigitbody;
		struct Joint;

		// ��{���
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

			// �g���@�\(1)�@�p��Ή��󋵁@�p�ꃂ�f����
			// ���ۂ̓t�@�C�������ɑ��� //
			uint8_t isEngAvailable = 0;
			ModelInfo modelInfoEng = {};

			// ���C�u�������g�p����֐�
			void DebugOut() const;
			Header(); ~Header();
		private:
			// �R�s�[�֎~
			Header(const Header&);
			const Header& operator=(const Header&) const;
		};
		const Header& GetHeader() const;
		void DebugOutHeader() const;

		// ���_�f�[�^
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

			// �A�E�g���C����`�悷�邩
			enum EdgeEnable : uint8_t
			{
				VEE_ENABLE = 0,
				VEE_DISABLE = 1,
				VEE_NONE,
			} edgeFlag = VEE_NONE;

			// �{�[��ID�擾�֐��@�͈͊O�`�F�b�N�t��
			const uint16_t& GetBoneID(const int i) const;

			// �z��ǂݍ��ݗp�֐� 
			// ���C�u�������g�p����֐�
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

		//�C���f�b�N�X�f�[�^
		const uint32_t& GetIndexCount() const;
		const uint16_t& GetIndex(const uint32_t i) const;
		const uint32_t GetLastIndexID() const;
		void DebugOutIndexData(const uint32_t i) const;
		void DebugOutAllIndexData() const;

		//�}�e���A���f�[�^
		const uint32_t& GetMaterialCount() const;

		struct Material
		{
			//color r g b a : x y z w
			float4 diffuse = {};
			float specularity = 0.f;
			float3 specular = {};
			float3 ambient = {};

			// ���L�e�N�X�`����ID
			int8_t toonIndex = 0;

			// �֊s��\�����邩 
			// �Ȃ������_�f�[�^�ƃt���O���t�B���́H //
			enum EdgeEnable : uint8_t
			{
				MEE_ENABLE = 1,
				MEE_DISABLE = 0,
				MEE_NONE = 99,
			} edgeFlag = MEE_NONE;

			uint32_t vertexCount = 0;
			TextBufferFixed<20> texturePath = {};

			// ���C�u�������g�p����֐�
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

		//�{�[���f�[�^
		const uint16_t& GetBoneCount() const;
		struct Bone
		{
			TextBufferFixed<20>	name = {};
			uint16_t parentIndex = 0;
			uint16_t childIndex = 0;

			// 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 
			// 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^��
			// �p��͓K��
			// �����d�l�������Ɍ��J����Ă��Ȃ����� 3:�s�� �͖{���ɕs��
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
			// �g���@�\(2)
			TextBufferFixed<20> nameEng = {};

			// ���C�u�������g�p����֐�
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

		//ik�f�[�^
		const uint16_t& GetIKCount() const;
		struct IK_Data
		{
			uint16_t ikBoneIndex = 0;
			uint16_t ikTargetBoneIndex = 0;
			uint8_t ikChainCount = 0;
			uint16_t iterations = 0;
			float controlWeight = 0.f;
		private:
			// �ϒ��z��@:�@�v�f���@ikChainLength
			// �O�̂��ߊu��
			uint16_t* ikChildBoneIndexArray = 0;
		public:
			// �ϒ��z����擾����
			const uint16_t& GetIkChildBoneID(const int i) const;



			// ���C�u�������g�p����֐�

			// �ϒ��z��̃��[�h�p�֐��A�R���X�g���N�^�@�f�X�g���N�^
			// ���[�U�[������Ăяo�����Ƃ͂Ȃ�
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

		//�\��f�[�^(pmd��p��pmx�ł́A���[�t�ƌĂ΂�Ă��邽�߁A������ɏ�������)
		const uint16_t& GetMorphCount() const;
		//Base���������\�(pmd�G�f�B�^��ŕ\������Ă���\�)
		const uint16_t GetMorphCountNotIncludeBase() const;
		struct Morph
		{
			TextBufferFixed<20> name = {};
			uint32_t offsCount = 0;

			MorphType type = MorphType::MT_NONE;

			// type�ŕϐ��̖������قȂ�
			// �\���͓��� //
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
			//�ϒ��z��@:�@�v�f���@vertexCount
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
			// �g���@�\(3)
			TextBufferFixed<20> nameEng = {};

			// base�Ƃ��Ē��_�f�[�^���擾
			const MorphBaseData& GetMorphBaseData(const int i) const;
			// �ʏ�̒��_�f�[�^�Ƃ��Ď擾
			const MorphOffsData& GetMorphOffsData(const int i) const;

			// ���C�u�������g�p����֐�
			// �ϒ��z��̃��[�h�p�֐�
			void LoadSkinVertex(void* _file);

			// �f�o�b�O�o�́@�����ɒ��߂̉ϒ��z��������߁A�\�����邩��I��
			void DebugOut(bool isOutVertexData = false) const;
			Morph(); ~Morph();
		private:
			Morph(const Morph&);
			const Morph& operator=(const Morph&) const;
		};
		//i = 0��base
		const Morph& GetMorph(const uint16_t i) const;
		const uint16_t GetLastMorphID() const;
		void DebugOutMorph(const uint16_t i, bool isOutVertexData = false) const;
		void DebugOutAllMorph(bool isOutVertexData = false) const;

		// ���[�t�p�\���g�f�[�^
		// ���[�t(�\��)�f�[�^��MMD�G�f�B�^��ɕ\������ۂ�
		// �g�p�����f�[�^���Ǝv����
		// pmd�G�f�B�^��ł́A
		// base���������C���f�b�N�X���\������Ă���A
		// �i�[����Ă���f�[�^��base���܂߂�
		// �C���f�b�N�X�ƂȂ��Ă��� //
		const uint8_t& GetMorphIndexForDisplayCount() const;
		// base���܂߂��C���f�b�N�X��Ԃ� //
		const uint16_t& GetMorphIndexForDisplay(const uint8_t i) const;
		const uint8_t GetLastMorphIndexForDisplayID() const;
		void DebugOutMorphIndexForDisplay(const uint8_t i) const;
		void DebugOutAllMorphIndexForDisplay() const;

		// �\���p�{�[�����f�[�^ //
		const uint8_t& GetBoneNameForDisplayCount() const;
		const TextBufferFixed<50>& GetBoneNameForDisplay(const uint8_t i) const;
		const TextBufferFixed<50>& GetBoneNameForDisplayEng(const uint8_t i) const;
		const uint8_t GetLastBoneNameForDisplayID() const;
		void DebugOutBoneNameForDisplay(const uint8_t i) const;
		void DebugOutAllBoneNameForDisplay() const;

		// �{�[���g�p�g���̓ǂݍ���
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

		// �ȉ��A�������Z�p�̃f�[�^
		// �׋����̂��߁A�s���m�ȃl�[�~���O�ɂȂ��Ă���\������
		// �������Z����������ۂɖ��O��ς���\��B //

		// ���̃f�[�^ 
		const uint32_t& GetRigitbodyCount() const;

		struct Rigitbody
		{
			TextBufferFixed<20> name = {};
			uint16_t relationshipBoneID = 0;
			// �O���[�v�̔ԍ��@�O���[�v1 = 0, �O���[�v16 = 15,
			uint8_t group = 0;
			// �ՓˑΏۂ̃I�u�W�F�N�g
			// �G�f�B�^��ł́u��ՓˑΏہv��I������`�F�b�N�{�b�N�X���\������Ă���
			// 16~5,4,3,2,1 �̏��ɕ���ł���A
			// �Ⴆ�΃G�f�B�^���6�Ԃ���Փ˂Ƃ��ă`�F�b�N����Ă���ꍇ��
			// 0x1111_1111_1101_1111 ���i�[�����//
			uint16_t groupTarget = 0;

			RigitbodyShapeType shapeType = RigitbodyShapeType::RST_NONE;

			float shapeW = 0;
			float shapeH = 0;
			float shapeD = 0;
			// �t�@�C�����̒l�́A�֘A�{�[���̍��W�ɑ΂���
			// �I�t�Z�b�g�l
			// pmd�G�f�B�^��̒l�̓{�[�����W��
			// �I�t�Z�b�g�����Z�������ۂ̍��W //
			float3 position = {};
			// �P�ʁ@���W�A��
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

			// �P��: ���W�A��
			float3 rotLowerLimit = {};
			float3 rotUpperLimit = {};

			float3 springPos = {};
			// �P�ʕs��
			// ��L�̉�]�����ƈقȂ�A�ϊ����Ȃ��Ă�pmd�G�f�B�^���
			// �����l�ƂȂ邽�߁A�x�A���邢�͔䗦�ȂǈقȂ�P�ʂ�
			// �p�����[�^���B�ڍוs�� //
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

		//�@�S�f�[�^��\������
		void DebugOutAllData() const;

	private:
		//����̃R���X�g���N�^�A�R�s�[�R���X�g���N�^�Ăяo���֎~
		//�K���t�@�C�������w�肵�Đ������邱��
		PmdFile(); PmdFile(const PmdFile& copy);
		const PmdFile& operator=(const PmdFile&) const;

		bool isSuccess;

		char* mDirectoryPath;

		//�@�e��f�[�^����
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
		// ���{��\���p�{�[�����͉��s����('\n' : 0x0a)�ŏI���@
		TextBufferFixed<50>* mBoneNameForDisplay;

		// �g���@�\(4)
		// �\���p�{�[�����p�� //
		TextBufferFixed<50>* mBoneNameForDisplayEng;

		uint32_t mBoneForDisplayCount;
		BoneForDisplay* mBoneForDisplay;

		TextBufferFixed<100> mToonTexturePath[10];

		uint32_t mRigitbodyCount;
		Rigitbody* mRigitbody;

		uint32_t mJointCount;
		Joint* mJoint;
	};

	// pmx�t�@�C����ǂݍ��݁A
	// �t�@�C���\���̂܂܌��J����N���X
	// ��{�I�ȃC���^�[�t�F�C�X��pmd�Ɠ��ꂷ��@//
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

		// �w�b�_���
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

		// ���_���
		const int32_t& GetVertexCount() const;
		struct Vertex
		{
			float3 position = {};
			float3 normal = {};
			float2 uv = {};
		private:
			float4 addtionalUV[4] = { {} };
		public:
			// BDEF1	:1�̃{�[���ɂ��ό`
			// BDEF2	:2�̃{�[���ɂ��ό`
			// BDEF4	:4�̃{�[���ɂ��ό`
			// SDEF		:�X�t�B���J���f�t�H�[��
			//			 �Ή����Ȃ��ꍇ��BDEF2�Ƃ��Ă����p��  //
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

			// �u�������z��̃A�N�Z�X�֐�
			const float4& GetAddtionalUV(const int32_t i) const;
			const int32_t& GetBoneID(const int32_t i) const;
			const float& GetWeight(const int32_t i) const;

			// ���C�u���������Ăяo���֐�
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


		// �C���f�b�N�X�f�[�^
		const int32_t& GetIndexCount() const;
		const int32_t& GetIndex(const int32_t i) const;
		const int32_t GetLastIndexID() const;
		void DebugOutIndexData(const int32_t i) const;
		void DebugOutAllIndexData() const;

		// �e�N�X�`���f�[�^
		const int32_t& GetTextureCount() const;
		const TextBufferVariable& GetTexturePath(const int32_t) const;
		const int32_t GetLastTextureID() const;
		void DebugOutTexturePath(const int32_t i) const;
		void DebugOutAllTexturePath() const;

		// �}�e���A���f�[�^
		const int32_t& GetMaterialCount() const;
		struct Material
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};

			float4 diffuse = {};
			float3 specular = {};
			float specularity = 0.f;
			float3 ambient = {};
			// �w�ʂ�`�悷�邩
			// �n�ʉe��`�悷�邩
			// �Z���t�V���h�E�}�b�v�ւ̕`����s����
			// �Z���t�ԓ���`�悷�邩
			// �G�b�W��`�悷�邩�@//
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
			// toon�e�N�X�`�������L�̂��̂��ǂ���
			// ���Ƀe�N�X�`���������Ȃ��ꍇ�́ATM_UNIQUE // 
			enum ToonMode : uint8_t
			{
				TM_UNIQUE = 0,
				TM_SHARED = 1,
				TM_NONE,
			} toonMode = TM_NONE;
			int32_t toonTextureID = 0;
			TextBufferVariable memo = {};
			int32_t vertexCount = 0;

			// �`��ݒ���擾����֐�
			// DrawConfig�̗񋓎q���w�肵�A�L���ł���΁@true���Ԃ�B
			bool GetDrawConfig(DrawConfig bitflag) const;

			// ���C�u���������g�p����֐�
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


		// �{�[���f�[�^
		const int32_t& GetBoneCount() const;
		struct Bone
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			float3 position = {};
			int32_t parentBoneID = 0;
			int32_t transformHierarchy = 0;
			/*
			�ꕔ���O��DxLib���Q�l�ɂ���
			0x0001  : �ڑ���(PMD�q�{�[���w��)�\�����@ -> 0:���W�I�t�Z�b�g�Ŏw�� 1:�{�[���Ŏw��
			0x0002  : ��]�\
			0x0004  : �ړ��\
			0x0008  : �\��
			0x0010  : �����
			0x0020  : IK
			0x0080  : ���[�J���t�^ | �t�^�Ώ� 0:���[�U�[�ό`�l�^IK�����N�^���d�t�^ 1:�e�̃��[�J���ό`��
			0x0100  : ��]�t�^
			0x0200  : �ړ��t�^
			0x0400  : ���Œ�
			0x0800  : ���[�J����
			0x1000  : ������ό`
			0x2000  : �O���e�ό`
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
				// �ڑ��悪���W�I�t�Z�b�g�̏ꍇ�@0x0001 == true
				float3 positionOffs = {};
				// �ڑ��悪�{�[���̏ꍇ 0x0001 == false
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
			// IK�����N�f�[�^�{�̂̉ϒ��z��
			IK_Link* ikLink = nullptr;//[ikLinkCount]
		public:

			// �{�[���̐ݒ���擾����
			// BoneConfig�̗񋓎q���w�肷��ƁA
			// ���̍��ڂ��L�����ǂ������Ԃ�
			// �L���@:true�@�����@:false //
			bool GetBoneConfig(BoneConfig bitFlag) const;
			// IK�����N�f�[�^���擾����֐�
			const IK_Link& GetIK_Link(const int32_t i) const;



			// ���C�u���������g�p����֐�
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

		// ���[�t�f�[�^
		const int32_t& GetMorphCount() const;
		struct Morph
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			// pmd �Ƌ���
			MorphType type = MorphType::MT_NONE;
			// pmx �Œǉ�
			// �O���[�v���[�t�@:2�ȏ�̃��[�t�𓯎��s�����[�t
			// ���_���[�t�@:���_�ʒu��ύX���郂�[�tpmd�̃��[�t�͂��ׂĂ���
			// �{�[�����[�t�@:�{�[���ʒu���ύX����邱�Ƃɂ�郂�[�t
			// UV���[�t�@:�e�N�X�`���̐؂�o���ʒu��ύX���郂�[�t
			// �}�e���A�����[�t�@:�}�e���A����ω������郂�[�t //
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

			// ���[�t�̃I�t�Z�b�g�f�[�^�̐�
			int32_t offsCount = 0;

			// �O���[�v���[�t�̃I�t�Z�b�g�f�[�^
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

			// ���_���[�t�̃I�t�Z�b�g�f�[�^
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

			// �{�[�����[�t�̃I�t�Z�b�g�f�[�^
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

			// UV���[�t�̃I�t�Z�b�g�f�[�^
			struct UV_Offs
			{
				int32_t vertexID = -1;
				// �ʏ��UV�̏ꍇ�Az, w�����͎g�p���Ȃ����A�f�[�^�㑶��
				float4 uv = {};
				void DebugOut() const;

				UV_Offs(); ~UV_Offs();
			private:
				UV_Offs(const UV_Offs&);
				const UV_Offs& operator=(const UV_Offs) const;
			};

			// �}�e���A�����[�t�̃I�t�Z�b�g�f�[�^
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

			// �I�t�Z�b�g�f�[�^���i�[���鋤�p��
			// 1�̃��[�t�f�[�^�ɕ�����̃I�t�Z�b�g���i�[����邱�Ƃ͂Ȃ� //
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
			// �I�t�Z�b�g�f�[�^�{�́@�ϒ��z��
			MorphOffsData* morphOffsData = nullptr;//[offsCount]
		public:

			// �I�t�Z�b�g�f�[�^���擾����֐��@�o�͐�ŁA�g�p����f�[�^��I������
			const MorphOffsData& GetMorphOffsData(const int32_t i) const;


			// ���C�u���������g�p����֐�
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


		// �\���g�f�[�^
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

			// ���C�u�������g�p����֐�
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

		// ���̃f�[�^
		const int32_t& GetRigitbodyCount() const;
		struct Rigitbody
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};
			int32_t relationshipBoneID = -1;
			uint8_t group = 0;
			// �ՓˑΏۂ̃I�u�W�F�N�g
			// �G�f�B�^��ł́u��ՓˑΏہv��I������`�F�b�N�{�b�N�X���\������Ă���
			// 16~5,4,3,2,1 �̏��ɕ���ł���A
			// �Ⴆ�΃G�f�B�^���6�Ԃ���Փ˂Ƃ��ă`�F�b�N����Ă���ꍇ��
			// 0x1111_1111_1101_1111 ���i�[�����//
			uint16_t groupTarget = 0;

			RigitbodyShapeType shapeType = RigitbodyShapeType::RST_NONE;

			float shapeW = 0.f;
			float shapeH = 0.f;
			float shapeD = 0.f;
			// pmd�ƈقȂ�Apmx�G�f�B�^��̒l�Ɠ����l���i�[����Ă���
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

		// �W���C���g�f�[�^
		const int32_t& GetJointCount() const;
		struct Joint
		{
			TextBufferVariable name = {};
			TextBufferVariable nameEng = {};

			enum JointType : uint8_t
			{
				// PMX2.0�ł�1�̂�
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
		// �f�t�H���g�A�R�s�[�֎~
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

	// ���[�V�����t�@�C��(������)
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

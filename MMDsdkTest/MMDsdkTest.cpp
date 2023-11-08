
#include "pch.h"
#include "CppUnitTest.h"

#include"MMDsdk.h"
#include"MathUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace MMDsdk;
using namespace MathUtil;

namespace MMDsdkTest
{
	bool strCmpFortest(const void* _str1, const void* _str2, const int length)
	{
		bool result = false;
		auto str1 = reinterpret_cast<const char*>(_str1);
		auto str2 = reinterpret_cast<const char*>(_str2);
		for (auto i = 0; i < length; ++i)
		{
			if (str2[i] == '\0')
			{
				break;
			}
			result = str1[i] == str2[i];
		}
		return result;
	}

	TEST_CLASS(PmdReadTest)
	{

		const char* testPmdModelPath = "Test/Model/PMD/�����~�NVer2.pmd";



		TEST_METHOD(ReadPmd)
		{
			PmdFile pmd(testPmdModelPath);

			// ��{���̓ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetHeader().version == 1.f);


			// �ȉ��f�[�^�ǂݍ��݃e�X�g
			// �ŏ��ƍŌ�̃f�[�^�̂݊m�F
			// ���_�ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetVertexCount() == 12354);

			auto& v0 = pmd.GetVertex(0);
			Assert::IsTrue(FloatEqual(v0.position.x, -6.9072f));
			Assert::IsTrue(FloatEqual(v0.position.y, 11.4279f));
			Assert::IsTrue(FloatEqual(v0.position.z, 0.4254f));
			Assert::IsTrue(FloatEqual(v0.normal.x, -0.7672437f));
			Assert::IsTrue(FloatEqual(v0.normal.y, -0.3205231f));
			Assert::IsTrue(FloatEqual(v0.normal.z, 0.5555196f));
			Assert::IsTrue(FloatEqual(v0.uv.x, 0.15464f));
			Assert::IsTrue(FloatEqual(v0.uv.y, -0.071395f));
			Assert::IsTrue(v0.GetBoneID(0) == 66);
			Assert::IsTrue(v0.GetBoneID(1) == 65);
			Assert::IsTrue(v0.weight == 100);
			Assert::IsTrue(v0.edgeFlag == PmdFile::Vertex::VEE_ENABLE);

			auto& vertexL = pmd.GetVertex(pmd.GetLastVertexID());
			Assert::IsTrue(FloatEqual(vertexL.position.x, 0.3274f));
			Assert::IsTrue(FloatEqual(vertexL.position.y, 17.8753f));
			Assert::IsTrue(FloatEqual(vertexL.position.z, -1.0512f));
			Assert::IsTrue(FloatEqual(vertexL.normal.x, 0.6153633f));
			Assert::IsTrue(FloatEqual(vertexL.normal.y, -0.2885867f));
			Assert::IsTrue(FloatEqual(vertexL.normal.z, -0.733516f));
			Assert::IsTrue(FloatEqual(vertexL.uv.x, 0.516369f));
			Assert::IsTrue(FloatEqual(vertexL.uv.y, -0.393763f));
			Assert::IsTrue(vertexL.GetBoneID(0) == 3);
			Assert::IsTrue(vertexL.GetBoneID(1) == 3);
			Assert::IsTrue(vertexL.weight == 100);
			Assert::IsTrue(vertexL.edgeFlag == PmdFile::Vertex::VEE_ENABLE);

			//�C���f�b�N�X�ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetIndexCount() == 22961 * 3);
			Assert::IsTrue(pmd.GetIndex(0) == 647);
			Assert::IsTrue(pmd.GetIndex(pmd.GetLastIndexID()) == 11286);

			//�}�e���A���ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetMaterialCount() == 17);

			auto& m0 = pmd.GetMaterial(0);
			Assert::IsTrue(FloatEqual(m0.diffuse.x, 0.133f));
			Assert::IsTrue(FloatEqual(m0.diffuse.y, 0.616f));
			Assert::IsTrue(FloatEqual(m0.diffuse.z, 0.71f));
			Assert::IsTrue(FloatEqual(m0.diffuse.w, 1.f));
			Assert::IsTrue(FloatEqual(m0.specularity, 5.f));
			Assert::IsTrue(FloatEqual(m0.specular.x, 0.f));
			Assert::IsTrue(FloatEqual(m0.specular.y, 0.f));
			Assert::IsTrue(FloatEqual(m0.specular.z, 0.f));
			Assert::IsTrue(FloatEqual(m0.ambient.x, 0.0665f));
			Assert::IsTrue(FloatEqual(m0.ambient.y, 0.308f));
			Assert::IsTrue(FloatEqual(m0.ambient.z, 0.355f));
			Assert::IsTrue(m0.toonIndex == 0);
			Assert::IsTrue(m0.edgeFlag == PmdFile::Material::MEE_ENABLE);
			Assert::IsTrue(m0.vertexCount == 7275);
			Assert::IsTrue(m0.texturePath.GetFirstChar() == '\0');

			//texturePath �����}�e���A��
			auto& mWithTex = pmd.GetMaterial(6);
			Assert::IsTrue(FloatEqual(mWithTex.diffuse.x, 0.654f));
			Assert::IsTrue(FloatEqual(mWithTex.diffuse.y, 0.654f));
			Assert::IsTrue(FloatEqual(mWithTex.diffuse.z, 0.654f));
			Assert::IsTrue(FloatEqual(mWithTex.diffuse.w, 1.f));
			Assert::IsTrue(FloatEqual(mWithTex.specularity, 5.f));
			Assert::IsTrue(FloatEqual(mWithTex.specular.x, 0.f));
			Assert::IsTrue(FloatEqual(mWithTex.specular.y, 0.f));
			Assert::IsTrue(FloatEqual(mWithTex.specular.z, 0.f));
			Assert::IsTrue(FloatEqual(mWithTex.ambient.x, 0.4f));
			Assert::IsTrue(FloatEqual(mWithTex.ambient.y, 0.4f));
			Assert::IsTrue(FloatEqual(mWithTex.ambient.z, 0.4f));
			Assert::IsTrue(mWithTex.toonIndex == 0);
			Assert::IsTrue(mWithTex.edgeFlag == PmdFile::Material::MEE_DISABLE);
			Assert::IsTrue(mWithTex.vertexCount == 432);
			Assert::IsTrue(strCmpFortest(GetText(mWithTex.texturePath), "eyeM2.bmp", mWithTex.texturePath.GetLength()));

			//�{�[���ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetBoneCount() == 140);

			auto& b0 = pmd.GetBone(0);
			Assert::IsTrue(strCmpFortest(GetText(b0.name), "�Z���^�[", b0.name.GetLength()));
			Assert::IsTrue(strCmpFortest(GetText(b0.nameEng), "center", b0.nameEng.GetLength()));
			Assert::IsTrue(b0.parentIndex == 65535);
			Assert::IsTrue(b0.childIndex == 116);
			Assert::IsTrue(b0.type == PmdFile::Bone::BT_ROTATE_AND_TRANSRATE);
			Assert::IsTrue(b0.ikParentIndex == 0);
			Assert::IsTrue(FloatEqual(b0.headPos.x, 0.f));
			Assert::IsTrue(FloatEqual(b0.headPos.y, 8.f));
			Assert::IsTrue(FloatEqual(b0.headPos.z, 0.f));

			auto& bL = pmd.GetBone(pmd.GetLastBoneID());
			Assert::IsTrue(strCmpFortest(GetText(bL.name), "�E�r��3", bL.name.GetLength()));
			Assert::IsTrue(strCmpFortest(GetText(bL.nameEng), "�E�r��3", bL.nameEng.GetLength()));
			Assert::IsTrue(bL.parentIndex == 50);
			Assert::IsTrue(bL.childIndex == 51);
			Assert::IsTrue(bL.type == PmdFile::Bone::BT_ROTATION_MOVE);
			Assert::IsTrue(bL.ikParentIndex == 75);
			Assert::IsTrue(FloatEqual(bL.headPos.x, -1.844493f));
			Assert::IsTrue(FloatEqual(bL.headPos.y, 15.36798f));
			Assert::IsTrue(FloatEqual(bL.headPos.z, 0.304996f));

			//IK�f�[�^�ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetIKCount() == 7);
			auto& ik0 = pmd.GetIKData(0);
			Assert::IsTrue(ik0.ikBoneIndex == 86);
			Assert::IsTrue(ik0.ikTargetBoneIndex == 81);
			Assert::IsTrue(ik0.ikChainCount == 3);
			Assert::IsTrue(ik0.iterations == 15);
			Assert::IsTrue(FloatEqual(ik0.controlWeight, 0.03f));
			Assert::IsTrue(ik0.GetIkChildBoneID(0) == 8);
			Assert::IsTrue(ik0.GetIkChildBoneID(1) == 7);
			Assert::IsTrue(ik0.GetIkChildBoneID(2) == 6);

			auto& ikL = pmd.GetIKData(pmd.GetLastIKDataID());
			Assert::IsTrue(ikL.ikBoneIndex == 92);
			Assert::IsTrue(ikL.ikTargetBoneIndex == 85);
			Assert::IsTrue(ikL.ikChainCount == 1);
			Assert::IsTrue(ikL.iterations == 3);
			Assert::IsTrue(FloatEqual(ikL.controlWeight, 1.f));
			Assert::IsTrue(ikL.GetIkChildBoneID(0) == 74);

			//�@�\��f�[�^�ǂݍ��݃e�X�g
			Assert::IsTrue(pmd.GetMorphCount() == 31);
			Assert::IsTrue(pmd.GetMorphCountNotIncludeBase() == 30);

			auto& base = pmd.GetMorph(0);

			auto& m1 = pmd.GetMorph(1);
			Assert::IsTrue(strCmpFortest(GetText(m1.name), "�^�ʖ�", m1.name.GetLength()));
			Assert::IsTrue(strCmpFortest(GetText(m1.nameEng), "serious", m1.nameEng.GetLength()));
			Assert::IsTrue(m1.offsCount == 78);
			Assert::IsTrue(m1.type == PmdFile::Morph::MT_EYEBROW);

			Assert::IsTrue(base.GetMorphBaseData(m1.GetMorphOffsData(0).baseIndex).index == 11245);
			Assert::IsTrue(m1.GetMorphOffsData(0).offsPosition.x == -0.0123f);
			Assert::IsTrue(m1.GetMorphOffsData(0).offsPosition.y == -0.046101f);
			Assert::IsTrue(m1.GetMorphOffsData(0).offsPosition.z == 0.f);

			auto lastId = m1.offsCount - 1;
			Assert::IsTrue(base.GetMorphBaseData(m1.GetMorphOffsData(lastId).baseIndex).index == 11322);
			Assert::IsTrue(m1.GetMorphOffsData(lastId).offsPosition.x == -0.0199f);
			Assert::IsTrue(m1.GetMorphOffsData(lastId).offsPosition.y == -0.097601f);
			Assert::IsTrue(m1.GetMorphOffsData(lastId).offsPosition.z == 0.0435f);



			auto& mL = pmd.GetMorph(pmd.GetLastMorphID());
			Assert::IsTrue(strCmpFortest(GetText(mL.name), "�ɂ��", mL.name.GetLength()));
			Assert::IsTrue(strCmpFortest(GetText(mL.nameEng), "grin", mL.nameEng.GetLength()));
			Assert::IsTrue(mL.offsCount == 45);
			Assert::IsTrue(mL.type == PmdFile::Morph::MT_LIP);

			Assert::IsTrue(base.GetMorphBaseData(mL.GetMorphOffsData(0).baseIndex).index == 12000);
			Assert::IsTrue(mL.GetMorphOffsData(0).offsPosition.x == 0.0443f);
			Assert::IsTrue(mL.GetMorphOffsData(0).offsPosition.y == 0.014502f);
			Assert::IsTrue(mL.GetMorphOffsData(0).offsPosition.z == 0.0105f);

			lastId = mL.offsCount - 1;
			Assert::IsTrue(base.GetMorphBaseData(mL.GetMorphOffsData(lastId).baseIndex).index == 12320);
			Assert::IsTrue(mL.GetMorphOffsData(lastId).offsPosition.x == -0.0132f);
			Assert::IsTrue(mL.GetMorphOffsData(lastId).offsPosition.y == 0.0305f);
			Assert::IsTrue(mL.GetMorphOffsData(lastId).offsPosition.z == -0.0021f);

			//���[�t(�\��)�@�\���g�f�[�^
			Assert::IsTrue(pmd.GetMorphIndexForDisplayCount() == 30);
			Assert::IsTrue(pmd.GetMorphIndexForDisplay(0) == 19);
			Assert::IsTrue(pmd.GetMorphIndexForDisplay(pmd.GetLastMorphIndexForDisplayID()) == 28);

			// �\���p�{�[�����f�[�^
			Assert::IsTrue(pmd.GetBoneNameForDisplayCount() == 7);

			const char* boneName0 = "�h�j";
			auto& boneName0FromFile = pmd.GetBoneNameForDisplay(0);
			Assert::IsTrue(strCmpFortest(GetText(boneName0FromFile), boneName0, boneName0FromFile.GetLength()));

			const char* boneName0Eng = "IK";
			auto& boneName0FromFileEng = pmd.GetBoneNameForDisplayEng(0);
			Assert::IsTrue(strCmpFortest(GetText(boneName0FromFileEng), boneName0Eng, boneName0FromFileEng.GetLength()));

			const char* boneNameL = "��";
			auto& boneNameLFromFile = pmd.GetBoneNameForDisplay(pmd.GetLastBoneNameForDisplayID());
			Assert::IsTrue(strCmpFortest(GetText(boneNameLFromFile), boneNameL, boneNameLFromFile.GetLength()));

			const char* boneNameLEng = "Legs";
			auto& boneNameLFromFileEng = pmd.GetBoneNameForDisplayEng(pmd.GetLastBoneNameForDisplayID());
			Assert::IsTrue(strCmpFortest(GetText(boneNameLFromFileEng), boneNameLEng, boneNameLFromFileEng.GetLength()));

			// �\���p�{�[���f�[�^
			Assert::IsTrue(pmd.GetBoneForDisplayCount() == 87);
			Assert::IsTrue(pmd.GetBoneForDisplay(0).boneIndexForDisplay == 86);
			Assert::IsTrue(pmd.GetBoneForDisplay(0).boneDisplayFrameIndex == 1);

			Assert::IsTrue(pmd.GetBoneForDisplay(pmd.GetLastBoneForDisplayID()).boneIndexForDisplay == 74);
			Assert::IsTrue(pmd.GetBoneForDisplay(pmd.GetLastBoneForDisplayID()).boneDisplayFrameIndex == 7);

			// �������Z�p���̃f�[�^
			Assert::IsTrue(pmd.GetRigitbodyCount() == 45);

			auto& body0 = pmd.GetRigitbody(0);
			Assert::IsTrue(strCmpFortest(GetText(body0.name), "��", body0.name.GetLength()));
			Assert::IsTrue(body0.relationshipBoneID == 3);
			Assert::IsTrue(body0.group == 0);
			Assert::IsTrue(body0.groupTarget == 0b1111111111111111);
			Assert::IsTrue(body0.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_SPHERE);
			Assert::IsTrue(FloatEqual(body0.shapeW, 1.6f));
			auto& relBone0 = pmd.GetBone(body0.relationshipBoneID);
			Assert::IsTrue(FloatEqual(body0.position.x + relBone0.headPos.x, 0.1f));
			Assert::IsTrue(FloatEqual(body0.position.y + relBone0.headPos.y, 18.3985f));
			Assert::IsTrue(FloatEqual(body0.position.z + relBone0.headPos.z, 0.2f));
			Assert::IsTrue(FloatEqual(RadianToDegree(body0.rotation.x), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(body0.rotation.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(body0.rotation.z), 0.00f));
			Assert::IsTrue(FloatEqual(body0.weight, 1.f));
			Assert::IsTrue(FloatEqual(body0.positionDim, 0.f));
			Assert::IsTrue(FloatEqual(body0.rotationDim, 0.f));
			Assert::IsTrue(FloatEqual(body0.recoil, 0.f));
			Assert::IsTrue(FloatEqual(body0.friction, 0.5f));
			Assert::IsTrue(body0.type == PmdFile::Rigitbody::RigitBodyType::RT_BONE_FOLLOW);

			auto& bodyL = pmd.GetRigitbody(pmd.GetLastRigitbodyID());
			Assert::IsTrue(strCmpFortest(GetText(bodyL.name), "�l�N�^�C3", bodyL.name.GetLength()));
			Assert::IsTrue(bodyL.relationshipBoneID == 8);
			Assert::IsTrue(bodyL.group == 05);
			Assert::IsTrue(bodyL.groupTarget == 0b1111111111011111);
			Assert::IsTrue(bodyL.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_BOX);
			Assert::IsTrue(FloatEqual(bodyL.shapeW, 0.3f));
			Assert::IsTrue(FloatEqual(bodyL.shapeH, 0.9f));
			Assert::IsTrue(FloatEqual(bodyL.shapeD, 0.2f));
			auto& relBoneL = pmd.GetBone(bodyL.relationshipBoneID);
			Assert::IsTrue(FloatEqual(bodyL.position.x + relBoneL.headPos.x, 0.f));
			Assert::IsTrue(FloatEqual(bodyL.position.y + relBoneL.headPos.y, 11.97617f));
			Assert::IsTrue(FloatEqual(bodyL.position.z + relBoneL.headPos.z, -1.37111f));
			Assert::IsTrue(FloatEqual(RadianToDegree(bodyL.rotation.x), 4.6f));
			Assert::IsTrue(FloatEqual(RadianToDegree(bodyL.rotation.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(bodyL.rotation.z), 0.00f));
			Assert::IsTrue(FloatEqual(bodyL.weight, 0.5f));
			Assert::IsTrue(FloatEqual(bodyL.positionDim, 0.9f));
			Assert::IsTrue(FloatEqual(bodyL.rotationDim, 2.f));
			Assert::IsTrue(FloatEqual(bodyL.recoil, 0.f));
			Assert::IsTrue(FloatEqual(bodyL.friction, 0.0f));
			Assert::IsTrue(bodyL.type == PmdFile::Rigitbody::RigitBodyType::RT_RIGITBODY_WITH_BONE_OFFS);

			// �������Z�W���C���g�f�[�^
			Assert::IsTrue(pmd.GetJointCount() == 27);

			auto& joint0 = pmd.GetJoint(0);
			Assert::IsTrue(strCmpFortest(GetText(joint0.name), "�E��1", joint0.name.GetLength()));
			Assert::IsTrue(joint0.rigitbodyIndexA == 0);
			Assert::IsTrue(joint0.rigitbodyIndexB == 1);
			Assert::IsTrue(FloatEqual(joint0.position.x, -1.6482f));
			Assert::IsTrue(FloatEqual(joint0.position.y, 19.6392f));
			Assert::IsTrue(FloatEqual(joint0.position.z, 1.4544f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotation.x), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotation.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotation.z), 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posLowerLimit.x, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posLowerLimit.y, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posLowerLimit.z, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posUpperLimit.x, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posUpperLimit.y, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.posUpperLimit.z, 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotLowerLimit.x), 10.f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotLowerLimit.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotLowerLimit.z), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotUpperLimit.x), 10.f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotUpperLimit.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(joint0.rotUpperLimit.z), 0.00f));
			Assert::IsTrue(FloatEqual(joint0.springPos.x, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.springPos.y, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.springPos.z, 0.00f));
			Assert::IsTrue(FloatEqual(joint0.springRot.x, 100.f));
			Assert::IsTrue(FloatEqual(joint0.springRot.y, 100.f));
			Assert::IsTrue(FloatEqual(joint0.springRot.z, 100.f));

			auto& jointL = pmd.GetJoint(pmd.GetLastJointID());
			Assert::IsTrue(strCmpFortest(GetText(jointL.name), "���X�J�[�g�O2", jointL.name.GetLength()));
			Assert::IsTrue(jointL.rigitbodyIndexA == 35);
			Assert::IsTrue(jointL.rigitbodyIndexB == 37);
			Assert::IsTrue(FloatEqual(jointL.position.x, 1.1f));
			Assert::IsTrue(FloatEqual(jointL.position.y, 11.f));
			Assert::IsTrue(FloatEqual(jointL.position.z, -1.3f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotation.x), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotation.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotation.z), 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posLowerLimit.x, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posLowerLimit.y, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posLowerLimit.z, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posUpperLimit.x, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posUpperLimit.y, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.posUpperLimit.z, 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotLowerLimit.x), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotLowerLimit.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotLowerLimit.z), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotUpperLimit.x), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotUpperLimit.y), 0.00f));
			Assert::IsTrue(FloatEqual(RadianToDegree(jointL.rotUpperLimit.z), 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springPos.x, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springPos.y, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springPos.z, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springRot.x, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springRot.y, 0.00f));
			Assert::IsTrue(FloatEqual(jointL.springRot.z, 0.00f));
		}
	};
	TEST_CLASS(PmxReadTest)
	{
	public:
		const char* testPmxModelPath = "Test/Model/PMX/���΂񂿂��/���΂񂿂��/���΂񂿂��.pmx";
		const char* testPmxModelPath2 = "Test/Model/PMX/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W����/Appearance Miku_��l�o�[�W���� ver.2.3.1.pmx";


		TEST_METHOD(ReadPmx)
		{
			PmxFile pmx(testPmxModelPath);

			{
				auto& h = pmx.GetHeader();

				Assert::IsTrue(FloatEqual(h.version, 2.f));
				Assert::IsTrue(h.fileConfigLength == 8);
				Assert::IsTrue(h.encode == UTF16);
				Assert::IsTrue(h.additionalUVcount == 0);
				Assert::IsTrue(h.vertexID_Size == 2);
				Assert::IsTrue(h.textureID_Size == 1);
				Assert::IsTrue(h.materialID_Size == 1);
				Assert::IsTrue(h.boneID_Size == 2);
				Assert::IsTrue(h.morphID_Size == 2);
				Assert::IsTrue(h.rigitbodyID_Size == 1);

				Assert::IsTrue(strCmpFortest(GetText(h.modelInfoJp.modelName), "���΂񂿂��", h.modelInfoJp.modelName.GetLength()));
			}

			// ���_�f�[�^�ǂݍ��݃e�X�g
			Assert::IsTrue(pmx.GetVertexCount() == 23263);
			Assert::IsTrue(pmx.GetLastVertexID() == pmx.GetVertexCount() - 1);

			{
				auto& v0 = pmx.GetVertex(0);
				Assert::IsTrue(FloatEqual(v0.position.x, 0.4900185f));
				Assert::IsTrue(FloatEqual(v0.position.y, 17.81798f));
				Assert::IsTrue(FloatEqual(v0.position.z, 0.3476278f));
				Assert::IsTrue(FloatEqual(v0.normal.x, 0.2145061f));
				Assert::IsTrue(FloatEqual(v0.normal.y, 0.9458833f));
				Assert::IsTrue(FloatEqual(v0.normal.z, 0.2434991f));
				Assert::IsTrue(FloatEqual(v0.uv.x, 0.244026f));
				Assert::IsTrue(FloatEqual(v0.uv.y, 0.24849f));
				for (int i = 0; pmx.GetHeader().additionalUVcount; ++i)
				{
					auto& addUV = v0.GetAddtionalUV(i);
					Assert::IsTrue(FloatEqual(addUV.x, 0.f));
					Assert::IsTrue(FloatEqual(addUV.y, 0.f));
					Assert::IsTrue(FloatEqual(addUV.z, 0.f));
					Assert::IsTrue(FloatEqual(addUV.w, 0.f));
				}
				Assert::IsTrue(v0.weightType == PmxFile::Vertex::BDEF1);
				Assert::IsTrue(v0.GetBoneID(0) == 7);
				Assert::IsTrue(v0.GetBoneID(1) == 0);
				Assert::IsTrue(v0.GetBoneID(2) == 0);
				Assert::IsTrue(v0.GetBoneID(3) == 0);
				Assert::IsTrue(FloatEqual(v0.GetWeight(0), 1.f));
				Assert::IsTrue(FloatEqual(v0.GetWeight(1), 0.f));
				Assert::IsTrue(FloatEqual(v0.GetWeight(2), 0.f));
				Assert::IsTrue(FloatEqual(v0.GetWeight(3), 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefC.x, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefC.y, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefC.z, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR0.x, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR0.y, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR0.z, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR1.x, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR1.y, 0.f));
				Assert::IsTrue(FloatEqual(v0.sdefR1.z, 0.f));
				Assert::IsTrue(v0.edgeRate == 1.f);
			}
			{
				auto& vBdef2 = pmx.GetVertex(1014);
				Assert::IsTrue(FloatEqual(vBdef2.position.x, 2.513021f));
				Assert::IsTrue(FloatEqual(vBdef2.position.y, 18.24938f));
				Assert::IsTrue(FloatEqual(vBdef2.position.z, 0.8821488f));
				Assert::IsTrue(FloatEqual(vBdef2.normal.x, 0.9501336f));
				Assert::IsTrue(FloatEqual(vBdef2.normal.y, -0.06675097f));
				Assert::IsTrue(FloatEqual(vBdef2.normal.z, 0.3046149f));
				Assert::IsTrue(FloatEqual(vBdef2.uv.x, 0.985731f));
				Assert::IsTrue(FloatEqual(vBdef2.uv.y, 0.82693f));
				for (int i = 0; pmx.GetHeader().additionalUVcount; ++i)
				{
					auto& addUV = vBdef2.GetAddtionalUV(i);
					Assert::IsTrue(FloatEqual(addUV.x, 0.f));
					Assert::IsTrue(FloatEqual(addUV.y, 0.f));
					Assert::IsTrue(FloatEqual(addUV.z, 0.f));
					Assert::IsTrue(FloatEqual(addUV.w, 0.f));
				}
				Assert::IsTrue(vBdef2.weightType == PmxFile::Vertex::BDEF2);
				Assert::IsTrue(vBdef2.GetBoneID(0) == 12);
				Assert::IsTrue(vBdef2.GetBoneID(1) == 11);
				Assert::IsTrue(vBdef2.GetBoneID(2) == 0);
				Assert::IsTrue(vBdef2.GetBoneID(3) == 0);
				Assert::IsTrue(FloatEqual(vBdef2.GetWeight(0), 0.9f));
				Assert::IsTrue(FloatEqual(vBdef2.GetWeight(1), 0.1f));
				Assert::IsTrue(FloatEqual(vBdef2.GetWeight(2), 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.GetWeight(3), 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefC.x, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefC.y, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefC.z, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR0.x, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR0.y, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR0.z, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR1.x, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR1.y, 0.f));
				Assert::IsTrue(FloatEqual(vBdef2.sdefR1.z, 0.f));

				Assert::IsTrue(vBdef2.edgeRate == 1.f);
			}
			{
				auto& vSdef = pmx.GetVertex(11367);
				Assert::IsTrue(FloatEqual(vSdef.position.x, -6.280599f));
				Assert::IsTrue(FloatEqual(vSdef.position.y, 9.3424f));
				Assert::IsTrue(FloatEqual(vSdef.position.z, -0.422f));
				Assert::IsTrue(FloatEqual(vSdef.normal.x, -0.4366429f));
				Assert::IsTrue(FloatEqual(vSdef.normal.y, 0.5716339f));
				Assert::IsTrue(FloatEqual(vSdef.normal.z, -0.6946779f));
				Assert::IsTrue(FloatEqual(vSdef.uv.x, 0.072791f));
				Assert::IsTrue(FloatEqual(vSdef.uv.y, 0.55609f));
				for (int i = 0; pmx.GetHeader().additionalUVcount; ++i)
				{
					auto& addUV = vSdef.GetAddtionalUV(i);
					Assert::IsTrue(FloatEqual(addUV.x, 0.f));
					Assert::IsTrue(FloatEqual(addUV.y, 0.f));
					Assert::IsTrue(FloatEqual(addUV.z, 0.f));
					Assert::IsTrue(FloatEqual(addUV.w, 0.f));
				}
				Assert::IsTrue(vSdef.weightType == PmxFile::Vertex::SDEF);
				Assert::IsTrue(vSdef.GetBoneID(0) == 119);
				Assert::IsTrue(vSdef.GetBoneID(1) == 120);
				Assert::IsTrue(vSdef.GetBoneID(2) == 0);
				Assert::IsTrue(vSdef.GetBoneID(3) == 0);
				Assert::IsTrue(FloatEqual(vSdef.GetWeight(0), 0.25f));
				Assert::IsTrue(FloatEqual(vSdef.GetWeight(1), 0.75f));
				Assert::IsTrue(FloatEqual(vSdef.GetWeight(2), 0.f));
				Assert::IsTrue(FloatEqual(vSdef.GetWeight(3), 0.f));
				Assert::IsTrue(FloatEqual(vSdef.sdefC.x, -6.255687f));
				Assert::IsTrue(FloatEqual(vSdef.sdefC.y, 9.30122f));
				Assert::IsTrue(FloatEqual(vSdef.sdefC.z, -0.3562029f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR0.x, -6.192173f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR0.y, 9.343068f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR0.z, -0.3540593f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR1.x, -6.255687f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR1.y, 9.30122f));
				Assert::IsTrue(FloatEqual(vSdef.sdefR1.z, -0.3562029f));
				Assert::IsTrue(vSdef.edgeRate == 1.f);
			}


			// �C���f�b�N�X�ǂݍ��݃e�X�g
			Assert::IsTrue(pmx.GetIndexCount() == 38148 * 3);

			Assert::IsTrue(pmx.GetLastIndexID() == pmx.GetIndexCount() - 1);

			Assert::IsTrue(pmx.GetIndex(0) == 9432);
			Assert::IsTrue(pmx.GetIndex(pmx.GetLastIndexID()) == 22318);

			// �e�N�X�`���ǂݍ��݃e�X�g
			Assert::IsTrue(pmx.GetTextureCount() == 6);
			Assert::IsTrue(strCmpFortest(GetText(pmx.GetTexturePath(0)), "hair.png", pmx.GetTexturePath(0).GetLength()));


			Assert::IsTrue(strCmpFortest(GetText(pmx.GetTexturePath(pmx.GetLastTextureID())), "k�g��.png", pmx.GetTexturePath(pmx.GetLastTextureID()).GetLength()));

			// �}�e���A���ǂݍ���
			Assert::IsTrue(pmx.GetMaterialCount() == 42);
			Assert::IsTrue(pmx.GetLastMaterialID() == pmx.GetMaterialCount() - 1);
			{
				auto& m0 = pmx.GetMaterial(0);
				Assert::IsTrue(strCmpFortest(GetText(m0.name), "�㔯", m0.name.GetLength()));
				Assert::IsTrue(m0.nameEng.GetLength() == 0);
				Assert::IsTrue(FloatEqual(m0.diffuse.x, 1.f));
				Assert::IsTrue(FloatEqual(m0.diffuse.y, 1.f));
				Assert::IsTrue(FloatEqual(m0.diffuse.z, 1.f));
				Assert::IsTrue(FloatEqual(m0.diffuse.w, 1.f));
				Assert::IsTrue(FloatEqual(m0.specular.x, 0.f));
				Assert::IsTrue(FloatEqual(m0.specular.y, 0.f));
				Assert::IsTrue(FloatEqual(m0.specular.z, 0.f));
				Assert::IsTrue(FloatEqual(m0.specularity, 0.f));
				Assert::IsTrue(FloatEqual(m0.ambient.x, 0.5f));
				Assert::IsTrue(FloatEqual(m0.ambient.y, 0.5f));
				Assert::IsTrue(FloatEqual(m0.ambient.z, 0.5f));
				Assert::IsTrue(m0.GetDrawConfig(PmxFile::Material::DC_NOT_CULLING) == true);
				Assert::IsTrue(m0.GetDrawConfig(PmxFile::Material::DC_DRAW_SHADOW) == false);
				Assert::IsTrue(m0.GetDrawConfig(PmxFile::Material::DC_DRAW_TO_SELF_SHADOW_MAP) == true);
				Assert::IsTrue(m0.GetDrawConfig(PmxFile::Material::DC_DRAW_SELF_SHADOW) == true);
				Assert::IsTrue(m0.GetDrawConfig(PmxFile::Material::DC_DRAW_EDGE) == true);
				Assert::IsTrue(FloatEqual(m0.edgeColor.x, 0.4039216f));
				Assert::IsTrue(FloatEqual(m0.edgeColor.y, 0.2901961f));
				Assert::IsTrue(FloatEqual(m0.edgeColor.z, 0.2352941f));
				Assert::IsTrue(FloatEqual(m0.edgeColor.w, 1.f));
				Assert::IsTrue(FloatEqual(m0.edgeSize, 1.f));
				Assert::IsTrue(m0.textureID == 0);
				Assert::IsTrue(m0.sphereTextureID == -1);
				Assert::IsTrue(m0.sphereMode == PmxFile::Material::SM_DISABLE);
				Assert::IsTrue(m0.toonMode == PmxFile::Material::ToonMode::TM_SHARED);
				Assert::IsTrue(m0.toonTextureID == 0);
				Assert::IsTrue(m0.memo.GetLength() == 0);
				Assert::IsTrue(m0.vertexCount == 1225 * 3);
			}
			{
				auto& mL = pmx.GetMaterial(pmx.GetLastMaterialID());
				Assert::IsTrue(strCmpFortest(GetText(mL.name), "�K���X", mL.name.GetLength()));
				Assert::IsTrue(mL.nameEng.GetLength() == 0);
				Assert::IsTrue(FloatEqual(mL.diffuse.x, 1.f));
				Assert::IsTrue(FloatEqual(mL.diffuse.y, 1.f));
				Assert::IsTrue(FloatEqual(mL.diffuse.z, 1.f));
				Assert::IsTrue(FloatEqual(mL.diffuse.w, 0.1f));
				Assert::IsTrue(FloatEqual(mL.specular.x, 0.f));
				Assert::IsTrue(FloatEqual(mL.specular.y, 0.f));
				Assert::IsTrue(FloatEqual(mL.specular.z, 0.f));
				Assert::IsTrue(FloatEqual(mL.specularity, 100.f));
				Assert::IsTrue(FloatEqual(mL.ambient.x, 0.25f));
				Assert::IsTrue(FloatEqual(mL.ambient.y, 0.25f));
				Assert::IsTrue(FloatEqual(mL.ambient.z, 0.25f));
				Assert::IsTrue(mL.GetDrawConfig(PmxFile::Material::DC_NOT_CULLING) == true);
				Assert::IsTrue(mL.GetDrawConfig(PmxFile::Material::DC_DRAW_SHADOW) == false);
				Assert::IsTrue(mL.GetDrawConfig(PmxFile::Material::DC_DRAW_TO_SELF_SHADOW_MAP) == false);
				Assert::IsTrue(mL.GetDrawConfig(PmxFile::Material::DC_DRAW_SELF_SHADOW) == false);
				Assert::IsTrue(mL.GetDrawConfig(PmxFile::Material::DC_DRAW_EDGE) == false);
				Assert::IsTrue(FloatEqual(mL.edgeColor.x, 0.f));
				Assert::IsTrue(FloatEqual(mL.edgeColor.y, 0.f));
				Assert::IsTrue(FloatEqual(mL.edgeColor.z, 0.f));
				Assert::IsTrue(FloatEqual(mL.edgeColor.w, 1.f));
				Assert::IsTrue(FloatEqual(mL.edgeSize, 1.f));
				Assert::IsTrue(mL.textureID == 5);
				Assert::IsTrue(mL.sphereTextureID == -1);
				Assert::IsTrue(mL.sphereMode == PmxFile::Material::SM_DISABLE);
				Assert::IsTrue(mL.toonMode == PmxFile::Material::ToonMode::TM_UNIQUE);
				Assert::IsTrue(mL.toonTextureID == -1);
				Assert::IsTrue(mL.memo.GetLength() == 0);
				Assert::IsTrue(mL.vertexCount == 240 * 3);
			}

			Assert::IsTrue(pmx.GetBoneCount() == 208);
			Assert::IsTrue(pmx.GetLastBoneID() == pmx.GetBoneCount() - 1);

			{
				auto& b0 = pmx.GetBone(0);

				Assert::IsTrue(strCmpFortest(GetText(b0.name), "�S�Ă̐e", b0.name.GetLength()));
				Assert::IsTrue(strCmpFortest(GetText(b0.nameEng), "master", b0.nameEng.GetLength()));
				Assert::IsTrue(FloatEqual(b0.position.x, 0.f));
				Assert::IsTrue(FloatEqual(b0.position.y, 0.f));
				Assert::IsTrue(FloatEqual(b0.position.z, -0.2195798f));
				Assert::IsTrue(b0.parentBoneID == -1);
				Assert::IsTrue(b0.transformHierarchy == 0);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IS_LINK_DEST_BY_BONE) == true);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_ROTATE) == true);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_MOVE) == true);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_DISPLAY) == true);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_CONTROLL) == true);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IK) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IS_ADD_LOCAL_BY_PARENT) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ADD_ROT) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ADD_MOV) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_LOCK_AXIS) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_LOCAL_AXIS) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_AFTER_PHISICS_TRANSFORM) == false);
				Assert::IsTrue(b0.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_OUT_PARENT_TRANSFORM) == false);
				Assert::IsTrue(b0.linkDestBoneID == 1);
				Assert::IsTrue(b0.addRot.addPalentBoneID == -1);
				Assert::IsTrue(FloatEqual(b0.addRot.addRatio, 0.f));
				Assert::IsTrue(b0.addMov.addPalentBoneID == -1);
				Assert::IsTrue(FloatEqual(b0.addMov.addRatio, 0.f));
				Assert::IsTrue(FloatEqual(b0.axisDirection.x, 0.f));
				Assert::IsTrue(FloatEqual(b0.axisDirection.y, 0.f));
				Assert::IsTrue(FloatEqual(b0.axisDirection.z, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisX_Direction.x, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisX_Direction.y, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisX_Direction.z, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisZ_Direction.x, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisZ_Direction.y, 0.f));
				Assert::IsTrue(FloatEqual(b0.localAxisZ_Direction.z, 0.f));
			}

			{
				auto& boneWithIK = pmx.GetBone(71);

				Assert::IsTrue(strCmpFortest(GetText(boneWithIK.name), "�����h�j", boneWithIK.name.GetLength()));
				Assert::IsTrue(strCmpFortest(GetText(boneWithIK.nameEng), "leg IK_R", boneWithIK.nameEng.GetLength()));
				Assert::IsTrue(FloatEqual(boneWithIK.position.x, 0.7553458f));
				Assert::IsTrue(FloatEqual(boneWithIK.position.y, 0.9500531f));
				Assert::IsTrue(FloatEqual(boneWithIK.position.z, 0.1935074f));
				Assert::IsTrue(boneWithIK.parentBoneID == 0);
				Assert::IsTrue(boneWithIK.transformHierarchy == 0);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IS_LINK_DEST_BY_BONE) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_ROTATE) == true);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_MOVE) == true);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_DISPLAY) == true);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ENABLE_CONTROLL) == true);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IK) == true);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_IS_ADD_LOCAL_BY_PARENT) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ADD_ROT) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_ADD_MOV) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_LOCK_AXIS) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_LOCAL_AXIS) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_AFTER_PHISICS_TRANSFORM) == false);
				Assert::IsTrue(boneWithIK.GetBoneConfig(PmxFile::Bone::BoneConfig::BC_OUT_PARENT_TRANSFORM) == false);
				//Assert::IsTrue(boneWithIK.linkDestBoneID == 1);
				Assert::IsTrue(FloatEqual(boneWithIK.positionOffs.x, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.positionOffs.y, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.positionOffs.z, 1.1f));
				Assert::IsTrue(boneWithIK.addRot.addPalentBoneID == -1);
				Assert::IsTrue(FloatEqual(boneWithIK.addRot.addRatio, 0.f));
				Assert::IsTrue(boneWithIK.addMov.addPalentBoneID == -1);
				Assert::IsTrue(FloatEqual(boneWithIK.addMov.addRatio, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.axisDirection.x, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.axisDirection.y, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.axisDirection.z, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisX_Direction.x, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisX_Direction.y, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisX_Direction.z, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisZ_Direction.x, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisZ_Direction.y, 0.f));
				Assert::IsTrue(FloatEqual(boneWithIK.localAxisZ_Direction.z, 0.f));
				Assert::IsTrue(boneWithIK.outParentTransformKey == 0.f);
				Assert::IsTrue(boneWithIK.ikTargetBoneID == 73);
				Assert::IsTrue(boneWithIK.ikLoopCount == 40);
				Assert::IsTrue(FloatEqual(RadianToDegree(boneWithIK.ikRotateLimit), 114.5916f));
				Assert::IsTrue(boneWithIK.ikLinkCount == 2);
				auto& ikl0 = boneWithIK.GetIK_Link(0);
				Assert::IsTrue(ikl0.linkBoneID == 70);
				Assert::IsTrue(ikl0.rotLimitConfig == PmxFile::Bone::IK_Link::IK_RLC_ON);
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.lowerLimit.x), -180.f));
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.lowerLimit.y), 0.f));
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.lowerLimit.z), 0.f));
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.upperLimit.x), -0.5f));
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.upperLimit.y), 0.f));
				Assert::IsTrue(FloatEqual(RadianToDegree(ikl0.upperLimit.z), 0.f));
			}

			Assert::IsTrue(pmx.GetMorphCount() == 133);

			// ���_���[�t�̓ǂݍ��݃e�X�g
			{
				auto& mph0 = pmx.GetMorph(0);

				Assert::IsTrue(strCmpFortest(GetText(mph0.name), "�܂΂���", mph0.name.GetLength()));
				Assert::IsTrue(mph0.nameEng.GetLength() == 0);
				Assert::IsTrue(mph0.type == PmdFile::Morph::MorphType::MT_EYE);
				Assert::IsTrue(mph0.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_VERTEX);
				Assert::IsTrue(mph0.offsCount == 442);

				{
					auto& offs0 = mph0.GetMorphOffsData(0);
					Assert::IsTrue(offs0.vertexOffs.vertexID == 13009);
					Assert::IsTrue(FloatEqual(offs0.vertexOffs.offsPos.x, 0.0004000664f));
					Assert::IsTrue(FloatEqual(offs0.vertexOffs.offsPos.y, -0.009400368f));
					Assert::IsTrue(FloatEqual(offs0.vertexOffs.offsPos.z, 0.f));
				}
				{
					auto& offsL = mph0.GetMorphOffsData(mph0.offsCount - 1);
					Assert::IsTrue(offsL.vertexOffs.vertexID == 17461);
					Assert::IsTrue(FloatEqual(offsL.vertexOffs.offsPos.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.vertexOffs.offsPos.y, -0.3823004f));
					Assert::IsTrue(FloatEqual(offsL.vertexOffs.offsPos.z, 0.07590008f));
				}
			}
			// �O���[�v���[�t�̓ǂݍ��݃e�X�g
			{
				auto& gMph = pmx.GetMorph(24);

				Assert::IsTrue(strCmpFortest(GetText(gMph.name), "�n�[�g", gMph.name.GetLength()));
				Assert::IsTrue(gMph.nameEng.GetLength() == 0);
				Assert::IsTrue(gMph.type == PmdFile::Morph::MorphType::MT_EYE);
				Assert::IsTrue(gMph.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_GROUP);
				Assert::IsTrue(gMph.offsCount == 2);

				{
					auto& offs0 = gMph.GetMorphOffsData(0);
					Assert::IsTrue(offs0.groupOffs.morphID == 120);
					Assert::IsTrue(FloatEqual(offs0.groupOffs.morphRatio, 1.f));
				}
				{
					auto& offsL = gMph.GetMorphOffsData(gMph.offsCount - 1);
					Assert::IsTrue(offsL.groupOffs.morphID == 26);
					Assert::IsTrue(FloatEqual(offsL.groupOffs.morphRatio, 1.f));
				}
			}
			// �}�e���A�����[�t�̓ǂݍ��݃e�X�g
			{
				auto& mMph = pmx.GetMorph(26);

				Assert::IsTrue(strCmpFortest(GetText(mMph.name), "�n�C���C�g����", mMph.name.GetLength()));
				Assert::IsTrue(mMph.nameEng.GetLength() == 0);
				Assert::IsTrue(mMph.type == PmdFile::Morph::MorphType::MT_EYE);
				Assert::IsTrue(mMph.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_MATERIAL);
				Assert::IsTrue(mMph.offsCount == 1);
				{
					auto& offs0 = mMph.GetMorphOffsData(0);
					Assert::IsTrue(offs0.materialOffs.materialID == 15);
					Assert::IsTrue(offs0.materialOffs.offsType == PmxFile::Morph::MaterialOffs::OffsType::OT_ADD);
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.w, -1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specularity, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeSize, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.w, 0.f));
				}
			}
			{
				auto& mMph2 = pmx.GetMorph(27);

				Assert::IsTrue(strCmpFortest(GetText(mMph2.name), "��AL����", mMph2.name.GetLength()));
				Assert::IsTrue(mMph2.nameEng.GetLength() == 0);
				Assert::IsTrue(mMph2.type == PmdFile::Morph::MorphType::MT_EYE);
				Assert::IsTrue(mMph2.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_MATERIAL);
				Assert::IsTrue(mMph2.offsCount == 2);
				{
					auto& offs0 = mMph2.GetMorphOffsData(0);
					Assert::IsTrue(offs0.materialOffs.materialID == 14);
					Assert::IsTrue(offs0.materialOffs.offsType == PmxFile::Morph::MaterialOffs::OffsType::OT_ADD);
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specularity, 10.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeSize, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.w, 0.f));
				}
				{
					auto& offsL = mMph2.GetMorphOffsData(mMph2.offsCount - 1);
					Assert::IsTrue(offsL.materialOffs.materialID == 15);
					Assert::IsTrue(offsL.materialOffs.offsType == PmxFile::Morph::MaterialOffs::OffsType::OT_ADD);
					Assert::IsTrue(FloatEqual(offsL.materialOffs.diffuse.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.diffuse.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.diffuse.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.diffuse.w, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.specular.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.specular.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.specular.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.specularity, 5.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.ambient.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.ambient.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.ambient.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.edgeColor.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.edgeColor.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.edgeColor.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.edgeColor.w, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.edgeSize, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.textureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.textureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.textureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.textureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.sphereTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.sphereTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.sphereTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.sphereTextureScale.w, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.toonTextureScale.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.toonTextureScale.y, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.toonTextureScale.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.materialOffs.toonTextureScale.w, 0.f));
				}
			}

			// UV���[�t�@�ǂݍ��݃e�X�g
			{
				auto& uvMph = pmx.GetMorph(118);

				Assert::IsTrue(strCmpFortest(GetText(uvMph.name), "�����F", uvMph.name.GetLength()));
				Assert::IsTrue(uvMph.nameEng.GetLength() == 0);
				Assert::IsTrue(uvMph.type == PmdFile::Morph::MorphType::MT_LIP);
				Assert::IsTrue(uvMph.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_UV);
				Assert::IsTrue(uvMph.offsCount == 295);

				{
					auto& offs0 = uvMph.GetMorphOffsData(0);
					Assert::IsTrue(offs0.uvOffs.vertexID == 14809);
					Assert::IsTrue(FloatEqual(offs0.uvOffs.uv.x, 0.f));
					Assert::IsTrue(FloatEqual(offs0.uvOffs.uv.y, -0.09956568f));
					Assert::IsTrue(FloatEqual(offs0.uvOffs.uv.z, 0.f));
					Assert::IsTrue(FloatEqual(offs0.uvOffs.uv.w, 0.f));
				}
				{
					auto& offsL = uvMph.GetMorphOffsData(uvMph.offsCount - 1);
					Assert::IsTrue(offsL.uvOffs.vertexID == 15200);
					Assert::IsTrue(FloatEqual(offsL.uvOffs.uv.x, 0.f));
					Assert::IsTrue(FloatEqual(offsL.uvOffs.uv.y, -0.09543592f));
					Assert::IsTrue(FloatEqual(offsL.uvOffs.uv.z, 0.f));
					Assert::IsTrue(FloatEqual(offsL.uvOffs.uv.w, 0.f));
				}
			}

			//�Ō�̗v�f�̓ǂݍ��݃e�X�g
			{
				auto& mphL = pmx.GetMorph(pmx.GetLastMorphID());

				Assert::IsTrue(strCmpFortest(GetText(mphL.name), "�G�b�W����", mphL.name.GetLength()));
				Assert::IsTrue(mphL.nameEng.GetLength() == 0);
				Assert::IsTrue(mphL.type == PmdFile::Morph::MorphType::MT_OTHER);
				Assert::IsTrue(mphL.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_MATERIAL);
				Assert::IsTrue(mphL.offsCount == 1);
				{
					auto& offs0 = mphL.GetMorphOffsData(0);
					Assert::IsTrue(offs0.materialOffs.materialID == -1);
					Assert::IsTrue(offs0.materialOffs.offsType == PmxFile::Morph::MaterialOffs::OffsType::OT_MUL);
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.diffuse.w, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specular.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.specularity, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.ambient.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeColor.w, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.edgeSize, 3.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.textureScale.w, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.sphereTextureScale.w, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.x, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.y, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.z, 1.f));
					Assert::IsTrue(FloatEqual(offs0.materialOffs.toonTextureScale.w, 1.f));
				}
			}

			// �\���g�ǂݍ��݃e�X�g
			Assert::IsTrue(pmx.GetDisplayFrameCount() == 12);
			{
				auto& d0 = pmx.GetDisplayFrame(0);

				Assert::IsTrue(strCmpFortest(GetText(d0.name), "Root", d0.name.GetLength()));
				Assert::IsTrue(strCmpFortest(GetText(d0.nameEng), "Root", d0.nameEng.GetLength()));
				Assert::IsTrue(d0.type == PmxFile::DisplayFrame::DisplayFrameType::DFT_SPECIAL);
				Assert::IsTrue(d0.frameElementCount == 1);
				{
					auto& fe0 = d0.GetFrameElement(0);
					Assert::IsTrue(fe0.elementType == PmxFile::DisplayFrame::FrameElement::FrameElementType::FET_BONE);
					Assert::IsTrue(fe0.objectID == 0);
				}
			}
			{
				auto& d1 = pmx.GetDisplayFrame(1);

				Assert::IsTrue(strCmpFortest(GetText(d1.name), "�\��", d1.name.GetLength()));
				Assert::IsTrue(strCmpFortest(GetText(d1.nameEng), "Exp", d1.nameEng.GetLength()));
				Assert::IsTrue(d1.type == PmxFile::DisplayFrame::DisplayFrameType::DFT_SPECIAL);
				Assert::IsTrue(d1.frameElementCount == 130);
				{
					auto& fe0 = d1.GetFrameElement(0);
					Assert::IsTrue(fe0.elementType == PmxFile::DisplayFrame::FrameElement::FrameElementType::FET_MORPH);
					Assert::IsTrue(fe0.objectID == 0);
				}
				{
					auto& fe0 = d1.GetFrameElement(d1.frameElementCount - 1);
					Assert::IsTrue(fe0.elementType == PmxFile::DisplayFrame::FrameElement::FrameElementType::FET_MORPH);
					Assert::IsTrue(fe0.objectID == 132);
				}
			}
			{
				auto& dL = pmx.GetDisplayFrame(pmx.GetLastDisplayFrameID());

				Assert::IsTrue(strCmpFortest(GetText(dL.name), "���̑�", dL.name.GetLength()));
				Assert::IsTrue(dL.nameEng.GetLength() == 0);
				Assert::IsTrue(dL.type == PmxFile::DisplayFrame::DisplayFrameType::DFT_NORMAL);
				Assert::IsTrue(dL.frameElementCount == 47);
				{
					auto& fe0 = dL.GetFrameElement(0);
					Assert::IsTrue(fe0.elementType == PmxFile::DisplayFrame::FrameElement::FrameElementType::FET_BONE);
					Assert::IsTrue(fe0.objectID == 14);
				}
				{
					auto& fe0 = dL.GetFrameElement(dL.frameElementCount - 1);
					Assert::IsTrue(fe0.elementType == PmxFile::DisplayFrame::FrameElement::FrameElementType::FET_BONE);
					Assert::IsTrue(fe0.objectID == 60);
				}
			}

			// ���̏��ǂݍ��݃e�X�g

			Assert::IsTrue(pmx.GetRigitbodyCount() == 41);

			{
				auto& r0 = pmx.GetRigitbody(0);

				Assert::IsTrue(strCmpFortest(GetText(r0.name), "��", r0.name.GetLength()));
				Assert::IsTrue(r0.nameEng.GetLength() == 0);
				Assert::IsTrue(r0.relationshipBoneID == 6);
				Assert::IsTrue(r0.group == 0);
				Assert::IsTrue(r0.groupTarget == 0b1111'1111'1111'1111);
				Assert::IsTrue(r0.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_SPHERE);
				Assert::IsTrue(FloatEqual(r0.shapeW, 1.f));
				Assert::IsTrue(FloatEqual(r0.position.x, 0.f));
				Assert::IsTrue(FloatEqual(r0.position.y, 15.62877f));
				Assert::IsTrue(FloatEqual(r0.position.z, -0.3113128f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r0.rotation.x), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r0.rotation.y), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r0.rotation.z), 0.00f));
				Assert::IsTrue(FloatEqual(r0.weight, 1.f));
				Assert::IsTrue(FloatEqual(r0.positionDim, 0.5f));
				Assert::IsTrue(FloatEqual(r0.rotationDim, 0.5f));
				Assert::IsTrue(FloatEqual(r0.recoil, 0.f));
				Assert::IsTrue(FloatEqual(r0.friction, 0.5f));
				Assert::IsTrue(r0.type == PmdFile::Rigitbody::RigitBodyType::RT_BONE_FOLLOW);
			}
			{
				// ��]��0�łȂ�����
				//auto& r1 = pmx.GetRigitbody(1);
				auto& r1 = pmx.GetRigitbody(1);

				Assert::IsTrue(strCmpFortest(GetText(r1.name), "�O��", r1.name.GetLength()));
				Assert::IsTrue(r1.nameEng.GetLength() == 0);
				Assert::IsTrue(r1.relationshipBoneID == 94);
				Assert::IsTrue(r1.group == 4);
				Assert::IsTrue(r1.groupTarget == 0b1111'1111'1110'1111);
				Assert::IsTrue(r1.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_CAPSULE);
				Assert::IsTrue(FloatEqual(r1.shapeW, 0.199335f));
				Assert::IsTrue(FloatEqual(r1.shapeH, 0.6f));
				Assert::IsTrue(FloatEqual(r1.position.x , 0.224f));
				Assert::IsTrue(FloatEqual(r1.position.y , 15.70385f));
				Assert::IsTrue(FloatEqual(r1.position.z , -1.912162f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r1.rotation.x), 0.7588f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r1.rotation.y), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r1.rotation.z), 0.00f));
				Assert::IsTrue(FloatEqual(r1.weight, 1.f));
				Assert::IsTrue(FloatEqual(r1.positionDim, 0.95f));
				Assert::IsTrue(FloatEqual(r1.rotationDim, 0.99f));
				Assert::IsTrue(FloatEqual(r1.recoil, 0.f));
				Assert::IsTrue(FloatEqual(r1.friction, 0.5f));
				Assert::IsTrue(r1.type == PmdFile::Rigitbody::RigitBodyType::RT_RIGITBODY);
			}
			{
				// �֘A�{�[�����I�t�Z�b�g�ʒu�����ꍇ
				// auto& r18 = pmx.GetRigitbody(18);
				auto& r18 = pmx.GetRigitbody(18);

				Assert::IsTrue(strCmpFortest(GetText(r18.name), "�X�q", r18.name.GetLength()));
				Assert::IsTrue(r18.nameEng.GetLength() == 0);
				Assert::IsTrue(r18.relationshipBoneID == 7);
				Assert::IsTrue(r18.group == 1);
				Assert::IsTrue(r18.groupTarget == 0b1111'1111'1111'1101);
				Assert::IsTrue(r18.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_SPHERE);
				Assert::IsTrue(FloatEqual(r18.shapeW, 1.f));
				Assert::IsTrue(FloatEqual(r18.position.x, 0.32f));
				Assert::IsTrue(FloatEqual(r18.position.y, 16.40877f));
				Assert::IsTrue(FloatEqual(r18.position.z, -0.05131277f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r18.rotation.x), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r18.rotation.y), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(r18.rotation.z), 0.00f));
				Assert::IsTrue(FloatEqual(r18.weight, 1.f));
				Assert::IsTrue(FloatEqual(r18.positionDim, 0.5f));
				Assert::IsTrue(FloatEqual(r18.rotationDim, 0.5f));
				Assert::IsTrue(FloatEqual(r18.recoil, 0.f));
				Assert::IsTrue(FloatEqual(r18.friction, 0.5f));
				Assert::IsTrue(r18.type == PmdFile::Rigitbody::RigitBodyType::RT_BONE_FOLLOW);
			}
			{
				// �Ō�̃f�[�^
				auto& rL = pmx.GetRigitbody(pmx.GetLastRigitbodyID());

				Assert::IsTrue(strCmpFortest(GetText(rL.name), "���H��", rL.name.GetLength()));
				Assert::IsTrue(rL.nameEng.GetLength() == 0);
				Assert::IsTrue(rL.relationshipBoneID == 11);
				Assert::IsTrue(rL.group == 4);
				Assert::IsTrue(rL.groupTarget == 0b1111'1111'1100'1111);
				Assert::IsTrue(rL.shapeType == PmdFile::Rigitbody::RigitBodyShapeType::RST_SPHERE);
				Assert::IsTrue(FloatEqual(rL.shapeW, 0.2f));
				Assert::IsTrue(FloatEqual(rL.position.x, 1.897798f));
				Assert::IsTrue(FloatEqual(rL.position.y, 16.30588f));
				Assert::IsTrue(FloatEqual(rL.position.z, -0.718574f));
				Assert::IsTrue(FloatEqual(RadianToDegree(rL.rotation.x), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(rL.rotation.y), 0.00f));
				Assert::IsTrue(FloatEqual(RadianToDegree(rL.rotation.z), 0.00f));
				Assert::IsTrue(FloatEqual(rL.weight, 1.f));
				Assert::IsTrue(FloatEqual(rL.positionDim, 0.5f));
				Assert::IsTrue(FloatEqual(rL.rotationDim, 0.5f));
				Assert::IsTrue(FloatEqual(rL.recoil, 0.f));
				Assert::IsTrue(FloatEqual(rL.friction, 0.5f));
				Assert::IsTrue(rL.type == PmdFile::Rigitbody::RigitBodyType::RT_BONE_FOLLOW);
			}

			Assert::IsTrue(pmx.GetJointCount() == 21);

			{
				auto& j0 = pmx.GetJoint(0);
				Assert::IsTrue(strCmpFortest(GetText(j0.name), "�O��", j0.name.GetLength()));
				Assert::IsTrue(j0.nameEng.GetLength() == 0);
				Assert::IsTrue(j0.type == PmxFile::Joint::JointType::JT_SPRING_6_DOF);
			}
			//last
		}

		// �{�[�����[�t�ǂݍ��݃e�X�g(�ŏ��̃��f���ɂ͑��݂��Ȃ��������ߑ����f���Ō���)
		TEST_METHOD(BoneMorphReadTest)
		{
			PmxFile pmx2(testPmxModelPath2);
			auto& bMph = pmx2.GetMorph(49);

			Assert::IsTrue(strCmpFortest(GetText(bMph.name), "���般��", bMph.name.GetLength()));
			Assert::IsTrue(bMph.nameEng.GetLength() == 0);
			Assert::IsTrue(bMph.type == PmdFile::Morph::MorphType::MT_OTHER);
			Assert::IsTrue(bMph.typeEX == PmxFile::Morph::MorphTypeEX::MTEX_BONE);
			Assert::IsTrue(bMph.offsCount == 14);

			{
				auto& offs0 = bMph.GetMorphOffsData(0);
				Assert::IsTrue(offs0.boneOffs.boneID == 53);
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.x, 0.f));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.y, 0.f));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.z, 0.f));

				MathUtil::Vector q = MathUtil::Vector::GenerateRotationQuaternionFromEuler(-9.945973f, -65.5116f, -30.5824f);

				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.x, q.GetFloat4().x));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.y, q.GetFloat4().y));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.z, q.GetFloat4().z));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.w, q.GetFloat4().w));
			}
			{
				auto& offs0 = bMph.GetMorphOffsData(bMph.offsCount - 1);

				Assert::IsTrue(offs0.boneOffs.boneID == 66);
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.x, 0.f));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.y, 0.f));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsPos.z, 0.f));

				MathUtil::Vector q = MathUtil::Vector::GenerateRotationQuaternionFromEuler(0.f, 0.f, -93.96494f);

				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.x, q.GetFloat4().x));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.y, q.GetFloat4().y));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.z, q.GetFloat4().z));
				Assert::IsTrue(FloatEqual(offs0.boneOffs.offsRotQ.w, q.GetFloat4().w));
			}


		}
	};
}

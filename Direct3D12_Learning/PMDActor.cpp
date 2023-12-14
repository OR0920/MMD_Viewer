// header
#include"PMDActor.h"
#include"PMDRenderer.h"
#include"Dx12Wrapper.h"

// std
#include<cassert>
#include<algorithm>
#include<array>

// windows
#pragma comment(lib, "winmm.lib")

// directx
#include"d3dx12.h"

// my lib
#include"System.h"
#include"MMDsdk.h"
#include"MathUtil.h"

#include"StringUtil.h"

#define CallInitFunctionWithAssert(func)\
if (FAILED(func))\
{\
	DebugMessageFunctionError(func, PMDActor::PMDActor());\
	assert(false);\
	return;\
}

static const char* const toonDirPath = "../x64/Debug/Test/Model/SharedToonTexture/";


static const char* const poseFilePath = "D:/Projects/directx12_samples-master/directx12_samples-master/Chapter10/motion/pose.vmd";
static const char* const simpleMortionFilePath = "D:/Projects/directx12_samples-master/directx12_samples-master/Chapter10/motion/swing.vmd";
static const char* const simpleMortionFilePath2 = "D:/Projects/directx12_samples-master/directx12_samples-master/Chapter10/motion/motion.vmd";
static const char* const simpleMortionFilePathIK = "D:/Projects/directx12_samples-master/directx12_samples-master/Chapter11/motion/squat.vmd";
static const char* const mortionFilePath = "D:/_3DModel/6666AAPのモーション素材集vol01/6666AAPのモーション素材集vol.01 歩き/Motion/シンプルウォーク.vmd";
static const char* const danceFilePath = "D:/_3DModel/ワールドイズマインをネルに踊らせてみた/ワールドイズマインをネルに踊らせてみた/ワールドイズマイン_ネル.vmd";

// mortion file
auto mfp = simpleMortionFilePathIK;

inline MathUtil::float4 GetFloat4FromPMD(const MMDsdk::float4& mf)
{
	MathUtil::float4 ret;

	ret.x = mf.x;
	ret.y = mf.y;
	ret.z = mf.z;
	ret.w = mf.w;

	return ret;
}

inline MathUtil::float3 GetFloat3FromPMD(const MMDsdk::float3& mf)
{
	MathUtil::float3 ret;

	ret.x = mf.x;
	ret.y = mf.y;
	ret.z = mf.z;

	return ret;
}

inline MathUtil::float2 GetFloat2FromPMD(const MMDsdk::float2& mf)
{
	MathUtil::float2 ret;

	ret.x = mf.x;
	ret.y = mf.y;

	return ret;
}

struct Vertex
{
	MathUtil::float3 pos;
	MathUtil::float3 normal;
	MathUtil::float2 uv;
	unsigned int boneNo[4];
	float boneWeight[4];
	unsigned char edgeFlg;
	uint16_t pad = 0;

	void GetDataFromPMD_Vertex(const MMDsdk::PmdFile::Vertex& v)
	{
		pos = GetFloat3FromPMD(v.position);
		normal = GetFloat3FromPMD(v.normal);
		uv = GetFloat2FromPMD(v.uv);
		boneNo[0] = v.GetBoneID(0);
		boneNo[1] = v.GetBoneID(1);
		boneWeight[0] = (v.weight / 100.f);
		boneWeight[1] = 1 - boneWeight[0];
		edgeFlg = v.edgeFlag;
	}
};

void PMDActor::MaterialOnShader::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);

	diffuse = GetFloat4FromPMD(data.diffuse);
	DebugOutFloat4(diffuse);
	specularity = data.specularity;

	specular = GetFloat3FromPMD(data.specular);
	ambient = GetFloat3FromPMD(data.ambient);
}

void PMDActor::MaterialOnCPU::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);

	toonIndex = data.toonIndex;
	edgeFlag = data.edgeFlag;
}


void PMDActor::MaterialOnCPU::LoadTexturePath(const char* const dirPath, const char* const texFileName)
{
	if (newArray_SplitFileName(nullptr, nullptr, texFileName) == false)
	{
		SetTexturePath(dirPath, texFileName);
	}
	else
	{
		char* texNameBuff1 = nullptr;
		char* texNameBuff2 = nullptr;
		newArray_SplitFileName(&texNameBuff1, &texNameBuff2, texFileName);

		SetTexturePath(dirPath, texNameBuff1);
		SetTexturePath(dirPath, texNameBuff2);

		System::SafeDeleteArray(&texNameBuff1);
		System::SafeDeleteArray(&texNameBuff2);
	}

}

void PMDActor::MaterialOnCPU::SetTexturePath(const char* const dirPath, const char* const texname)
{
	char* ext = nullptr;

	newArray_GetExtention(&ext, texname);

	char* pathBuff = nullptr;
	System::newArray_CopyAssetPath(&pathBuff, dirPath, texname);


	if (System::StringEqual(ext, "sph"))
	{
		sphPath = pathBuff;
	}
	else if (System::StringEqual(ext, "spa"))
	{
		spaPath = pathBuff;
	}
	else
	{
		texPath = pathBuff;
	}

	System::SafeDeleteArray(&pathBuff);
	System::SafeDeleteArray(&ext);
}

void PMDActor::Material::GetMaterialDataFromPMD(const void* materialDataFromFile)
{
	onShader.GetMaterialDataFromPMD(materialDataFromFile);
	onCPU.GetMaterialDataFromPMD(materialDataFromFile);

	const auto& data = *reinterpret_cast<const MMDsdk::PmdFile::Material*>(materialDataFromFile);
	vertexCount = data.vertexCount;
}

enum BoneType
{
	ROTATION,
	ROTATION_MOVE,
	IK,
	UNDEFINED,
	IK_CHILD,
	ROTATION_CHILD,
	IK_DESTINATION,
	INVISIVBLE
};

struct BoneNode
{
	int boneID = 0;
	BoneType type;
	int ikParentBoneID;
	MathUtil::float3 startPos;
	MathUtil::float3 endPosth;
	std::vector<BoneNode*> children;
};

struct KeyFrame
{
	unsigned int frameNo;
	MathUtil::Vector position;
	MathUtil::Vector q;
	MathUtil::float2 p1, p2;
	unsigned char bezierParam[64];

	KeyFrame(const MMDsdk::VmdFile::Mortion& file)
	{
		frameNo = file.frameNumber;
		position = GetFloat3FromPMD(file.position);
		q = GetFloat4FromPMD(file.rotation);

		p1 =
		{
			file.GetBezierParam(3) / 127.f,
			file.GetBezierParam(7) / 127.f
		};
		p2 =
		{
			file.GetBezierParam(11) / 127.f,
			file.GetBezierParam(15) / 127.f
		};

		for (int i = 0; i < 64; ++i)
		{
			bezierParam[i] = file.GetBezierParam(i);
		}

	}
};

struct IK_Data
{
	int boneID;
	int targetID;
	int iterations;
	float limit;
	std::vector<int> nodeIDs;
};

MathUtil::Matrix LookAtMatrix
(
	const MathUtil::Vector& lookAt,
	const MathUtil::float3& up,
	const MathUtil::float3& right
)
{
	MathUtil::Vector vz = lookAt;

	MathUtil::Vector vy = MathUtil::Vector::GenerateVectorNormalized(up);
	MathUtil::Vector vx = MathUtil::Vector::GenerateVectorNormalized(vy.Cross3(vz).GetFloat3());
	vy = MathUtil::Vector::GenerateVectorNormalized(vz.Cross3(vx).GetFloat3());

	if (std::abs(vy.Dot3(vz)) == 1.f)
	{
		vx = MathUtil::Vector::GenerateVectorNormalized(right);
		vy = MathUtil::Vector::GenerateVectorNormalized(vz.Cross3(vx).GetFloat3());
		vx = MathUtil::Vector::GenerateVectorNormalized(vy.Cross3(vz).GetFloat3());
	}

	MathUtil::Vector vr[4] = { vx, vy, vz, MathUtil::Vector::basicW };
	return MathUtil::Matrix(vr);
}

MathUtil::Matrix LookAtMatrix
(
	const MathUtil::Vector& origin,
	const MathUtil::Vector& lookAt,
	const MathUtil::float3& up,
	const MathUtil::float3& right
)
{
	return
		MathUtil::Matrix::
		GenerateMatrixTranspose(LookAtMatrix(origin, up, right))
		* LookAtMatrix(lookAt, up, right);
}

class MMD_Model
{
public:
	MMD_Model() {}
	void Load(const char* const filepath)
	{
		MMDsdk::PmdFile pmd(filepath);
		if (pmd.IsSuccessLoad() == true)
		{
			LoadModel(pmd);
			DebugMessage("load as pmd");
			return;
		}
		MMDsdk::PmxFile pmx(filepath);
		if (pmx.IsSuccessLoad() == true)
		{
			LoadModel(pmx);
			DebugMessage("load as pmx");
			return;
		}
	}
private:
	int frameCount = 0;
public:
	void LoadMortion(const char* const filepath)
	{
		MMDsdk::VmdFile vmdPose(filepath);
		for (int i = 0; i < vmdPose.GetMortionCount(); ++i)
		{
			auto& m = vmdPose.GetMortion(i);
			mMotion[m.name.GetText()].emplace_back(KeyFrame(m));

			frameCount = std::max<unsigned int>(frameCount, m.frameNumber);
		}

		for (auto& motion : mMotion)
		{
			std::sort
			(
				motion.second.begin(), motion.second.end(),
				[](const KeyFrame& l, const KeyFrame& r)
				{
					return l.frameNo <= r.frameNo;
				}
			);
		}

	}

	bool IsSuccessLoad() const
	{
		return isSuccess;
	}

	const int GetVertexCount() const
	{
		return mMesh.size();
	}
	const Vertex& GetVertex(const int i) const
	{
		return mMesh[i];
	}
	const int GetIndexCount() const
	{
		return mIndex.size();
	}
	const std::vector<int>& GetIndex() const
	{
		return mIndex;
	}

	const int GetMaterialCount() const
	{
		return mMaterials.size();
	}

	const PMDActor::Material& GetMaterial(const int i) const
	{
		return mMaterials[i];
	}
	const std::vector<PMDActor::Material>& GetMaterial() const
	{
		return mMaterials;
	}

	const int GetBoneCount() const
	{
		return mBoneMatrices.size();
	}

	const MathUtil::Matrix& GetBone(const int i) const
	{
		return mBoneMatrices[i];
	}

	void RecurSiveMatrixMultiply(BoneNode* node, const MathUtil::Matrix& mat)
	{
		mBoneMatrices[node->boneID] *= mat;

		for (auto& childNodes : node->children)
		{
			RecurSiveMatrixMultiply(childNodes, mBoneMatrices[node->boneID]);
		}
	}

	MathUtil::Matrix GetBoneTransForm(std::string name, const MathUtil::Matrix& mat)
	{
		auto& bone = mBoneNodeTable[name];
		auto& pos = bone.startPos;
		auto boneMat = MathUtil::Matrix::GenerateMatrixTranslation(pos);
		auto iBoneMat = MathUtil::Matrix::GenerateMatrixInverse(boneMat);

		return iBoneMat * mat * boneMat;
	}

	void AddBoneTransform(std::string name, const MathUtil::Matrix& mat)
	{
		auto& bone = mBoneNodeTable[name];
	
		mBoneMatrices[bone.boneID] = GetBoneTransForm(name, mat);
	}

private:
	DWORD _startTime = 0;
	const float epsilon = 0.0005f;

	float GetYFromXOnVezier(float x, const MathUtil::float2& a, const MathUtil::float2& b, uint8_t n)
	{
		if (a.x == a.y && b.x == b.y)
		{
			return x;
		}

		float t = x;
		const float k0 = 1 + 3 * a.x - 3 * b.x;
		const float k1 = 3 * b.x - 6 * a.x;
		const float k2 = 3 * a.x;


		for (int i = 0; i < n; ++i)
		{
			auto ft = k0 * t * t * t + k1 * t * t + k2 * t - x;
			if (-epsilon <= ft && ft <= epsilon)
			{
				break;
			}

			t -= ft / 2;
		}

		auto r = 1 - t;
		return t * t * t + 3 * t * t * r * b.y + 3 * t * r * r * a.y;
	}

	void SolveLookAtIK(const IK_Data& ik)
	{
		auto rootNode = mBoneNodeAddressArray[ik.nodeIDs[0]];
		auto targetNode = mBoneNodeAddressArray[ik.targetID];

		auto rootPos_1 = MathUtil::Vector(rootNode->startPos);
		auto targetPos_1 = MathUtil::Vector(targetNode->startPos);

		auto originVector = targetPos_1 - rootPos_1;
		originVector = MathUtil::Vector::GenerateVectorNormalized(originVector.GetFloat3());

		// xmvector3TransformCoordとの違いが判らなかったためバグったら修正する
		auto rootPos_2 = MathUtil::Vector::GenerateVector3Transform(mBoneMatrices[ik.nodeIDs[0]], rootPos_1);
		auto targetPos_2 = MathUtil::Vector::GenerateVector3Transform(mBoneMatrices[ik.boneID], targetPos_1);

		auto targetVector = targetPos_1 - rootPos_2;
		targetVector = MathUtil::Vector::GenerateVectorNormalized(targetVector.GetFloat3());

		mBoneMatrices[ik.nodeIDs[0]] =
			MathUtil::Matrix::GenerateMatrixTranslation(-rootPos_2)
			* LookAtMatrix
			(
				originVector,
				targetVector,
				MathUtil::float3(0.f, 1.f, 0.f),
				MathUtil::float3(1.f, 0.f, 0.f)
			)
			* MathUtil::Matrix::GenerateMatrixTranslation(rootPos_2);
			
	}

	void SolveCosIK(const IK_Data& ik)
	{
		std::vector<MathUtil::Vector> positions;
		std::array<float, 2> edgeLens;

		auto& targetNode = mBoneNodeAddressArray[ik.boneID];
		auto targetPos = MathUtil::Vector::GenerateVector3Transform(mBoneMatrices[ik.boneID], targetNode->startPos);

		auto endNode = mBoneNodeAddressArray[ik.targetID];
		positions.emplace_back(endNode->startPos);

		for (auto chainBoneID : ik.nodeIDs)
		{
			positions.emplace_back(mBoneNodeAddressArray[chainBoneID]->startPos);
		}

		std::reverse(positions.begin(), positions.end());

		edgeLens[0] = (positions[1] - positions[0]).Vector3Length();
		edgeLens[1] = (positions[2] - positions[1]).Vector3Length();

		positions[0] = MathUtil::Vector::GenerateVector3Transform(mBoneMatrices[ik.nodeIDs[1]], positions[0]);
		positions[2] = MathUtil::Vector::GenerateVector3Transform(mBoneMatrices[ik.boneID], positions[2]);

		auto linearVector = positions[2] - positions[0];

		float A = linearVector.Vector3Length();
		float B = edgeLens[0];
		float C = edgeLens[0];

		linearVector = MathUtil::Vector::GenerateVectorNormalized(linearVector.GetFloat3());

		float theta_1 = std::acosf((A * A + B * B - C * C) / (2 * A * B));
		float theta_2 = std::acosf((B * B + C * C - A * A) / (2 * B * C));


		MathUtil::Vector axis = MathUtil::Vector::zero;

		if (std::find(kneeIDs.begin(), kneeIDs.end(), ik.nodeIDs[0]) == kneeIDs.end())
		{
			auto vm = positions[2] - positions[0];
			vm = MathUtil::Vector::GenerateVectorNormalized(vm.GetFloat3());

			auto vt = targetPos - positions[0];
			vt = MathUtil::Vector::GenerateVectorNormalized(vt.GetFloat3());

			axis = vt.Cross3(vm);
		}
		else
		{
			axis = MathUtil::Vector::basicX;
		}

		auto mat_1 =
			MathUtil::Matrix::GenerateMatrixTranslation(-positions[0])
			* MathUtil::Matrix::GenerateMatrixRotationAxis(axis, theta_1)
			* MathUtil::Matrix::GenerateMatrixTranslation(positions[0]);

		auto mat_2 =
			MathUtil::Matrix::GenerateMatrixTranslation(-positions[1])
			* MathUtil::Matrix::GenerateMatrixRotationAxis(axis, theta_2 - MathUtil::PI)
			* MathUtil::Matrix::GenerateMatrixTranslation(positions[1]);

		mBoneMatrices[ik.nodeIDs[1]] *= mat_1;
		mBoneMatrices[ik.nodeIDs[0]] = mat_2 * mBoneMatrices[ik.nodeIDs[1]];
		mBoneMatrices[ik.targetID] = mBoneMatrices[ik.nodeIDs[0]];
	}

	void SolveCCDIK(const IK_Data& ik)
	{
		auto targetBoneNode = mBoneNodeAddressArray[ik.boneID];
		auto targetOriginPos = targetBoneNode->startPos;

		// キャンセル用親ボーン逆行列
		auto parentMat = mBoneMatrices[mBoneNodeAddressArray[ik.boneID]->ikParentBoneID];
		auto iParentMat = MathUtil::Matrix::GenerateMatrixInverse(parentMat);
		// ボーン行列
		auto boneMat = mBoneMatrices[ik.boneID];

		auto targetNextPos = MathUtil::Vector::GenerateVector3Transform(boneMat * iParentMat, targetOriginPos);
		
		MathUtil::Vector endPos = mBoneNodeAddressArray[ik.targetID]->startPos;

		std::vector<MathUtil::Vector> bonePositions;

		for (auto& ci : ik.nodeIDs)
		{
			bonePositions.push_back(mBoneNodeAddressArray[ci]->startPos);
		}

		std::vector<MathUtil::Matrix> mats;
		mats.assign(bonePositions.size(), MathUtil::Matrix::GenerateMatrixIdentity());

		auto ikLimitRad = ik.limit * MathUtil::PI;

		for (int c = 0; c < ik.iterations; ++c)
		{
			auto ikVec = endPos - targetNextPos;
			if (ikVec.Vector3Length() <= epsilon)
			{
				break;
			}

			for (int bi = 0; bi < bonePositions.size(); ++bi)
			{
				const auto& pos = bonePositions[bi];

				auto vecToEnd = endPos - pos;
				vecToEnd = MathUtil::Vector::GenerateVectorNormalized(vecToEnd.GetFloat3());
				auto vecToTarget = targetNextPos - pos;
				vecToTarget = MathUtil::Vector::GenerateVectorNormalized(vecToTarget.GetFloat3());

				auto vecTargetToEnd = vecToEnd - vecToTarget;
				if (vecTargetToEnd.Vector3Length() <= epsilon)
				{
					continue;
				}

				auto cross = vecToEnd.Cross3(vecToTarget);
				cross = MathUtil::Vector::GenerateVectorNormalized(cross.GetFloat3());

				float angle = vecToEnd.AngleBetWeenVector3(vecToTarget);

				angle = std::min<>(angle, ikLimitRad);
				
				MathUtil::Matrix rot = MathUtil::Matrix::GenerateMatrixRotationAxis(cross, angle);
				
				auto mat =
					MathUtil::Matrix::GenerateMatrixTranslation(-pos)
					* rot
					* MathUtil::Matrix::GenerateMatrixTranslation(pos);

				mats[bi] *= mat;

				for (auto i = bi - 1; i >= 0; --i)
				{
					bonePositions[i] = MathUtil::Vector::GenerateVector3Transform(mat, bonePositions[i]);
				}

				endPos = MathUtil::Vector::GenerateVector3Transform(mat, endPos);

				if ((endPos - targetNextPos).Vector3Length() <= epsilon)
				{
					break;
				}
			}
		}


		int i = 0;

		for (auto& c : ik.nodeIDs)
		{
			mBoneMatrices[c] = mats[i];
			++i;
		}

		auto rootNode = mBoneNodeAddressArray[ik.nodeIDs.back()];
		RecurSiveMatrixMultiply(rootNode, parentMat);
		// last
	}

	void SolveIK(int frameNo)
	{
		for (auto& ik : mIK_Data)
		{
			switch (ik.nodeIDs.size())
			{
			case 0: assert(true); break;
			case 1: SolveLookAtIK(ik); break;
			case 2: SolveCosIK(ik); break;
			default: SolveCCDIK(ik); break;
			}
		}
	}

public:
	void PlayAnimation()
	{
		_startTime = timeGetTime();
	}

	void Update()
	{
		DWORD elapsedTime = timeGetTime() - _startTime;
		DWORD frameNo = 30 * (elapsedTime / 1000.f);

		frameNo %= frameCount;

		//DebugOutParamI(frameNo);

		std::fill(mBoneMatrices.begin(), mBoneMatrices.end(), MathUtil::Matrix::GenerateMatrixIdentity());
		//SolveIK();

		//AddBoneTransform("右手首", MathUtil::Matrix::GenerateMatrixRotationX(MathUtil::DegreeToRadian(40.f)));
		//AddBoneTransform("右ひじ", MathUtil::Matrix::GenerateMatrixRotationZ(MathUtil::DegreeToRadian(-90.f)));
		//AddBoneTransform("左手首", MathUtil::Matrix::GenerateMatrixRotationX(MathUtil::DegreeToRadian(50.f)));
		//AddBoneTransform("左ひじ", MathUtil::Matrix::GenerateMatrixRotationZ(MathUtil::DegreeToRadian(90.f)));
		for (auto& m : mMotion)
		{
			if (mBoneNodeTable.find(m.first) == mBoneNodeTable.end()) continue;

			auto mortion = m.second;
			auto rit = std::find_if
			(
				mortion.rbegin(), mortion.rend(),
				[frameNo](const KeyFrame& mortion)
				{

					return mortion.frameNo <= frameNo;
				}
			);

			if (rit == mortion.rend()) continue;

			auto it = rit.base();

			MathUtil::Vector rotationQ;
			MathUtil::Vector offset = rit->position;
			if (it != mortion.end())
			{
				auto t = static_cast<float>(frameNo - rit->frameNo) / static_cast<float>(it->frameNo - rit->frameNo);

				t = GetYFromXOnVezier(t, it->p1, it->p2, 12);

				rotationQ = MathUtil::Vector::GenerateRotationQuaternionSlerp(rit->q, it->q, t);
				offset = MathUtil::Vector::GenerateVectorLerp(offset, it->position, t);
			}
			else
			{
				rotationQ = rit->q;
			}

			auto rotMat = GetBoneTransForm(m.first, MathUtil::Matrix::GenerateMatrixRotationQ(rotationQ));
			mBoneMatrices[mBoneNodeTable.find(m.first)->second.boneID] = rotMat * MathUtil::Matrix::GenerateMatrixTranslation(offset);
		}


		auto centerMat = MathUtil::Matrix::GenerateMatrixIdentity();
		RecurSiveMatrixMultiply(&mBoneNodeTable["センター"], centerMat);
		
		SolveIK(frameNo);

	}


private:
	void LoadModel(const MMDsdk::PmdFile& pmd)
	{
		mMesh.assign(pmd.GetVertexCount(), {});
		for (int i = 0; i < pmd.GetVertexCount(); ++i)
		{
			mMesh[i].GetDataFromPMD_Vertex(pmd.GetVertex(i));
		}

		mIndex.assign(pmd.GetIndexCount(), 0);
		for (int i = 0; i < pmd.GetIndexCount(); ++i)
		{
			mIndex[i] = static_cast<int>(pmd.GetIndex(i));
		}


		mMaterials.assign(pmd.GetMaterialCount(), {});
		for (int i = 0; i < pmd.GetMaterialCount(); ++i)
		{
			auto& fileM = pmd.GetMaterial(i);
			mMaterials[i].GetMaterialDataFromPMD(&fileM);
			if (fileM.texturePath.GetText()[0] != '\0')
			{
				mMaterials[i].onCPU.LoadTexturePath(pmd.GetDirectoryPath(), fileM.texturePath.GetText());
			}
		}

		mBoneNames.assign(pmd.GetBoneCount(), {});
		mBoneNodeAddressArray.assign(pmd.GetBoneCount(), nullptr);

		kneeIDs.clear();

		for (int i = 0; i < pmd.GetBoneCount(); ++i)
		{
			auto& fb = pmd.GetBone(i);
			mBoneNames[i] = fb.name.GetText();
			auto& node = mBoneNodeTable[fb.name.GetText()];
			node.boneID = i;
			node.startPos = GetFloat3FromPMD(fb.headPos);

			mBoneNodeAddressArray[i] = &node;

			std::string boneName = fb.name.GetText();
			if (boneName.find("ひざ") != std::string::npos)
			{
				kneeIDs.emplace_back(i);
			}
		}
		for (int i = 0; i < pmd.GetBoneCount(); ++i)
		{
			auto& fb = pmd.GetBone(i);

			if (0 <= fb.parentIndex && fb.parentIndex < pmd.GetBoneCount())
			{
				auto& parentName = mBoneNames[fb.parentIndex];
				mBoneNodeTable[parentName].children.emplace_back(&mBoneNodeTable[fb.name.GetText()]);
			}
		}

		mBoneMatrices.assign(pmd.GetBoneCount(), MathUtil::Matrix::GenerateMatrixIdentity());

		mIK_Data.assign(pmd.GetIKCount(), {});
		for (int i = 0; i < pmd.GetIKCount(); ++i)
		{
			auto& ik = pmd.GetIKData(i);
			auto& mik = mIK_Data[i];

			mik.boneID = ik.ikBoneIndex;
			mik.targetID = ik.ikTargetBoneIndex;
			mik.iterations = ik.iterations;
			mik.limit = ik.controlWeight;

			mik.nodeIDs.assign(ik.ikChainCount, -1);
			for (int i = 0; i < ik.ikChainCount; ++i)
			{
				mik.nodeIDs[i] = ik.GetIkChildBoneID(i);
			}
		}

		isSuccess = true;
	}
	void LoadModel(const MMDsdk::PmxFile& pmx)
	{
		mMesh.assign(pmx.GetVertexCount(), {});
		for (int i = 0; i < pmx.GetVertexCount(); ++i)
		{
			auto& v = mMesh[i];
			auto& xv = pmx.GetVertex(i);
			v.pos = GetFloat3FromPMD(xv.position);
			v.normal = GetFloat3FromPMD(xv.normal);
			v.uv = GetFloat2FromPMD(xv.uv);

			v.boneNo[0] = xv.GetBoneID(0);
			v.boneNo[1] = xv.GetBoneID(1);
			v.boneNo[2] = xv.GetBoneID(2);
			v.boneNo[3] = xv.GetBoneID(3);
			v.boneWeight[0] = xv.GetWeight(0);
			v.boneWeight[1] = xv.GetWeight(1);
			v.boneWeight[2] = xv.GetWeight(2);
			v.boneWeight[3] = xv.GetWeight(3);
			v.edgeFlg = xv.edgeRate;
		}

		mIndex.assign(pmx.GetIndexCount(), 0);
		for (int i = 0; i < pmx.GetIndexCount(); ++i)
		{
			mIndex[i] = pmx.GetIndex(i);
		}

		auto materialCount = pmx.GetMaterialCount();

		mMaterials.assign(materialCount, {});
		for (int i = 0; i < materialCount; ++i)
		{
			auto& xm = pmx.GetMaterial(i);
			auto& ms = mMaterials[i].onShader;
			ms.diffuse = GetFloat4FromPMD(xm.diffuse);
			ms.specular = GetFloat3FromPMD(xm.specular);
			ms.specularity = xm.specularity;
			ms.ambient = GetFloat3FromPMD(xm.ambient);
			auto& mc = mMaterials[i].onCPU;
			mc.edgeFlag = xm.edgeSize;

			if (xm.toonMode == MMDsdk::PmxFile::Material::TM_SHARED)
			{
				mc.toonIndex = xm.toonTextureID;
			}
			else if (xm.toonMode == MMDsdk::PmxFile::Material::TM_UNIQUE && xm.toonTextureID != -1)
			{
				char* toonPath = nullptr;
				System::newArray_CopyAssetPath(&toonPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.toonTextureID).GetText());
				mc.uniqueToonPath = toonPath;
				System::SafeDeleteArray(&toonPath);
			}
			else
			{
				mc.toonIndex = -1;
			}
			mMaterials[i].vertexCount = xm.vertexCount;
			char* texPath = nullptr;
			if (xm.textureID != -1)
			{
				System::newArray_CopyAssetPath(&texPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.textureID).GetText());
				mc.texPath = texPath;
				System::SafeDeleteArray(&texPath);
			}
			if (xm.sphereTextureID != -1)
			{
				System::newArray_CopyAssetPath(&texPath, pmx.GetDirectoryPath(), pmx.GetTexturePath(xm.sphereTextureID).GetText());
				switch (xm.sphereMode)
				{
				case MMDsdk::PmxFile::Material::SphereMode::SM_SPH:
					mc.sphPath = texPath;
				case MMDsdk::PmxFile::Material::SphereMode::SM_SPA:
					mc.spaPath = texPath;
				default:
					break;
				}
				System::SafeDeleteArray(&texPath);
			}
		}

		mBoneNames.assign(pmx.GetBoneCount(), {});

		for (int i = 0; i < pmx.GetBoneCount(); ++i)
		{
			auto& fb = pmx.GetBone(i);
			mBoneNames[i] = fb.name.GetText();
			auto& node = mBoneNodeTable[fb.name.GetText()];
			node.boneID = i;
			node.startPos = GetFloat3FromPMD(fb.position);
		}
		for (int i = 0; i < pmx.GetBoneCount(); ++i)
		{
			auto& fb = pmx.GetBone(i);

			if (0 <= fb.parentBoneID && fb.parentBoneID < pmx.GetBoneCount())
			{
				auto& parentName = mBoneNames[fb.parentBoneID];
				mBoneNodeTable[parentName].children.emplace_back(&mBoneNodeTable[fb.name.GetText()]);
			}
		}

		mBoneMatrices.assign(pmx.GetBoneCount(), MathUtil::Matrix::GenerateMatrixIdentity());

		isSuccess = true;
	}

	bool isSuccess = false;

	std::vector<Vertex> mMesh;
	std::vector<int> mIndex;
	std::vector<PMDActor::Material> mMaterials;
	std::map<std::string, BoneNode> mBoneNodeTable;
	std::vector<std::string> mBoneNames;
	std::vector<MathUtil::Matrix> mBoneMatrices;
	std::unordered_map<std::string, std::vector<KeyFrame>> mMotion;

	std::vector<IK_Data> mIK_Data;

	std::vector<BoneNode*> mBoneNodeAddressArray;
	std::vector<uint32_t> kneeIDs;
};

HRESULT PMDActor::LoadPMDFile(const std::string argFilepath)
{
	model = new MMD_Model();
	model->Load(argFilepath.c_str());

	if (model->IsSuccessLoad() == false)
	{
		return S_FALSE;
	}

	model->LoadMortion(mfp);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	DebugOutParam(model->GetVertexCount());
	DebugOutParam(sizeof(Vertex));
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(model->GetVertexCount() * sizeof(Vertex));

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mVertexBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(mVertexBuffer), PMDActor::LoadPMDFile());
		DebugOutParamHex(result);
		assert(SUCCEEDED(result));
		return result;
	}

	char* vertexBufferMap = nullptr;
	result = mVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferMap));

	if (FAILED(result))
	{
		DebugMessageFunctionError(mVertexBuffer->Map(), PMDActor::LoadPMDFile);
		assert(SUCCEEDED(result));
		return result;
	}

	for (int i = 0; i < model->GetVertexCount(); ++i)
	{
		*reinterpret_cast<Vertex*>(vertexBufferMap) = model->GetVertex(i);
		vertexBufferMap += sizeof(Vertex);
	}
	mVertexBuffer->Unmap(0, nullptr);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = sizeof(Vertex) * model->GetVertexCount();
	mVertexBufferView.StrideInBytes = sizeof(Vertex);


	auto indexResDesc = CD3DX12_RESOURCE_DESC::Buffer(model->GetIndexCount() * sizeof(int));
	DebugOutParam(model->GetIndexCount());
	DebugOutParam(indexResDesc.Width);
	result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&indexResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mIndexBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(mIndexBuffer), PMDActor::LoadPMDFile());
		assert(SUCCEEDED(result));
		return result;
	}

	int* mappedIndex = nullptr;
	result = mIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mIndexBuffer->Map(), PMDActor::LoadPMDFile());
		assert(SUCCEEDED(result));
		return result;
	}
	MMDsdk::PmxFile pmx(argFilepath.c_str());
	for (int i = 0; i < model->GetIndexCount(); ++i)
	{
		mappedIndex[i] = model->GetIndex()[i];
	}
	//std::copy(model->GetIndex().begin(), model->GetIndex().end(), mappedIndex);
	mIndexBuffer->Unmap(0, nullptr);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = sizeof(int) * model->GetIndexCount();


	std::vector<std::string> toonTexPath(10);
	std::vector<std::string> toonTexName =
	{
		"toon01.bmp",
		"toon02.bmp",
		"toon03.bmp",
		"toon04.bmp",
		"toon05.bmp",
		"toon06.bmp",
		"toon07.bmp",
		"toon08.bmp",
		"toon09.bmp",
		"toon10.bmp"
	};

	for (int i = 0; i < 10; ++i)
	{
		char* texPath = nullptr;
		System::newArray_CopyAssetPath(&texPath, toonDirPath, toonTexName[i].c_str());

		toonTexPath[i] = texPath;
		System::SafeDeleteArray(&texPath);
	}

	mTextureResources.assign(model->GetMaterialCount(), nullptr);
	mSPH_TextureResources.assign(model->GetMaterialCount(), nullptr);
	mSPA_TextureResources.assign(model->GetMaterialCount(), nullptr);
	mToonTextureResources.assign(model->GetMaterialCount(), nullptr);

	for (int i = 0; i < model->GetMaterialCount(); ++i)
	{
		auto& m = model->GetMaterial(i).onCPU;
		if (0 <= m.toonIndex && m.toonIndex < 10)
		{
			mToonTextureResources[i] = mDx12.GetTextureByPath(toonTexPath[m.toonIndex]);
		}
		else if (m.uniqueToonPath.size() != 0)
		{
			mToonTextureResources[i] = mDx12.GetTextureByPath(m.uniqueToonPath);
		}

		if (m.texPath.size() != 0)
		{
			mTextureResources[i] = mDx12.GetTextureByPath(m.texPath);
		}

		if (m.sphPath.size() != 0)
		{
			mSPH_TextureResources[i] = mDx12.GetTextureByPath(m.sphPath);
		}

		if (m.spaPath.size() != 0)
		{
			mSPA_TextureResources[i] = mDx12.GetTextureByPath(m.spaPath);
		}
	}
}

HRESULT PMDActor::CreateTransformView()
{
	auto buffSize = sizeof(Transform) + sizeof(MathUtil::Matrix) * model->GetBoneCount();
	buffSize = (buffSize + 0xff) & ~0xff;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mTransformBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateCommittedResource(mTransformBuffer),
			PMDActor::CreateTransformView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	result = mTransformBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedMatrices));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mTransformBuffer->Map(), PMDActor::CreateTransformView());
		assert(SUCCEEDED(result));
		return result;
	}

	*mMappedMatrices = mTransform.world;

	model->Update();
	for (int i = 0; i < model->GetBoneCount(); ++i)
	{
		mMappedMatrices[i + 1] = model->GetBone(i);
	}


	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = mDx12.GetDevice()->CreateDescriptorHeap
	(
		&transformDescHeapDesc,
		IID_PPV_ARGS(mTransformHeap.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateDescriptorHeap(mTransformHeap),
			PMDActor::CreateTransformView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mTransformBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	mDx12.GetDevice()->CreateConstantBufferView(&cbvDesc, mTransformHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT PMDActor::CreateMaterialData()
{
	auto& mMaterials = model->GetMaterial();
	auto materialBuffSize = sizeof(MaterialOnShader);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	DebugOutParamI(materialBuffSize);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * mMaterials.size());

	auto result = mDx12.GetDevice()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mMaterialBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError(mDx12.GetDevice()->CreateCommittedResource(), PMDActor::CreateMaterialData());
		assert(SUCCEEDED(result));
		return result;
	}

	unsigned char* mapMaterial = nullptr;
	result = mMaterialBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterial));
	if (FAILED(result))
	{
		DebugMessageFunctionError(mMaterialBuffer->Map(), PMDActor::CreateMaterialData());
		assert(SUCCEEDED(result));
		return result;
	}

	for (int i = 0; i < mMaterials.size(); ++i)
	{
		//auto& m = mMaterials[i].onShader;
		//auto& mm = *reinterpret_cast<MaterialOnShader*>(mapMaterial);
		//mm.diffuse = m.diffuse;
		//mm.specular = m.specular;
		//mm.specularity = m.specularity;
		//mm.ambient = m.ambient;
		*reinterpret_cast<MaterialOnShader*>(mapMaterial) = mMaterials[i].onShader;
		mapMaterial += materialBuffSize;
	}

	mMaterialBuffer->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDActor::CreateMaterialAndTextureView()
{
	auto& mMaterials = model->GetMaterial();
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = mMaterials.size() * 5;
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;
	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	auto result = mDx12.GetDevice()->CreateDescriptorHeap
	(
		&materialDescHeapDesc,
		IID_PPV_ARGS(mMaterialHeap.ReleaseAndGetAddressOf())
	);

	if (FAILED(result))
	{
		DebugMessageFunctionError
		(
			mDx12.GetDevice()
			->CreateDescriptorHeap(mMaterialHeap),
			PMDActor::CreateMaterialAndTextureView()
		);
		assert(SUCCEEDED(result));
		return result;
	}

	auto materialBuffSize = sizeof(MaterialOnShader);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBV_Desc = {};
	matCBV_Desc.BufferLocation = mMaterialBuffer->GetGPUVirtualAddress();
	matCBV_Desc.SizeInBytes = materialBuffSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapHandle(mMaterialHeap->GetCPUDescriptorHandleForHeapStart());

	auto incSize = mDx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < mMaterials.size(); ++i)
	{
		mDx12.GetDevice()->CreateConstantBufferView(&matCBV_Desc, matDescHeapHandle);
		matDescHeapHandle.ptr += incSize;
		matCBV_Desc.BufferLocation += materialBuffSize;
		auto m = mMaterials[i].onCPU;
		DebugOutString(m.texPath.c_str());
		DebugOutString(m.sphPath.c_str());
		DebugOutString(m.spaPath.c_str());
		DebugOutString(m.uniqueToonPath.c_str());

		if (mTextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureWhite->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureWhite.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mTextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mTextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);

		if (mSPH_TextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureWhite->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureWhite.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mSPH_TextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mSPH_TextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);


		if (mSPA_TextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureBlack->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureBlack.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mSPA_TextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mSPA_TextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);




		if (mToonTextureResources[i] == nullptr)
		{
			srvDesc.Format = mRenderer.mDefaultTextureGrad->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mRenderer.mDefaultTextureGrad.Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		else
		{
			srvDesc.Format = mToonTextureResources[i]->GetDesc().Format;
			mDx12.GetDevice()->CreateShaderResourceView
			(
				mToonTextureResources[i].Get(),
				&srvDesc,
				matDescHeapHandle
			);
		}
		matDescHeapHandle.Offset(incSize);
	}
}


void PMDActor::Update()
{
	//mAngle += 0.03f;
	mMappedMatrices[0] = MathUtil::Matrix::GenerateMatrixRotationY(mAngle);

	model->Update();
	for (int i = 0; i < model->GetBoneCount(); ++i)
	{
		mMappedMatrices[i + 1] = model->GetBone(i);
	}
}

void PMDActor::Draw()
{
	const auto& cmdl = mDx12.GetCommandList();

	cmdl->IASetVertexBuffers(0, 1, &mVertexBufferView);
	cmdl->IASetIndexBuffer(&mIndexBufferView);

	ID3D12DescriptorHeap* transHeaps[] = { mTransformHeap.Get() };
	cmdl->SetDescriptorHeaps(1, transHeaps);
	cmdl->SetGraphicsRootDescriptorTable(1, mTransformHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* materialHeaps[] = { mMaterialHeap.Get() };
	cmdl->SetDescriptorHeaps(1, materialHeaps);

	auto materialHandle = mMaterialHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int indexOffs = 0;
	auto cbv_srv_incSize = mDx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

	for (auto& m : model->GetMaterial())
	{
		cmdl->SetGraphicsRootDescriptorTable(2, materialHandle);
		cmdl->DrawIndexedInstanced(m.vertexCount, 1, indexOffs, 0, 0);
		materialHandle.ptr += cbv_srv_incSize;
		indexOffs += m.vertexCount;
	}
}

PMDActor::~PMDActor()
{
	System::SafeDelete(&model);
}


PMDActor::PMDActor(const std::string argFilepath, PMDRenderer& argRenderer)
	:
	mRenderer(argRenderer),
	mDx12(argRenderer.mDx12),
	mAngle(0.f)
{
	mTransform.world = DirectX::XMMatrixIdentity();
	CallInitFunctionWithAssert(LoadPMDFile(argFilepath));
	CallInitFunctionWithAssert(CreateTransformView());
	CallInitFunctionWithAssert(CreateMaterialData());
	CallInitFunctionWithAssert(CreateMaterialAndTextureView());
	model->PlayAnimation();
}

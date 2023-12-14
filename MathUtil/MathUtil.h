/*
	���w���C�u����

	Direct X Math����A���O�����B�؂����锖�����b�p�[���C�u����
	�ڐA���Ȃǂ͈�U�l���Ȃ�

	�K�v�Ȃ��̂������b�v����

*/
#ifndef _MATH_UTIL_H_
#define _MATH_UTIL_H_

#include<DirectXMath.h>

#ifdef _DEBUG
// ���C�u�����ŗL�̍\���̂��R���\�[���o�͂���}�N��
#include<iostream>
#define DebugOutVector(v) std::cout << #v  << " = ( " << v.GetFloat4().x << " , " << v.GetFloat4().y << " , " << v.GetFloat4().z << " , "<< v.GetFloat4().w << " )" << std::endl;
#define DebugOutFloat4(v) std::cout << #v  << " = ( " << v.x << " , " << v.y << " , " << v.z << " , " << v.w << " )" << std::endl;
#define DebugOutFloat3(v) std::cout << #v  << " = ( " << v.x << " , " << v.y << " , " << v.z << " )" << std::endl;
#define DebugOutFloat2(v) std::cout << #v  << " = ( " << v.x << " , " << v.y << " )" << std::endl;
#else
#define DebugOutVector(v)
#define DebugOutFloat4(v)
#define DebugOutFloat3(v)
#define DebugOutFloat2(v)
#endif

namespace MathUtil
{
	extern const float PI;
	extern const float PI_DIV2;
	extern const float PI_DIV4;
	extern const float _2PI;

	//�f�[�^�p
	using float2 = DirectX::XMFLOAT2;
	using float3 = DirectX::XMFLOAT3;
	using float4 = DirectX::XMFLOAT4;

	class Matrix;

	// ���Z�p�x�N�g���N���X
	class Vector
	{
		friend Matrix;
	public:
		//�f�t�H���g������
		//���ɉ����w�肵�Ȃ������ꍇ��x�N�g��
		Vector(); ~Vector();

		//�f�[�^�p�\���̂ŏ�����
		//2~4�����őΉ�
		Vector(const float2& data);
		Vector(const float3& data);
		Vector(const float4& data);

		//���l�ŏ�����
		//z,w�͏ȗ������ꍇ0�ł��邱�Ƃ�ۏ�
		Vector(const float _x, const float _y, const float _z = 0.f, const float _w = 0.f);

		//�R�s�[�ŏ�����
		//�Q�Ƃł͂Ȃ�mData�̎��̃R�s�[
		Vector(const Vector& other);

		Vector(const DirectX::XMVECTOR& other);

		//�f�[�^�p�\���̂��擾
		//���������p�ł͂Ȃ�
		const float2 GetFloat2() const;
		const float3 GetFloat3() const;
		const float4 GetFloat4() const;

		//�����A�N�Z�X
		//���������p�ł͂Ȃ�
		const float x() const;
		const float y() const;
		const float z() const;
		const float w() const;

		const float Vector2Length() const;
		const float Vector3Length() const;
		const float Vector4Length() const;

		//�e�퉉�Z
		// ��r���Z�q
		bool operator==(const Vector& other) const;
		// �R�s�[
		const Vector& operator=(const Vector& other);
		// �a
		const Vector operator+(const Vector& other) const;
		// ��
		const Vector operator-(const Vector& other) const;
		const Vector operator-()const;
		// �X�J����
		const Vector operator*(const float other) const;
		// �h�b�g��
		const float Dot2(const Vector& other) const;
		const float Dot3(const Vector& other) const;
		const float Dot4(const Vector& other) const;
		// �N���X��
		// 2�����x�N�g���̊O�� 3�����ȏオ���͂��ꂽ�ꍇ��z, w�͖�������� 
		// this.x * other.y - this.y * other.x //
		const float Cross2(const Vector& other) const;
		// 3�����x�N�g���̊O�ρ@
		// 4�����x�N�g�������͂��ꂽ�ꍇ�� w = 0.f 
		// 2�����x�N�g�������͂��ꂽ�ꍇ�́@(x, y, 0.f)�Ƃ��Ĉ���//
		const Vector Cross3(const Vector& other) const;

		const float AngleBetWeenVector2(const Vector& other) const;
		const float AngleBetWeenVector3(const Vector& other) const;


		// { 0.f, 0.f, 0.f, 0.f }
		static const Vector zero;
		// { 1.f, 0.f, 0.f, 0.f }
		static const Vector basicX;
		// { 0.f, 1.f, 0.f, 0.f }
		static const Vector basicY;
		// { 0.f, 0.f, 1.f, 0.f }
		static const Vector basicZ;
		// { 0.f, 0.f, 0.f, 1.f }
		static const Vector basicW;

		static Vector GenerateVectorNormalized(const float2& rawVec);
		static Vector GenerateVectorNormalized(const float3& rawVec);
		static Vector GenerateVectorNormalized(const float4& rawVec);

		static Vector GenerateVector3Transform(const Matrix& matrix, const Vector& vector);

		static Vector GenerateVectorLerp(const Vector a, const Vector b, const float t);

		static Vector GenerateRotationQuaternionSlerp(const Vector a, const Vector b, const float t);
		static Vector GenerateRotationQuaternionFromEuler(const float x, const float y, const float z);

	private:
		DirectX::XMVECTOR mData;
	};


	class Matrix
	{
		friend Vector;
	public:
		Matrix(); ~Matrix();
		Matrix(const Matrix& mat);
		Matrix(const DirectX::XMMATRIX& mat);
		Matrix(const Vector vectors[4]);

		const Matrix operator+(const Matrix& other) const;

		const Matrix operator*(const float other) const;
		const Matrix operator*(const Matrix& other) const;
		void operator*=(const Matrix& other);


		static Matrix GenerateMatrixIdentity();

		static Matrix GenerateMatrixInverse(const Matrix& matrix);

		static Matrix GenerateMatrixTranspose(const Matrix& matrix);

		static Matrix GenerateMatrixLookToLH
		(
			const Vector& eye,
			const Vector& eyeDir,
			const Vector& up
		);

		static Matrix GenerateMatrixLookAtLH
		(
			const Vector& eye,
			const Vector& target,
			const Vector& up
		);

		static Matrix GenerateMatrixPerspectiveFovLH
		(
			const float FovAngleY,
			const float AspectRatio,
			const float NearZ,
			const float FarZ
		);

		static Matrix GenerateMatrixTranslation(const Vector& position);

		static Matrix GenerateMatrixRotationX(const float angle);
		static Matrix GenerateMatrixRotationY(const float angle);
		static Matrix GenerateMatrixRotationZ(const float angle);
		static Matrix GenerateMatrixRotationQ(const Vector& q);
		static Matrix GenerateMatrixRotationAxis(const Vector& axis, float angle);
	private:
		DirectX::XMMATRIX mData;
	};

	//�G�ȏ�����r�֐��@
	bool FloatEqual(const float a, const float b);

	float RadianToDegree(float radian);
	float DegreeToRadian(float degree);
}

#endif // !_MATH_UTIL_H_


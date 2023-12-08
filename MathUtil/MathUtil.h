/*
	���w���C�u����

	Direct X Math����A���O�����B�؂����锖�����b�p�[���C�u����
	�ڐA���Ȃǂ͈�U�l���Ȃ�

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

	//�f�[�^�p

	using float2 = DirectX::XMFLOAT2;
	using float3 = DirectX::XMFLOAT3;
	using float4 = DirectX::XMFLOAT4;

	// ���Z�p�x�N�g���N���X
	class Vector
	{
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
		//�Q�Ƃł͂Ȃ�mData�̎��Ԃ��R�s�[
		Vector(const Vector& other);

		//�f�[�^�p�\���̂��擾
		//���������p�ł͂Ȃ�
		const float2 GetFloat2() const;
		const float3 GetFloat3() const;
		const float4 GetFloat4() const;

		const DirectX::XMVECTOR GetData() const;

		//�����A�N�Z�X
		//���������p�ł͂Ȃ�
		const float x() const;
		const float y() const;
		const float z() const;
		const float w() const;

		//�e�퉉�Z
		// ��r���Z�q
		bool operator==(const Vector& other) const;
		// �R�s�[
		const Vector& operator=(const Vector& other);
		// �a
		const Vector operator+(const Vector& other) const;
		// ��
		const Vector operator-(const Vector& other) const;
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

		static Vector GenerateRotationQuaternionFromEuler(float x, float y, float z);

	private:
		DirectX::XMVECTOR mData;
	};


	//�G�ȏ�����r�֐��@�e�X�g�L�q�p
	bool FloatEqual(const float a, const float b);

	float RadianToDegree(float radian);
	float DegreeToRadian(float degree);
}

#endif // !_MATH_UTIL_H_


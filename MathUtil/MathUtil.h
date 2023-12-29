/*
	数学ライブラリ

	Direct X Mathから、名前だけ隠ぺいする薄いラッパーライブラリ
	移植性などは一旦考えない

	必要なものだけラップする

*/
#ifndef _MATH_UTIL_H_
#define _MATH_UTIL_H_

#include<DirectXMath.h>

#ifdef _DEBUG
// ライブラリ固有の構造体をコンソール出力するマクロ
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

	//データ用
	using float2 = DirectX::XMFLOAT2;
	using float3 = DirectX::XMFLOAT3;
	using float4 = DirectX::XMFLOAT4;

	class Matrix;

	// 演算用ベクトルクラス
	class Vector
	{
		friend Matrix;
	public:
		//デフォルト初期化
		//特に何も指定しなかった場合零ベクトル
		Vector(); ~Vector();

		//データ用構造体で初期化
		//2~4次元で対応
		Vector(const float2& data);
		Vector(const float3& data);
		Vector(const float4& data);

		//数値で初期化
		//z,wは省略した場合0であることを保証
		Vector(const float _x, const float _y, const float _z = 0.f, const float _w = 0.f);

		//コピーで初期化
		//参照ではなくmDataの実体コピー
		Vector(const Vector& other);

		Vector(const DirectX::XMVECTOR& other);

		//データ用構造体を取得
		//書き換え用ではない
		const float2 GetFloat2() const;
		const float3 GetFloat3() const;
		const float4 GetFloat4() const;

		//成分アクセス
		//書き換え用ではない
		const float x() const;
		const float y() const;
		const float z() const;
		const float w() const;

		const float Vector2Length() const;
		const float Vector3Length() const;
		const float Vector4Length() const;

		void Vector2Normalize();
		void Vector3Normalize();
		void Vector4Normalize();

		Vector GetVector2Normalized() const;
		Vector GetVector3Normalized() const;
		Vector GetVector4Normalized() const;

		//各種演算
		// 比較演算子
		bool operator==(const Vector& other) const;
		// コピー
		const Vector& operator=(const Vector& other);
		// 和
		const Vector operator+(const Vector& other) const;
		// 差
		const Vector operator-(const Vector& other) const;
		const Vector operator-()const;
		// スカラ積
		const Vector operator*(const float other) const;
		// ドット積
		const float Dot2(const Vector& other) const;
		const float Dot3(const Vector& other) const;
		const float Dot4(const Vector& other) const;
		// クロス積
		// 2次元ベクトルの外積 3次元以上が入力された場合はz, wは無視される 
		// this.x * other.y - this.y * other.x //
		const float Cross2(const Vector& other) const;
		// 3次元ベクトルの外積　
		// 4次元ベクトルが入力された場合は w = 0.f 
		// 2次元ベクトルが入力された場合は　(x, y, 0.f)として扱う//
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

		// 単位すべてラジアン
		static Matrix GenerateMatrixRotationX(const float angle);
		static Matrix GenerateMatrixRotationY(const float angle);
		static Matrix GenerateMatrixRotationZ(const float angle);
		static Matrix GenerateMatrixRotationQ(const Vector& q);
		static Matrix GenerateMatrixRotationAxis(const Vector& axis, float angle);
	private:
		DirectX::XMMATRIX mData;
	};

	//雑な少数比較関数　
	bool FloatEqual(const float a, const float b);

	float RadianToDegree(float radian);
	float DegreeToRadian(float degree);
}

#endif // !_MATH_UTIL_H_


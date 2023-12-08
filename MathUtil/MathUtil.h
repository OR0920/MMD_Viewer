/*
	数学ライブラリ

	Direct X Mathから、名前だけ隠ぺいする薄いラッパーライブラリ
	移植性などは一旦考えない

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

	//データ用

	using float2 = DirectX::XMFLOAT2;
	using float3 = DirectX::XMFLOAT3;
	using float4 = DirectX::XMFLOAT4;

	// 演算用ベクトルクラス
	class Vector
	{
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
		//参照ではなくmDataの実態をコピー
		Vector(const Vector& other);

		//データ用構造体を取得
		//書き換え用ではない
		const float2 GetFloat2() const;
		const float3 GetFloat3() const;
		const float4 GetFloat4() const;

		const DirectX::XMVECTOR GetData() const;

		//成分アクセス
		//書き換え用ではない
		const float x() const;
		const float y() const;
		const float z() const;
		const float w() const;

		//各種演算
		// 比較演算子
		bool operator==(const Vector& other) const;
		// コピー
		const Vector& operator=(const Vector& other);
		// 和
		const Vector operator+(const Vector& other) const;
		// 差
		const Vector operator-(const Vector& other) const;
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

		static Vector GenerateRotationQuaternionFromEuler(float x, float y, float z);

	private:
		DirectX::XMVECTOR mData;
	};


	//雑な少数比較関数　テスト記述用
	bool FloatEqual(const float a, const float b);

	float RadianToDegree(float radian);
	float DegreeToRadian(float degree);
}

#endif // !_MATH_UTIL_H_


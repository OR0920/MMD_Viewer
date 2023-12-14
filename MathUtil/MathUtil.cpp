
#include"MathUtil.h"

using namespace DirectX;
using namespace MathUtil;

const float MathUtil::PI = XM_PI;
const float MathUtil::PI_DIV2 = XM_PIDIV2;
const float MathUtil::PI_DIV4 = XM_PIDIV4;
const float MathUtil::_2PI = XM_2PI;

// ベクトルクラスの実装

// コンストラクタ群・デストラクタ
Vector::Vector() : mData(XMVectorZero()) {}
Vector::Vector(const float2& data) : mData(XMLoadFloat2(reinterpret_cast<const XMFLOAT2*>(&data))) {}
Vector::Vector(const float3& data) : mData(XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&data))) {}
Vector::Vector(const float4& data) : mData(XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&data))) {}
Vector::Vector(const Vector& other) : mData(other.mData) {};
Vector::Vector(const DirectX::XMVECTOR& other) : mData(other) {};

Vector::Vector
(
	const float _x,
	const float _y,
	const float _z,
	const float _w
)
	:
	mData()
{
	//XMFLOAT4 data(_x, _y, _z, _w);
	mData = XMVectorSet(_x, _y, _z, _w);
}

Vector::~Vector() {}

// データ取得系
// 基本的に実装は一緒
const float2 Vector::GetFloat2() const
{
	float2 ret{};
	XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(&ret), mData);
	return ret;
}

const float3 Vector::GetFloat3() const
{
	float3 ret{};
	XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&ret), mData);
	return ret;
}

const float4 Vector::GetFloat4() const
{
	float4 ret{};
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&ret), mData);
	return ret;
}

const float Vector::x() const { return XMVectorGetX(mData); }
const float Vector::y() const { return XMVectorGetY(mData); }
const float Vector::z() const { return XMVectorGetZ(mData); }
const float Vector::w() const { return XMVectorGetW(mData); }

const float Vector::Vector2Length() const { return XMVectorGetX(XMVector2Length(mData)); }
const float Vector::Vector3Length() const { return XMVectorGetX(XMVector3Length(mData)); }
const float Vector::Vector4Length() const { return XMVectorGetX(XMVector4Length(mData)); }

// 演算
bool Vector::operator==(const Vector& other) const
{
	auto resultVec = XMVectorEqual(mData, other.mData);
	uint32_t result = 0;
	XMStoreInt(&result, resultVec);
	return static_cast<bool>(result);
}

const Vector& Vector::operator=(const Vector& other)
{
	mData = other.mData;
	return *this;
}

const Vector Vector::operator+(const Vector& other) const
{
	return XMVectorAdd(mData, other.mData);
}

const Vector Vector::operator-(const Vector& other) const
{
	return XMVectorSubtract(mData, other.mData);
}

const Vector Vector::operator-() const
{
	return -mData;
}

const Vector Vector::operator*(const float other) const
{
	return XMVectorScale(mData, other);
}

const float Vector::Dot2(const Vector& other) const
{
	return XMVectorGetX(XMVector2Dot(mData, other.mData));
}

const float Vector::Dot3(const Vector& other) const
{
	return XMVectorGetX(XMVector3Dot(mData, other.mData));
}

const float Vector::Dot4(const Vector& other) const
{
	return XMVectorGetX(XMVector4Dot(mData, other.mData));
}

const float Vector::Cross2(const Vector& other) const
{
	return XMVectorGetX(XMVector2Cross(mData, other.mData));
}

const Vector Vector::Cross3(const Vector& other) const
{
	return XMVector3Cross(mData, other.mData);
}

const float Vector::AngleBetWeenVector2(const Vector& other) const
{
	return	XMVectorGetX(XMVector2AngleBetweenVectors(mData, other.mData));
}

const float Vector::AngleBetWeenVector3(const Vector& other) const
{
	return XMVectorGetX(XMVector3AngleBetweenVectors(mData, other.mData));
}

// static メンバ　定数や生成メソッド

const Vector Vector::zero(0.f, 0.f, 0.f, 0.f);
const Vector Vector::basicX(1.f, 0.f, 0.f, 0.f);
const Vector Vector::basicY(0.f, 1.f, 0.f, 0.f);
const Vector Vector::basicZ(0.f, 0.f, 1.f, 0.f);
const Vector Vector::basicW(0.f, 0.f, 0.f, 1.f);

Vector Vector::GenerateVectorNormalized(const float2& rawVec)
{
	return XMVector2Normalize(XMLoadFloat2(&rawVec));
}

Vector Vector::GenerateVectorNormalized(const float3& rawVec)
{
	return XMVector3Normalize(XMLoadFloat3(&rawVec));
}

Vector Vector::GenerateVectorNormalized(const float4& rawVec)
{
	return XMVector4Normalize(XMLoadFloat4(&rawVec));
}

Vector Vector::GenerateVector3Transform(const Matrix& matrix, const Vector& vector)
{
	return XMVector3Transform(vector.mData, matrix.mData);
}

Vector Vector::GenerateVectorLerp(const Vector a, const Vector b, const float t)
{
	return XMVectorLerp(a.mData, b.mData, t);
}

Vector Vector::GenerateRotationQuaternionSlerp(const Vector a, const Vector b, const float t)
{
	return XMQuaternionSlerp(a.mData, b.mData, t);
}

Vector Vector::GenerateRotationQuaternionFromEuler(const float x, const float y, const float z)
{
	return XMQuaternionRotationRollPitchYaw
	(
		XMConvertToRadians(x),
		XMConvertToRadians(y),
		XMConvertToRadians(z)
	);
}



Matrix::Matrix()
	:
	mData()
{

}

Matrix::Matrix(const Matrix& matrix)
	:
	mData(matrix.mData)
{

}

Matrix::Matrix(const DirectX::XMMATRIX& matrix)
	:
	mData(matrix)
{

}

Matrix::Matrix(const Vector vectors[4])
	:
	mData(XMMatrixIdentity())
{
	for (int i = 0; i < 4; i++)
	{
		mData.r[i] = vectors[i].mData;
	}
}


Matrix::~Matrix()
{

}

const Matrix Matrix::operator+(const Matrix& other) const
{
	return mData + other.mData;
}

const Matrix Matrix::operator*(const float other) const
{
	return mData * other;
}

const Matrix Matrix::operator*(const Matrix& ohter) const
{
	return mData * ohter.mData;
}

void Matrix::operator*=(const Matrix& other)
{
	mData *= other.mData;
}

Matrix Matrix::GenerateMatrixIdentity()
{
	return  XMMatrixIdentity();
}

Matrix Matrix::GenerateMatrixInverse(const Matrix& matrix)
{
	return XMMatrixInverse(nullptr, matrix.mData);
}

Matrix Matrix::GenerateMatrixTranspose(const Matrix& matrix)
{
	return XMMatrixTranspose(matrix.mData);
}


Matrix Matrix::GenerateMatrixLookToLH(const Vector& eye, const Vector& eyeDir, const Vector& up)
{
	return XMMatrixLookToLH(eye.mData, eyeDir.mData, up.mData);
}

Matrix Matrix::GenerateMatrixLookAtLH(const Vector& eye, const Vector& target, const Vector& up)
{
	return XMMatrixLookAtLH(eye.mData, target.mData, up.mData);
}

Matrix Matrix::GenerateMatrixPerspectiveFovLH
(
	const float fovAngleY,
	const float aspectRatio,
	const float nearZ,
	const float farZ
)
{
	return XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
}

Matrix Matrix::GenerateMatrixTranslation(const Vector& position)
{
	return XMMatrixTranslationFromVector(position.mData);
}

Matrix Matrix::GenerateMatrixRotationX(const float angle)
{
	return XMMatrixRotationX(angle);
}

Matrix Matrix::GenerateMatrixRotationY(const float angle)
{
	return XMMatrixRotationY(angle);
}

Matrix Matrix::GenerateMatrixRotationZ(const float angle)
{
	return XMMatrixRotationZ(angle);
}

Matrix Matrix::GenerateMatrixRotationQ(const Vector& q)
{
	return XMMatrixRotationQuaternion(q.mData);
}

Matrix Matrix::GenerateMatrixRotationAxis(const Vector& axis, const float angle)
{
	return XMMatrixRotationAxis(axis.mData, angle);
}

bool MathUtil::FloatEqual(float a, float b)
{
	return fabs(a - b) <= 0.00001;
}

float MathUtil::RadianToDegree(float radian)
{
	return XMConvertToDegrees(radian);
}

float MathUtil::DegreeToRadian(float degree)
{
	return XMConvertToRadians(degree);
}
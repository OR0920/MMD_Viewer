
#include"MathUtil.h"

using namespace DirectX;
using namespace MathUtil;

// ベクトルクラスの実装

// コンストラクタ群・デストラクタ
Vector::Vector() : mData(XMVectorZero()) {}
Vector::Vector(const float2& data) : mData(XMLoadFloat2(reinterpret_cast<const XMFLOAT2*>(&data))) {}
Vector::Vector(const float3& data) : mData(XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&data))) {}
Vector::Vector(const float4& data) : mData(XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&data))) {}
Vector::Vector(const Vector& other) : mData(other.mData) {};

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

const DirectX::XMVECTOR Vector::GetData() const
{
	return mData;
}

const float Vector::x() const { return XMVectorGetX(mData); }
const float Vector::y() const { return XMVectorGetY(mData); }
const float Vector::z() const { return XMVectorGetZ(mData); }
const float Vector::w() const { return XMVectorGetW(mData); }

// 演算子オーバーロード
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
	Vector ret{};
	ret.mData = XMVectorAdd(mData, other.mData);
	return ret;
}

const Vector Vector::operator-(const Vector& other) const
{
	Vector ret{};
	ret.mData = XMVectorSubtract(mData, other.mData);
	return ret;
}

const Vector Vector::operator*(const float other) const
{
	Vector ret{};
	Vector otherv(other, other, other, other);
	ret.mData = XMVectorMultiply(mData, otherv.mData);
	return ret;
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
	Vector ret;
	ret.mData = XMVector3Cross(mData, other.mData);
	return ret;
}

Vector Vector::GenerateRotationQuaternionFromEuler(float x, float y, float z)
{
	Vector ret;
	ret.mData = XMQuaternionRotationRollPitchYaw
	(
		XMConvertToRadians(x),
		XMConvertToRadians(y),
		XMConvertToRadians(z)
	);
	return ret;
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


Matrix::~Matrix()
{

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
	return XMMatrixIdentity();
}

Matrix Matrix::GenerateMatrixInverse(const Matrix& matrix)
{
	return XMMatrixInverse(nullptr, matrix.mData);
}

Matrix Matrix::GenerateMatrixLookAtLH(const Vector& eye, const Vector& target, const Vector& up)
{
	return XMMatrixLookAtLH(eye.GetData(), target.GetData(), up.GetData());
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

Matrix Matrix::GenerateMatrixTranslation(const float3 position)
{
	return XMMatrixTranslation(position.x, position.y, position.z);
}

Matrix Matrix::GenerateMatrixRotationY(const float angle)
{
	return XMMatrixRotationY(angle);
}

Matrix Matrix::GenerateMatrixRotationZ(const float angle)
{
	return XMMatrixRotationZ(angle);
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
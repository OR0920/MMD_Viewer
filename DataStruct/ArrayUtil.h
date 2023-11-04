//
// Array.h
// 配列クラス
// 生配列に範囲外アクセスの確認をつけたもの
//

#ifndef _ARRAY_H_
#define _ARRAY_H_

#ifdef _DEBUG
#include<cassert>
#define IS_OUT_OF_RANGE(id, arrayCount)\
if(id < 0 || arrayCount <= id)\
{\
	assert(false && "Array Out Of Range ! ");\
}
#else
#define IS_OUT_OF_RANGE(arrayname, id, arrayCount)
#endif // _DEBUG

template<class T, size_t _Size>
class Array
{
public:
	Array() {}
	~Array() {}

	const size_t Size()
	{
		return mSize;
	}

	T& operator[](const size_t i)
	{
		IS_OUT_OF_RANGE(i, mSize);
		return data[i];
	}

	T** GetArrayAddress()
	{
		return &data;
	}

	const T** GetArrayAddress() const
	{
		return &data;
	}

	T* GetArray()
	{
		return data;
	}

	const T* GetArray() const
	{
		return data;
	}

private:
	T data[_Size] = {};
	const size_t mSize = _Size;
};

#endif // !_ARRAY_H_

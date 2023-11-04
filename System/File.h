//
// File.h
// ファイル入出力クラスを定義する
//

#ifndef _FILE_H_
#define _FILE_H_

#include<fstream>

namespace System
{
	// バイナリ書き出しクラス
	class FileWriteBin
	{
	public:
		FileWriteBin(const char* filepath);

		// ポインタ経由での書き出し
		// 標準ライブラリとほぼ変わらず
		// 使用例
		// T v;
		// instance.Write(&v);
		template <class T> void Write(T* data)
		{
			mFile.write(reinterpret_cast<char*>(data), sizeof(T));
		}

		// 参照経由での書き出し
		// 変数のまま渡せる
		// 使用例
		// T v;
		// instance.Write(v); 
		template <class T> void Write(T& data)
		{
			mFile.write(reinterpret_cast<char*>(&data), sizeof(T));
		}

		// 配列の書き出し 要素数を指定する 
		// 使用例
		// T array[arrSize];
		// instance.Write(array, arrSize);
		template <class T> void WriteArray(T* arr, int arrSize)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.write(reinterpret_cast<char*>(&arr[i]), sizeof(T));
			}
		}

		// true : 成功, false : 失敗
		bool IsFileOpenSuccsess() const;

		// 明示的にファイルを閉じる
		// 同一のスコープ内で、
		// 他ファイル系クラスのインスタンスとファイルを共有する場合に使用する
		void Close();

	private:
		std::ofstream mFile;
	};

	// バイナリ読み出しクラス
	class FileReadBin
	{
	public:
		FileReadBin(const char* filepath);

		// ポインタ経由での読み込み 
		// 標準ライブラリとほぼ同じ
		// 使用例
		// T v;
		// instance.Read(&v) //
		template<class T> void Read(T* data)
		{
			mFile.read(reinterpret_cast<char*>(data), sizeof(T));
		}

		// 参照経由での読み込み
		// 変数をそのまま渡せばいい
		// 使用例
		// T v;
		// instance.Read(v) //
		template<class T> void Read(T& data)
		{
			mFile.read(reinterpret_cast<char*>(&data), sizeof(T));
		}

		// バイトサイズを指定したポインタ経由での読み込み
		// フォーマットで指定された型と、受け取る型が異なる場合に利用する 
		// 使用例
		// int v;
		// instance.Read(&v, sizeof(unsigned char)) //
		template<class T> void Read(T* data, size_t size)
		{
			mFile.read(reinterpret_cast<char*>(data), size);
		}

		// バイトサイズを指定した参照経由での読み込み
		// 上と用途は変わらず　
		// 使用例
		// int v;
		// instance.Read(v, sizeof(unsigned char)) //
		template<class T> void Read(T& data, size_t size)
		{
			mFile.read(reinterpret_cast<char*>(&data), size);
		}

		// 配列の読み込み
		// 使用例
		// T array[arrSize];
		// instance.Read(array, arrSize) //
		template <class T> void ReadArray(T* arr, int arrSize)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.read(reinterpret_cast<char*>(&arr[i]), sizeof(T));
			}
		}

		// バイトサイズを指定した配列の読み込み
		// 配列変数、配列要素数、1要素のバイトサイズを指定する
		// 使用例
		// int array[arrSize];
		// instance.Read(array, arrSize, sizeof(unsigned char)) //
		template <class T> void ReadArray(T* arr, int arrSize, size_t size)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.read(reinterpret_cast<char*>(&arr[i]), size);
			}
		}

		//説明略
		bool IsFileOpenSuccsess() const;
		void Close();

	private:
		std::ifstream mFile;
	};
}

#endif // !_FILE_H_

//
// File.h
// �t�@�C�����o�̓N���X���`����
//

#ifndef _FILE_H_
#define _FILE_H_

#include<fstream>

namespace System
{
	// �o�C�i�������o���N���X
	class FileWriteBin
	{
	public:
		FileWriteBin(const char* filepath);

		// �|�C���^�o�R�ł̏����o��
		// �W�����C�u�����Ƃقڕς�炸
		// �g�p��
		// T v;
		// instance.Write(&v);
		template <class T> void Write(T* data)
		{
			mFile.write(reinterpret_cast<char*>(data), sizeof(T));
		}

		// �Q�ƌo�R�ł̏����o��
		// �ϐ��̂܂ܓn����
		// �g�p��
		// T v;
		// instance.Write(v); 
		template <class T> void Write(T& data)
		{
			mFile.write(reinterpret_cast<char*>(&data), sizeof(T));
		}

		// �z��̏����o�� �v�f�����w�肷�� 
		// �g�p��
		// T array[arrSize];
		// instance.Write(array, arrSize);
		template <class T> void WriteArray(T* arr, int arrSize)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.write(reinterpret_cast<char*>(&arr[i]), sizeof(T));
			}
		}

		// true : ����, false : ���s
		bool IsFileOpenSuccsess() const;

		// �����I�Ƀt�@�C�������
		// ����̃X�R�[�v���ŁA
		// ���t�@�C���n�N���X�̃C���X�^���X�ƃt�@�C�������L����ꍇ�Ɏg�p����
		void Close();

	private:
		std::ofstream mFile;
	};

	// �o�C�i���ǂݏo���N���X
	class FileReadBin
	{
	public:
		FileReadBin(const char* filepath);

		// �|�C���^�o�R�ł̓ǂݍ��� 
		// �W�����C�u�����Ƃقړ���
		// �g�p��
		// T v;
		// instance.Read(&v) //
		template<class T> void Read(T* data)
		{
			mFile.read(reinterpret_cast<char*>(data), sizeof(T));
		}

		// �Q�ƌo�R�ł̓ǂݍ���
		// �ϐ������̂܂ܓn���΂���
		// �g�p��
		// T v;
		// instance.Read(v) //
		template<class T> void Read(T& data)
		{
			mFile.read(reinterpret_cast<char*>(&data), sizeof(T));
		}

		// �o�C�g�T�C�Y���w�肵���|�C���^�o�R�ł̓ǂݍ���
		// �t�H�[�}�b�g�Ŏw�肳�ꂽ�^�ƁA�󂯎��^���قȂ�ꍇ�ɗ��p���� 
		// �g�p��
		// int v;
		// instance.Read(&v, sizeof(unsigned char)) //
		template<class T> void Read(T* data, size_t size)
		{
			mFile.read(reinterpret_cast<char*>(data), size);
		}

		// �o�C�g�T�C�Y���w�肵���Q�ƌo�R�ł̓ǂݍ���
		// ��Ɨp�r�͕ς�炸�@
		// �g�p��
		// int v;
		// instance.Read(v, sizeof(unsigned char)) //
		template<class T> void Read(T& data, size_t size)
		{
			mFile.read(reinterpret_cast<char*>(&data), size);
		}

		// �z��̓ǂݍ���
		// �g�p��
		// T array[arrSize];
		// instance.Read(array, arrSize) //
		template <class T> void ReadArray(T* arr, int arrSize)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.read(reinterpret_cast<char*>(&arr[i]), sizeof(T));
			}
		}

		// �o�C�g�T�C�Y���w�肵���z��̓ǂݍ���
		// �z��ϐ��A�z��v�f���A1�v�f�̃o�C�g�T�C�Y���w�肷��
		// �g�p��
		// int array[arrSize];
		// instance.Read(array, arrSize, sizeof(unsigned char)) //
		template <class T> void ReadArray(T* arr, int arrSize, size_t size)
		{
			for (int i = 0; i < arrSize; ++i)
			{
				mFile.read(reinterpret_cast<char*>(&arr[i]), size);
			}
		}

		//������
		bool IsFileOpenSuccsess() const;
		void Close();

	private:
		std::ifstream mFile;
	};
}

#endif // !_FILE_H_

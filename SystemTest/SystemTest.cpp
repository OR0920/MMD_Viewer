#include "pch.h"
#include "CppUnitTest.h"

#include"System.h"

#include<string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace System;

namespace SystemTest
{
	TEST_CLASS(SystemTest)
	{
	public:
		TEST_METHOD(CopyDirectoryPathTest)
		{
			const char* filepath = "TestRoot/TestDir/TestFilePath.file";
			char* dirpath = nullptr;

			CopyDirectoryPath(&dirpath, filepath);
			
			std::string dirpathStr(dirpath);

			Assert::IsTrue(dirpathStr == "TestRoot/TestDir/");

			SafeDeleteArray(&dirpath);
		}
	};

	TEST_CLASS(FileIOTest)
	{
		const char* testFilePath = "Test/test.test";
		struct TestStruct
		{
			char c;
			unsigned char uc;
			short s;
			unsigned short us;
			int i;
			unsigned int ui;
			float f;
			double d;
		};

		TEST_METHOD(open)
		{
			FileWriteBin fw(testFilePath);
			
			Assert::IsTrue(fw.IsFileOpenSuccsess() == true);

			fw.Close();

			FileReadBin fr(testFilePath);

			Assert::IsTrue(fr.IsFileOpenSuccsess() == true);

			fr.Close();

			{
				FileWriteBin fw2(testFilePath);
				Assert::IsTrue(fw2.IsFileOpenSuccsess() == true);
			}
			{
				FileReadBin fr2(testFilePath);
				Assert::IsTrue(fr2.IsFileOpenSuccsess() == true);
			}
		}

		TEST_METHOD(FileWriteAndRead)
		{
			char charTestData = '#';
			unsigned char uCharTestData = 3;
			short shortTestData = -3524;
			unsigned short uShortTestData = 324;
			int intTestData = -58394;
			unsigned int uIntTestData = 53489;
			float floatTestData = 534.f;
			double doubleTestData = 2483.53534;
			
			TestStruct testData =
			{
				charTestData,
				uCharTestData,
				shortTestData,
				uShortTestData,
				intTestData,
				uIntTestData,
				floatTestData,
				doubleTestData
			};

			FileWriteBin fw(testFilePath);
			fw.Write(&testData.c);
			fw.Write(&testData.uc);
			fw.Write(&testData.s);
			fw.Write(&testData.us);
			fw.Write(&testData.i);
			fw.Write(&testData.ui);
			fw.Write(&testData.f);
			fw.Write(&testData.d);

			fw.Close();

			testData = {};

			FileReadBin fr(testFilePath);
			fr.Read(&testData.c);
			fr.Read(&testData.uc);
			fr.Read(&testData.s);
			fr.Read(&testData.us);
			fr.Read(&testData.i);
			fr.Read(&testData.ui);
			fr.Read(&testData.f);
			fr.Read(&testData.d);

			fr.Close();

			Assert::IsTrue(testData.c == charTestData);
			Assert::IsTrue(testData.uc == uCharTestData);
			Assert::IsTrue(testData.s == shortTestData);
			Assert::IsTrue(testData.us == uShortTestData);
			Assert::IsTrue(testData.i == intTestData);
			Assert::IsTrue(testData.ui == uIntTestData);
			Assert::IsTrue(testData.f == floatTestData);
			Assert::IsTrue(testData.d == doubleTestData);
		}

		TEST_METHOD(FileWriteAndReadWithRef)
		{
			char charTestData = '#';
			unsigned char uCharTestData = 3;
			short shortTestData = -3524;
			unsigned short uShortTestData = 324;
			int intTestData = -58394;
			unsigned int uIntTestData = 53489;
			float floatTestData = 534.f;
			double doubleTestData = 2483.53534;

			TestStruct testData =
			{
				charTestData,
				uCharTestData,
				shortTestData,
				uShortTestData,
				intTestData,
				uIntTestData,
				floatTestData,
				doubleTestData
			};

			FileWriteBin fw(testFilePath);
			fw.Write(testData.c);
			fw.Write(testData.uc);
			fw.Write(testData.s);
			fw.Write(testData.us);
			fw.Write(testData.i);
			fw.Write(testData.ui);
			fw.Write(testData.f);
			fw.Write(testData.d);

			fw.Close();

			testData = {};

			FileReadBin fr(testFilePath);
			fr.Read(testData.c);
			fr.Read(testData.uc);
			fr.Read(testData.s);
			fr.Read(testData.us);
			fr.Read(testData.i);
			fr.Read(testData.ui);
			fr.Read(testData.f);
			fr.Read(testData.d);

			fr.Close();

			Assert::IsTrue(testData.c == charTestData);
			Assert::IsTrue(testData.uc == uCharTestData);
			Assert::IsTrue(testData.s == shortTestData);
			Assert::IsTrue(testData.us == uShortTestData);
			Assert::IsTrue(testData.i == intTestData);
			Assert::IsTrue(testData.ui == uIntTestData);
			Assert::IsTrue(testData.f == floatTestData);
			Assert::IsTrue(testData.d == doubleTestData);
		}


		TEST_METHOD(FileWriteAndReadWithSize)
		{
			char charTestData = '#';
			unsigned char uCharTestData = 3;
			short shortTestData = -3524;
			unsigned short uShortTestData = 324;
			int intTestData = -58394;
			unsigned int uIntTestData = 53489;
			float floatTestData = 534.f;
			double doubleTestData = 2483.53534;

			TestStruct testData =
			{
				charTestData,
				uCharTestData,
				shortTestData,
				uShortTestData,
				intTestData,
				uIntTestData,
				floatTestData,
				doubleTestData
			};

			FileWriteBin fw(testFilePath);
			fw.Write(testData.c);
			fw.Write(testData.uc);
			fw.Write(testData.s);
			fw.Write(testData.us);
			fw.Write(testData.i);
			fw.Write(testData.ui);
			fw.Write(testData.f);
			fw.Write(testData.d);

			fw.Close();

			testData = {};

			long long veryLargeVariable[6] = {};

			FileReadBin fr(testFilePath);
			fr.Read(&veryLargeVariable[0], sizeof(char));
			fr.Read(&veryLargeVariable[1], sizeof(unsigned char));
			fr.Read(&veryLargeVariable[2], sizeof(short));
			fr.Read(&veryLargeVariable[3], sizeof(unsigned short));
			fr.Read(&veryLargeVariable[4], sizeof(int));
			fr.Read(&veryLargeVariable[5], sizeof(unsigned int));
			fr.Read(testData.f);
			fr.Read(testData.d);

			fr.Close();

			Assert::IsTrue(static_cast<char>(veryLargeVariable[0])== charTestData);
			Assert::IsTrue(static_cast<unsigned char>(veryLargeVariable[1]) == uCharTestData);
			Assert::IsTrue(static_cast<short>(veryLargeVariable[2]) == shortTestData);
			Assert::IsTrue(static_cast<unsigned short>(veryLargeVariable[3])== uShortTestData);
			Assert::IsTrue(static_cast<int>(veryLargeVariable[4])== intTestData);
			Assert::IsTrue(static_cast<unsigned int>(veryLargeVariable[5])== uIntTestData);
			Assert::IsTrue(testData.f == floatTestData);
			Assert::IsTrue(testData.d == doubleTestData);
		}


		TEST_METHOD(FileWriteAndReadWithSizeAndRef)
		{
			char charTestData = '#';
			unsigned char uCharTestData = 3;
			short shortTestData = -3524;
			unsigned short uShortTestData = 324;
			int intTestData = -58394;
			unsigned int uIntTestData = 53489;
			float floatTestData = 534.f;
			double doubleTestData = 2483.53534;

			TestStruct testData =
			{
				charTestData,
				uCharTestData,
				shortTestData,
				uShortTestData,
				intTestData,
				uIntTestData,
				floatTestData,
				doubleTestData
			};

			FileWriteBin fw(testFilePath);
			fw.Write(testData.c);
			fw.Write(testData.uc);
			fw.Write(testData.s);
			fw.Write(testData.us);
			fw.Write(testData.i);
			fw.Write(testData.ui);
			fw.Write(testData.f);
			fw.Write(testData.d);

			fw.Close();

			testData = {};

			long long veryLargeVariable[6] = {};

			FileReadBin fr(testFilePath);
			fr.Read(veryLargeVariable[0], sizeof(char));
			fr.Read(veryLargeVariable[1], sizeof(unsigned char));
			fr.Read(veryLargeVariable[2], sizeof(short));
			fr.Read(veryLargeVariable[3], sizeof(unsigned short));
			fr.Read(veryLargeVariable[4], sizeof(int));
			fr.Read(veryLargeVariable[5], sizeof(unsigned int));
			fr.Read(testData.f);
			fr.Read(testData.d);

			fr.Close();

			Assert::IsTrue(static_cast<char>(veryLargeVariable[0]) == charTestData);
			Assert::IsTrue(static_cast<unsigned char>(veryLargeVariable[1]) == uCharTestData);
			Assert::IsTrue(static_cast<short>(veryLargeVariable[2]) == shortTestData);
			Assert::IsTrue(static_cast<unsigned short>(veryLargeVariable[3]) == uShortTestData);
			Assert::IsTrue(static_cast<int>(veryLargeVariable[4]) == intTestData);
			Assert::IsTrue(static_cast<unsigned int>(veryLargeVariable[5]) == uIntTestData);
			Assert::IsTrue(testData.f == floatTestData);
			Assert::IsTrue(testData.d == doubleTestData);
		}

		TEST_METHOD(ReadArray)
		{
			int arr[100] = {};
			int arrSize = sizeof(arr)/sizeof(int);
			for (int i = 0; i < arrSize; ++i)
			{
				arr[i] = rand();
			}

			FileWriteBin fw("Test/ReadArray.test");
			fw.Write(arrSize);
			fw.WriteArray(arr, arrSize);

			fw.Close();

			FileReadBin fr("Test/ReadArray.test");
			int rArrSize;
			fr.Read(rArrSize);

			int* rArr = new int[rArrSize];
			fr.ReadArray(rArr, rArrSize);

			Assert::IsTrue(rArrSize == arrSize);
			for (int i = 0; i < rArrSize; ++i)
			{
				Assert::IsTrue(rArr[i] == arr[i]);
			}

			delete[] rArr;
			rArr = nullptr;
		}

		TEST_METHOD(ReadArrayWithSize)
		{
			int arr[100] = {};
			int arrSize = sizeof(arr) / sizeof(int);
			for (int i = 0; i < arrSize; ++i)
			{
				arr[i] = rand();
			}

			FileWriteBin fw("Test/ReadArray.test");
			fw.Write(arrSize);
			fw.WriteArray(arr, arrSize);

			fw.Close();

			FileReadBin fr("Test/ReadArray.test");
			int rArrSize;
			fr.Read(rArrSize);

			long* rArr = new long[rArrSize];
			fr.ReadArray(rArr, rArrSize, sizeof(int));

			Assert::IsTrue(rArrSize == arrSize);
			for (int i = 0; i < rArrSize; ++i)
			{
				Assert::IsTrue(rArr[i] == arr[i]);
			}

			delete[] rArr;
			rArr = nullptr;
		}
		// TODO 
		// 配列　サイズ指定　ポインタで受け取り
		// //
	};
}

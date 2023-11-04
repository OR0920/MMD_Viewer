#include "pch.h"
#include "CppUnitTest.h"

#include"ArrayUtil.h"

#include<array>

std::array<int, 5> a;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DataStructTest
{
	TEST_CLASS(ArrayTest)
	{
	public:

		TEST_METHOD(Init)
		{
			static const int arraySize = 5;
			Array<int, arraySize> test;

			Assert::IsTrue(test.Size() == arraySize);

			for (int i = 0; i < test.Size(); ++i)
			{
				Assert::IsTrue(test[i] == 0);
			}


			//test[arraySize];‚Å—áŠO‚ª”­¶‚·‚é‚±‚Æ‚ðŠm”F

			Array<char, 10> test2;
			Assert::IsTrue(test2.Size() == 10);

			for (int i = 0; i < test2.Size(); ++i)
			{
				Assert::IsTrue(test2[i] == '\0');
			}
		}

		TEST_METHOD(Accsess)
		{

		}
	};
}

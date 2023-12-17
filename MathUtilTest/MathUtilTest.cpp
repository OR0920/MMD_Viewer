#include "pch.h"
#include "CppUnitTest.h"
#include"MathUtil.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace MathUtil;

namespace MathUtilTest
{
	TEST_CLASS(MathUtilTest)
	{
	public:
		TEST_METHOD(FloatEqualAsIntTest)
		{
			Assert::IsFalse(FloatEqual(1.f, 1.4f));
			Assert::IsTrue(FloatEqual(0.000001f, 0.000001f));
		}
	};

	TEST_CLASS(VectorTest)
	{
	public:
		TEST_METHOD(VectorInit)
		{
			Vector vec = {};
			Assert::IsTrue(vec.GetFloat4().x == 0.f);
			Assert::IsTrue(vec.GetFloat4().y == 0.f);
			Assert::IsTrue(vec.GetFloat4().z == 0.f);
			Assert::IsTrue(vec.GetFloat4().w == 0.f);

			float3 initValueA = { 3.f, 4.f, 5.f };
			Vector vecA(initValueA);

			Assert::IsTrue(vecA.GetFloat3().x == 3.f);
			Assert::IsTrue(vecA.GetFloat3().y == 4.f);
			Assert::IsTrue(vecA.GetFloat3().z == 5.f);
			Assert::IsTrue(vecA.GetFloat4().w == 0.f);

			float3 initValueB = { 5.f, 10.f, 300.f };
			Vector vecB(initValueB);

			Assert::IsTrue(vecB.GetFloat3().x == 5.f);
			Assert::IsTrue(vecB.GetFloat3().y == 10.f);
			Assert::IsTrue(vecB.GetFloat3().z == 300.f);

			float2 initValueC = { 1.f, 5.f };
			Vector vecC(initValueC);

			Assert::IsTrue(vecC.GetFloat2().x == 1.f);
			Assert::IsTrue(vecC.GetFloat2().y == 5.f);

			float2 initValueD = { 3.f, 45.f };
			Vector vecD(initValueD);

			Assert::IsTrue(vecD.GetFloat2().x == 3.f);
			Assert::IsTrue(vecD.GetFloat2().y == 45.f);

			float4 initValueE = { 4.f, 5.f, 2.f, 4.f };
			Vector vecE(initValueE);

			Assert::IsTrue(vecE.GetFloat4().x == 4.f);
			Assert::IsTrue(vecE.GetFloat4().y == 5.f);
			Assert::IsTrue(vecE.GetFloat4().z == 2.f);
			Assert::IsTrue(vecE.GetFloat4().w == 4.f);

			float4 initValueF = { 44.f, 5.f, 523.f, 64.f };
			Vector vecF(initValueF);

			Assert::IsTrue(vecF.GetFloat4().x == 44.f);
			Assert::IsTrue(vecF.GetFloat4().y == 5.f);
			Assert::IsTrue(vecF.GetFloat4().z == 523.f);
			Assert::IsTrue(vecF.GetFloat4().w == 64.f);

			Vector vecG(3.f, 4.f);

			Assert::IsTrue(vecG.GetFloat2().x == 3.f);
			Assert::IsTrue(vecG.GetFloat2().y == 4.f);
			Assert::IsTrue(vecG.GetFloat4().z == 0.f);
			Assert::IsTrue(vecG.GetFloat4().w == 0.f);

			Vector vecH(3.f, 4.f, 5.f);

			Assert::IsTrue(vecH.GetFloat3().x == 3.f);
			Assert::IsTrue(vecH.GetFloat3().y == 4.f);
			Assert::IsTrue(vecH.GetFloat3().z == 5.f);
			Assert::IsTrue(vecH.GetFloat4().w == 0.f);

			Vector vecI(2.f, 5.f, 3.f, 8.f);
			Assert::IsTrue(vecI.x() == 2.f);
			Assert::IsTrue(vecI.y() == 5.f);
			Assert::IsTrue(vecI.z() == 3.f);
			Assert::IsTrue(vecI.w() == 8.f);

		}

		TEST_METHOD(Length)
		{
			Vector v2 = { 3.f, 4.f };
			Assert::IsTrue(FloatEqual(v2.Vector2Length(), 5.f));
			Assert::IsTrue(FloatEqual(v2.Vector3Length(), 5.f));
			Assert::IsTrue(FloatEqual(v2.Vector4Length(), 5.f));

			Vector v3 = { 3.f, 4.f, 5.f };
			Assert::IsTrue(FloatEqual(v3.Vector2Length(), 5.f));
			Assert::IsTrue(FloatEqual(v3.Vector3Length(), 7.0710678f));
			Assert::IsTrue(FloatEqual(v3.Vector4Length(), 7.0710678f));

			Vector v4 = { 3.f, 4.f, 5.f, 6.f };
			Assert::IsTrue(FloatEqual(v4.Vector2Length(), 5.f));
			Assert::IsTrue(FloatEqual(v4.Vector3Length(), 7.0710678f));
			Assert::IsTrue(FloatEqual(v4.Vector4Length(), 9.2736184f));
		}


		TEST_METHOD(Equal)
		{
			Vector a(3.f, 4.f, 5.f);
			float3 bInit = { 3.f, 4.f, 5.f };
			Vector b(bInit);

			Assert::IsTrue(a == b);

			Vector c(4.f, 5.f, 2.f);

			Assert::IsFalse(a == c);
		}

		TEST_METHOD(copy)
		{
			Vector a(3.f, 4.f, 5.f);
			Vector b = {};
			b = a;

			Assert::IsTrue(b == a);

			Vector c(2.f, 4.f, 5.f);
			a = c;

			//aが変更されてもbが変更されていない
			//参照がコピーされていないことの保証
			Assert::IsFalse(b == a);

			Vector d = c;

			Assert::IsTrue(d == c);

			c = b;

			//cが変更されてもdが変更されていない
			//参照がコピーされていないことの保証
			Assert::IsFalse(d == c);

		}

		TEST_METHOD(Add)
		{
			{
				Vector a(3.f, 4.f, 5.f), b(2.f, 5.f, 1.f);
				Vector c = a + b;
				Vector ans(5.f, 9.f, 6.f);
				Assert::IsTrue(c == ans);

				Vector d(1.f, 1.f, 1.f);
				Vector e = a + d;
				Vector ans2(4.f, 5.f, 6.f);
				Assert::IsTrue(e == ans2);

				Vector f = a + b + d;
				Vector ans3(6.f, 10.f, 7.f);
				Assert::IsTrue(f == ans3);
			}
			{
				Vector a(3.f, 4.f, 5.f, 6.f), b(2.f, 5.f, 1.f, 3.f);
				Vector c = a + b;
				Vector ans(5.f, 9.f, 6.f, 9.f);
				Assert::IsTrue(c == ans);

				Vector d(1.f, 1.f, 1.f, 1.f);
				Vector e = a + d;
				Vector ans2(4.f, 5.f, 6.f, 7.f);
				Assert::IsTrue(e == ans2);

				Vector f = a + b + d;
				Vector ans3(6.f, 10.f, 7.f, 10.f);
				Assert::IsTrue(f == ans3);
			}
		}

		TEST_METHOD(Sub)
		{
			{
				Vector a(3.f, 4.f, 5.f), b(2.f, 5.f, 1.f);
				Vector c = a - b;
				Vector ans(1.f, -1.f, 4.f);
				Assert::IsTrue(c == ans);

				Vector d(1.f, 1.f, 1.f);
				Vector e = a - d;
				Vector ans2(2.f, 3.f, 4.f);
				Assert::IsTrue(e == ans2);

				Vector f = a - b - d;
				Vector ans3(0.f, -2.f, 3.f);
				Assert::IsTrue(f == ans3);
			}
			{
				Vector a(3.f, 4.f, 5.f, 6.f), b(2.f, 5.f, 1.f, 3.f);
				Vector c = a - b;
				Vector ans(1.f, -1.f, 4.f, 3.f);
				Assert::IsTrue(c == ans);

				Vector d(1.f, 1.f, 1.f, 1.f);
				Vector e = a - d;
				Vector ans2(2.f, 3.f, 4.f, 5.f);
				Assert::IsTrue(e == ans2);

				Vector f = a - b - d;
				Vector ans3(0.f, -2.f, 3.f, 2.f);
				Assert::IsTrue(f == ans3);
			}
		}

		TEST_METHOD(ScalaMul)
		{
			{
				Vector a(3.f, 4.f, 5.f);
				float s = 2.f;
				Vector as = a * s;
				Vector ans(6.f, 8.f, 10.f);

				Assert::IsTrue(ans == as);

				float s2 = 0.5f;
				Vector as2 = a * s2;
				Vector ans2(1.5f, 2.f, 2.5f);

				Assert::IsTrue(ans2 == as2);
			}
			{
				Vector a(3.f, 4.f, 5.f, 6.f);
				float s = 2.f;
				Vector as = a * s;
				Vector ans(6.f, 8.f, 10.f, 12.f);

				Assert::IsTrue(ans == as);

				float s2 = 0.5f;
				Vector as2 = a * s2;
				Vector ans2(1.5f, 2.f, 2.5f, 3.f);

				Assert::IsTrue(ans2 == as2);
			}
		}

		TEST_METHOD(Dot2)
		{
			Vector a(2.f, 6.f), b(4.f, 1.f);
			auto aDotb = a.Dot2(b);
			auto ans = 14.f;
			Assert::IsTrue(aDotb == ans);

			auto bDota = b.Dot2(a);
			Assert::IsTrue(bDota == aDotb);

			Vector c(5.f, 3.f);
			auto aDotc = a.Dot2(c);
			auto ans2 = 28.f;
			Assert::IsTrue(aDotc == ans2);

			Vector d(3.f, 6.f, 1.f, 6.f);
			auto aDotd = a.Dot2(d);
			auto ans3 = 42.f;
			Assert::IsTrue(aDotd == ans3);
		}

		TEST_METHOD(Dot3)
		{
			Vector a(3.f, 4.f, 5.f), b(6.f, 1.f, 3.f);
			float aDotb = a.Dot3(b);
			float ans = 37.f;
			Assert::IsTrue(aDotb == ans);

			float bDota = b.Dot3(a);
			Assert::IsTrue(aDotb == bDota);

			Vector c(1.f, 2.f, 5.f);
			float aDotc = a.Dot3(c);
			float ans2 = 36.f;
			Assert::IsTrue(aDotc == ans2);
		}

		TEST_METHOD(Dot4)
		{
			Vector a(8.f, 5.f, 3.f, 5.f), b(2.f, 5.f, 9.f, 3.f);
			float aDotb = a.Dot4(b);

			float ans = 83.f;
			Assert::IsTrue(aDotb == ans);

			float bDota = b.Dot4(a);
			Assert::IsTrue(aDotb == bDota);

			Vector c(3.f, 1.f, 2.f, 5.f);
			float aDotc = a.Dot4(c);
			float ans2 = 60.f;
			Assert::IsTrue(aDotc == ans2);

			float aDotb_3d = a.Dot3(b);
			float ans3 = 68.f;
			Assert::IsTrue(aDotb_3d == ans3);
		}

		TEST_METHOD(Cross2)
		{
			Vector a(2.f, 3.f), b(3.f, 9.f);
			auto aCrossB = a.Cross2(b);
			auto ans = 9.f;
			Assert::IsTrue(aCrossB == ans);
			auto bCrossA = b.Cross2(a);
			auto ans2 = -9.f;
			Assert::IsTrue(bCrossA == ans2);
		}

		TEST_METHOD(Cross3)
		{
			Vector a(3.f, 6.f, 1.f);
			Vector b(3.f, 5.f, 4.f);
			auto aCrossB = a.Cross3(b);
			Vector ans(19.f, -9.f, -3.f);
			Assert::IsTrue(aCrossB == ans);

			auto bCrossA = b.Cross3(a);
			Vector ans2(-19.f, 9.f, 3.f);
			Assert::IsTrue(bCrossA == ans2);

			Vector b2(3.f, 5.f);
			auto aCrossB2 = a.Cross3(b2);
			Vector ans3(-5.f, 3.f, -3.f);
			Assert::IsTrue(aCrossB2 == ans3);

			Vector b4(3.f, 5.f, 4.f, 9999.f);
			auto aCrossB4 = a.Cross3(b4);
			Assert::IsTrue(aCrossB == aCrossB4);
			Assert::IsTrue(aCrossB4.w() == 0.f);

		}

		TEST_METHOD(AngleBetween)
		{
			Vector a = { 1.f, 0.f, 0.f };
			Vector b = { 0.f, 1.f, 0.f };
			Assert::IsTrue(FloatEqual(a.AngleBetWeenVector2(b), DegreeToRadian(90.f)));
			Assert::IsTrue(FloatEqual(a.AngleBetWeenVector3(b), DegreeToRadian(90.f)));
			Vector c = { 0.f, 0.f, 1.f };
			Assert::IsTrue(FloatEqual(a.AngleBetWeenVector3(c), DegreeToRadian(90.f)));
		}

		TEST_METHOD(_Normalize2)
		{
			float2 f = { 3.f, 4.f }, xf{};
			auto v = Vector::GenerateVectorNormalized(f);

			auto xv = DirectX::XMVector2Normalize(DirectX::XMVectorSet(3.f, 4.f, 0.f, 0.f));
			DirectX::XMStoreFloat2(&xf, xv);

			Assert::IsTrue(v == xf);

			Vector v2 = f;
			v2.Vector2Normalize();

			Assert::IsTrue(v2 == xf);

			Vector v3 = f;

			Assert::IsTrue(v3 == f);

			auto v4 = v3.GetVector2Normalized();
			Assert::IsTrue(v4 == xf);
			Assert::IsTrue(v3 == f);
		}

		TEST_METHOD(_Normalize3)
		{
			float3 f = { 3.f, 4.f, 5.f }, xf{};
			auto v = Vector::GenerateVectorNormalized(f);

			auto xv = DirectX::XMVector3Normalize(DirectX::XMVectorSet(3.f, 4.f, 5.f, 0.f));
			DirectX::XMStoreFloat3(&xf, xv);

			Assert::IsTrue(v == xf);

			Vector v2 = f;
			v2.Vector3Normalize();

			Assert::IsTrue(v2 == xf);

			Vector v3 = f;

			Assert::IsTrue(v3 == f);

			auto v4 = v3.GetVector3Normalized();
			Assert::IsTrue(v4 == xf);
			Assert::IsTrue(v3 == f);
		}

		TEST_METHOD(_Normalize4)
		{
			float4 f = { 3.f, 4.f, 5.f , 6.f }, xf{};
			auto v = Vector::GenerateVectorNormalized(f);

			auto xv = DirectX::XMVector4Normalize(DirectX::XMVectorSet(3.f, 4.f, 5.f, 6.f));
			DirectX::XMStoreFloat4(&xf, xv);

			Assert::IsTrue(v == xf);

			Vector v2 = f;
			v2.Vector4Normalize();

			Assert::IsTrue(v2 == xf);

			Vector v3 = f;

			auto v4 = v3.GetVector4Normalized();
			Assert::IsTrue(v4 == xf);
			Assert::IsTrue(v3 == f);
		}

	};

}

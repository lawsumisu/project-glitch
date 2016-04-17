#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/VectorUtility.h"
#include "../PGlitch_CPP/MathUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CustomUtilities;


namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<Vector2f>(const Vector2f& v)
            {
                RETURN_WIDE_STRING(toString(v).c_str());
            }
        }
    }
}

namespace PGlitchTest
{		
    static const float pi = 4 * atanf(1);
	TEST_CLASS(VectorUtilityTest)
	{
        
	public:
		TEST_METHOD(testMagnitude)
		{
            Vector2f v = Vector2f(3, 4);
            Vector2f w = Vector2f(-3, -4);

            Assert::AreEqual((float)5, magnitude(v));
            Assert::AreEqual((float)5, magnitude(w));
		}

        TEST_METHOD(testNormalize) {
            Vector2f v = Vector2f(3, 4);

            Assert::AreEqual(.6f, normalize(v).x);
            Assert::AreEqual(.8f, normalize(v).y);
            Assert::AreEqual((float)1, magnitude(normalize(v)));
        }

        TEST_METHOD(testDot) {
            Vector2f v1 = Vector2f(3, 4);
            Vector2f v2 = Vector2f(5, 6);

            Assert::AreEqual((float)39, dot(v1, v2));
            Assert::AreEqual(magnitude(v1) * magnitude(v1), dot(v1, v1));
        }

        TEST_METHOD(testAngle) {
            Vector2f v1 = Vector2f(1, 0);
            Vector2f v2 = Vector2f(0, 1);
            
            Assert::AreEqual(pi / 2, angle(v1, v2));
            Assert::AreEqual(0.f, angle(v1, v1));

        }

        TEST_METHOD(testAngleNegative) {
            Vector2f v1 = Vector2f(1, 0);
            
            Assert::AreEqual(pi, angle(v1, v1 * -1.f));
        }
	};

    TEST_CLASS(MathUtilityTest) {
        TEST_METHOD(testSign) {
            Assert::AreEqual(-1, sign(-5.f));
            Assert::AreEqual(1, sign(133.2f));
            Assert::AreEqual(0, sign(0));
        }
        TEST_METHOD(testLinearRegressionHorizontal) {
            std::vector<Vector2f> points = {};
            points.push_back(Vector2f(1, 5));
            points.push_back(Vector2f(2, 5));
            points.push_back(Vector2f(3, 5));
            points.push_back(Vector2f(4, 5));
            points.push_back(Vector2f(5, 5));

            Vector2f output = linearRegression(points);

            Assert::AreEqual(5.f, output.x);
            Assert::AreEqual(0.f, output.y);
        }
        TEST_METHOD(testLinearRegression) {
            std::vector<Vector2f> points = {};
            points.push_back(Vector2f(49,124));
            points.push_back(Vector2f(69, 95));
            points.push_back(Vector2f(89, 75));
            points.push_back(Vector2f(99, 45));
            points.push_back(Vector2f(109, 18));

            Vector2f output = linearRegression(points);

            Assert::IsTrue(abs(211.212f - output.x) <= 0.001f);
            Assert::IsTrue(abs(-1.68448f -  output.y) <= 0.001f);        
        }

   
    };
}
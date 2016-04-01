#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/VectorUtility.h"
#include "../PGlitch_CPP/MathUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CustomUtilities;

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

            Assert::AreEqual((float)5, VectorUtility::magnitude(v));
            Assert::AreEqual((float)5, VectorUtility::magnitude(w));
		}

        TEST_METHOD(testNormalize) {
            Vector2f v = Vector2f(3, 4);

            Assert::AreEqual(.6f, VectorUtility::normalize(v).x);
            Assert::AreEqual(.8f, VectorUtility::normalize(v).y);
            Assert::AreEqual((float)1, VectorUtility::magnitude(VectorUtility::normalize(v)));
        }

        TEST_METHOD(testDot) {
            Vector2f v1 = Vector2f(3, 4);
            Vector2f v2 = Vector2f(5, 6);

            Assert::AreEqual((float)39, VectorUtility::dot(v1, v2));
            Assert::AreEqual(VectorUtility::magnitude(v1) * VectorUtility::magnitude(v1), VectorUtility::dot(v1, v1));
        }

        TEST_METHOD(testAngle) {
            Vector2f v1 = Vector2f(1, 0);
            Vector2f v2 = Vector2f(0, 1);
            
            Assert::AreEqual(pi / 2, VectorUtility::angle(v1, v2));
            Assert::AreEqual(0.f, VectorUtility::angle(v1, v1));

        }

        TEST_METHOD(testAngleNegative) {
            Vector2f v1 = Vector2f(1, 0);
            
            Assert::AreEqual(pi, VectorUtility::angle(v1, v1 * -1.f));
        }
	};

    TEST_CLASS(MathUtilityTest) {
        TEST_METHOD(testLinearRegressionHorizontal) {
            std::vector<Vector2f> points = {};
            points.push_back(Vector2f(1, 5));
            points.push_back(Vector2f(2, 5));
            points.push_back(Vector2f(3, 5));
            points.push_back(Vector2f(4, 5));
            points.push_back(Vector2f(5, 5));

            Vector2f output = MathUtility::linearRegression(points);

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

            Vector2f output = MathUtility::linearRegression(points);

            Assert::IsTrue(abs(211.212f - output.x) <= 0.001f);
            Assert::IsTrue(abs(-1.68448f -  output.y) <= 0.001f);        
        }
    };
}
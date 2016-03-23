#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/VectorUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CustomUtilities;

namespace PGlitchTest
{		
    static const float pi = 4 * atan(1);
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
}
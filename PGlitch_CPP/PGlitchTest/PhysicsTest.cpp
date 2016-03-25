#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/PillarCollider.h"
#include "../PGlitch_CPP/RectUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sf;
using namespace CustomUtilities;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<FloatRect>(const FloatRect& R)
            {
                RETURN_WIDE_STRING(RectUtility::toString(R).c_str());
            }
        }
    }
}

namespace PGlitchTest
{		
	TEST_CLASS(PillarColliderTest)
	{
	public:
		TEST_METHOD(testIntersection)
		{
            FloatRect r1 = FloatRect(0.f, 0.f, 6.f, 6.f);
            FloatRect r2 = FloatRect(3.f, 0.f, 4.f, 2.f);

            Assert::IsTrue(r1.intersects(r2));
		}

        TEST_METHOD(testIntersectionRectLine) {
            FloatRect r1 = FloatRect(0.f, 0.f, 6.f, 6.f);
            FloatRect r2 = FloatRect(4.f, 0.f, 4.f, 0.01f);
            FloatRect r3 = FloatRect();

            Assert::IsTrue(r1.intersects(r2));

        }

        TEST_METHOD(testIntersectsIndividual) {
            PillarCollider MC = PillarCollider::uniformDepth({ -2,-4,-6 }, 1, 0, Vector2f(0, 0));
            vector<FloatRect> actual = MC.intersects(FloatRect(0, -5, 1, 5));

            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(0, -2, 1, 2), actual[0]);

            actual = MC.intersects(FloatRect(1, -5, 1, 5));
            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(1, -4, 1, 4), actual[0]);

            actual = MC.intersects(FloatRect(2, -5, 1, 5));
            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(2, -5, 1, 5), actual[0]);
        }

        TEST_METHOD(testIntersectsOverlap) {
            PillarCollider collider = PillarCollider::uniformDepth({ -2,-4,-6 }, 1, 0, Vector2f(0, 0));
            vector<FloatRect> actual = collider.intersects(FloatRect(0.5, -5, 1, 5));

            Assert::AreEqual(2U, actual.size());
            Assert::AreEqual(FloatRect(.5, -2, .5, 2), actual[0]);
            Assert::AreEqual(FloatRect(1, -4, .5, 4), actual[1]);
        }

        TEST_METHOD(testIntersectsNone) {
            PillarCollider collider = PillarCollider::uniformDepth({ -2,-4,-6 }, 1, 0, Vector2f(0, 0));

            vector<FloatRect> above = collider.intersects(FloatRect(0, -10, 3, 2));
            Assert::AreEqual(0U, above.size());

            vector<FloatRect> below = collider.intersects(FloatRect(0, 0, 3, 2));
            Assert::AreEqual(0U, below.size());

            vector<FloatRect> left = collider.intersects(FloatRect(-3, -4, 3, 2));
            Assert::AreEqual(0U, left.size());

            vector<FloatRect> right = collider.intersects(FloatRect(3, -4, 3, 2));
            Assert::AreEqual(0U, right.size());

        }
	};
}
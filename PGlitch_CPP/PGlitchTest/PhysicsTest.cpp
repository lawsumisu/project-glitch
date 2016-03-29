#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/PillarCollider.h"
#include "../PGlitch_CPP/RectUtility.h"
#include "../PGlitch_CPP/SensorCollider.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sf;
using namespace CustomUtilities;
using namespace Physics;

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

            /*actual = MC.intersects(FloatRect(1, -5, 1, 5));
            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(1, -4, 1, 4), actual[0]);

            actual = MC.intersects(FloatRect(2, -5, 1, 5));
            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(2, -5, 1, 5), actual[0]);*/
        }

        TEST_METHOD(testIntersectsOverlap) {
            PillarCollider collider = PillarCollider::uniformDepth({ -2,-4,-6 }, 1, 0, Vector2f(0, 0));
            vector<FloatRect> actual = collider.intersects(FloatRect(0.5, -5, 1, 5));

            Assert::AreEqual(2U, actual.size());
            Assert::AreEqual(FloatRect(.5, -2, .5, 2), actual[0]);
            Assert::AreEqual(FloatRect(1, -4, .5, 4), actual[1]);
        }

        TEST_METHOD(testIntersectsInner) {
            PillarCollider collider = PillarCollider::uniformDepth({ -2,-4,-6 }, 10, 0, Vector2f(0, 0));
            vector<FloatRect> actual = collider.intersects(FloatRect(11, -5, 5, 5));

            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(11, -4, 5, 4), actual[0]);
        }

        TEST_METHOD(testIntersectsOverlapY) {
            PillarCollider collider = PillarCollider::uniformDepth({ -2,-4,-6 }, 10, 0, Vector2f(0, 0));
            vector<FloatRect> actual = collider.intersects(FloatRect(11, -7, 5, 10));

            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(11, -4, 5, 4), actual[0]);
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

        TEST_METHOD(testIntersectsNonZeroOrigin) {
            PillarCollider MC = PillarCollider::uniformDepth({ -2,-4,-6 }, 1, 0, Vector2f(5, 5));
            vector<FloatRect> actual = MC.intersects(FloatRect(5, -1, 1, 5));

            Assert::AreEqual(1U, actual.size());
            Assert::AreEqual(FloatRect(5, 3, 1, 1), actual[0]);
        }
	};

    TEST_CLASS(SensorColliderTest) {
    public:
        TEST_METHOD(testCollision) {
            SensorCollider sensor = SensorCollider(Vector2f(100,100), Vector2f(16,5), Vector2f(32,48));
            PillarCollider ground = PillarCollider::uniformDepth({ -2 }, 400, 0, Vector2f(0, 120));
            Collision collision = sensor.collides(ground);
            
            Assert::IsTrue(collision.withGround());
            Assert::IsFalse(collision.withLeft());

        }

        TEST_METHOD(testCollisionHorizontal) {
            SensorCollider sensor = SensorCollider(Vector2f(100, 100), Vector2f(16, 5), Vector2f(32, 48));
            PillarCollider left = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(85, 120));
            PillarCollider right = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(150, 120));
            Collision collision = sensor.collides(left);

            Assert::IsTrue(collision.withLeft());
            Assert::IsFalse(sensor.collides(right).withRight());
        }

        TEST_METHOD(testCollisionMultiple) {
            SensorCollider sensor = SensorCollider(Vector2f(100, 100), Vector2f(16, 5), Vector2f(32, 48));
            PillarCollider left = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(85, 120));
            PillarCollider ground = PillarCollider::uniformDepth({ -2 }, 400, 0, Vector2f(0, 120));
            PillarCollider right = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(150, 120));
            Collision collision = sensor.collides(vector<PillarCollider>{ ground, left, right });
            Collision collision2 = sensor.collides(vector<PillarCollider>{ground});

            Logger::WriteMessage(collision.toString().c_str());
            Logger::WriteMessage(collision2.toString().c_str());
            Assert::IsTrue(collision.withGround());
            Assert::IsFalse(collision.withRight());
            //Assert::IsFalse(collision.withLeft());

        }
    };
}
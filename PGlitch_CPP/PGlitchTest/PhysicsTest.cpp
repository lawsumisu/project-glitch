#include "stdafx.h"
#include "CppUnitTest.h"
#include <SFML/Graphics/Rect.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sf;

namespace PGlitchTest
{		
	TEST_CLASS(RectTest)
	{
	public:
		TEST_METHOD(testIntersection)
		{
            FloatRect r1 = FloatRect(0.f, 0.f, 6.f, 6.f);
            FloatRect r2 = FloatRect(3.f, 0.f, 4.f, 2.f);

            Assert::IsTrue(r1.intersects(r2));
		}

        TEST_METHOD(testIntersectionRectLine) {
            FloatRect r1 = FloatRect(0.f, 0.f, 4.f, 4.f);
            FloatRect r2 = FloatRect(4.f, 0.f, 4.f, 4.f);
            FloatRect r3 = FloatRect();

            Assert::IsFalse(r1.intersects(r2));
            r1.intersects(r2, r3);
            Assert::AreEqual(0.f, r3.width*r3.height);
        }
	};
}
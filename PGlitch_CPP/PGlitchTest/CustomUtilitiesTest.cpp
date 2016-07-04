#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/VectorUtility.h"
#include "../PGlitch_CPP/MathUtility.h"
#include "../PGlitch_CPP/Segment.h"
#include "../PGlitch_CPP/RectUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CustomUtilities;
using namespace std;


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

            template<> static std::wstring ToString<FloatRect>(const FloatRect& r)
            {
                RETURN_WIDE_STRING(toString(r).c_str());
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

        TEST_METHOD(testToPoint) {
            Assert::AreEqual(Vector2f(sqrtf(2)/2, sqrtf(2)/2), toPoint(1.f, pi / 4));
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

            Logger::WriteMessage(toString(output).c_str());
            Assert::IsTrue(abs(211.212f - output.x) <= 0.001f);
            Assert::IsTrue(abs(-1.68448f -  output.y) <= 0.001f);        
        }

        TEST_METHOD(testClampOutsideBounds) {
            Assert::AreEqual(2.f, clamp(1.f, 2.f, 3.f));
            Assert::AreEqual(3.f, clamp(4.f, 2.f, 3.f));
        }

        TEST_METHOD(testClampAtBounds) {
            Assert::AreEqual(2.f, clamp(2.f, 2.f, 3.f));
            Assert::AreEqual(-2.f, clamp(-2.f, -3.f, -2.f));
        }

        TEST_METHOD(testClampWithinBounds) {
            Assert::AreEqual(2.f, clamp(2.f, 1.f, 3.f));
        }

        TEST_METHOD(testClampUnorderedBounds) {
            Assert::AreEqual(clamp(2.f, 0.f, 1.f), clamp(2.f, 1.f, 0.f));
            Assert::AreEqual(1.f, clamp(2.f, 0.f, 1.f));
        }

        TEST_METHOD(testClampSameBounds) {
            Assert::AreEqual(0.f, clamp(0.f, 0.f, 0.f));
            Assert::AreEqual(0.f, clamp(-1.f, 0.f, 0.f));
            Assert::AreEqual(0.f, clamp(1.f, 0.f, 0.f));
        }

   
    };

    TEST_CLASS(LineUtilityTest) {
        TEST_METHOD(testToString) {
            Segment l = Segment(Vector2f(), Vector2f(1, 1));

            Assert::AreEqual("Line:[(0,0) -> (1,1)]", l.toString().c_str());

            Segment l2 = Segment(Vector2f(), pi / 2, 3);

            Logger::WriteMessage(l2.toString().c_str());
        }

        TEST_METHOD(testIntersects) {
            Segment l1 = Segment(Vector2f(), Vector2f(1, 1));
            Segment l2 = Segment(Vector2f(-1, 1), Vector2f());

            std::pair<bool, float> intersection = l1.intersects(l2);

            Assert::IsTrue(intersection.first);
            Assert::AreEqual(0.f, intersection.second);
        }

        TEST_METHOD(testIntersectsNonAxisAligned) {
            Segment l1 = Segment(Vector2f(), Vector2f(1, 1));
            Segment l2 = Segment(Vector2f(1, 0), Vector2f(0,1));

            std::pair<bool, float> intersection = l1.intersects(l2);

            Assert::IsTrue(intersection.first);
            Assert::AreEqual(.5f, intersection.second);
        }

        TEST_METHOD(testIntersectsFalse) {
            Segment l1 = Segment(Vector2f(), Vector2f(1, 1));
            Segment l2 = Segment(Vector2f(.5f, 0), 0, 3);

            std::pair<bool, float> intersection = l1.intersects(l2);

            Assert::IsFalse(intersection.first);
        }

        TEST_METHOD(testAtPoint) {
            Segment line = Segment(Vector2f(), Vector2f(1, 1));

            Assert::AreEqual(Vector2f(.5f, .5f), line.atPoint(.5f));
        }

        TEST_METHOD(testGetT) {
            Segment line = Segment(Vector2f(), Vector2f(1, 1));

            Assert::AreEqual(1.f, line.getT(Vector2f(1, 1)));
            Assert::AreEqual(0.f, line.getT(Vector2f(0, 0)));
            Assert::AreEqual(.5f, line.getT(Vector2f(.5f, .5f)));
        }

        TEST_METHOD(testGetTAxisAligned) {
            Segment line = Segment(Vector2f(), Vector2f(0, 5));

            Assert::AreEqual(.2f, line.getT(Vector2f(0, 1)));
        }

        TEST_METHOD(testIntersectsRect) {
            Segment line = Segment(Vector2f(0, 2), Vector2f(5, 2));
            FloatRect rect = FloatRect(0, 0, 4, 4);

            Assert::IsTrue(line.intersects(rect).first);
            Assert::AreEqual(0.f, line.intersects(rect).second);

            FloatRect rect2 = FloatRect(2, 0, 4, 4);
            Assert::IsTrue(line.intersects(rect2).first);
            Assert::AreEqual(Vector2f(2, 2), line.atPoint(line.intersects(rect2).second));
        }

        TEST_METHOD(testIntersectRectNot) {
            Segment line = Segment(Vector2f(0, 2), Vector2f(5, 2));
            FloatRect rect = FloatRect(6, 0, 4, 4);

            Assert::IsFalse(line.intersects(rect).first);
        }

        TEST_METHOD(testInnerLine) {
            Segment line(Vector2f(0, 2), Vector2f(5, 2));
            FloatRect rect(3, -2, 4, 5);

            Assert::IsTrue(line.findInnerLine(rect).first);
            Logger::WriteMessage(line.findInnerLine(rect).second.toString().c_str());
            Assert::AreEqual(.6f, line.intersects(line.findInnerLine(rect).second).second);
        }

        TEST_METHOD(testInnerLineCollinear) {
            Segment line(Vector2f(0, 0), Vector2f(5, 0));
            FloatRect rect(2, 0, 2, 2);

            pair<bool, Segment> innerLine = line.findInnerLine(rect);
            Assert::IsTrue(innerLine.first);
            Assert::AreEqual(Vector2f(2, 0), innerLine.second.start());
            Assert::AreEqual(Vector2f(4, 0), innerLine.second.end());
        }

        TEST_METHOD(testInnerLineExterior) {
            Segment line(Vector2f(0, 2), Vector2f(10, 2));
            FloatRect rect(3, -2, 4, 5);

            Assert::IsTrue(line.findInnerLine(rect).first);
            Logger::WriteMessage(line.findInnerLine(rect).second.toString().c_str());
            //Assert::AreEqual(.6f, line.intersects(line.findInnerLine(rect).second).second);
        }
    };

}
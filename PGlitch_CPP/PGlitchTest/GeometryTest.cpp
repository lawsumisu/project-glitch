#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/Polygon.h"
#include "../PGlitch_CPP/VectorUtility.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace sf;
using namespace std;
using namespace CustomUtilities;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<Vector2f>(const Vector2f& v) {
                RETURN_WIDE_STRING(toString(v).c_str());
            }

            template<> static std::wstring ToString<Segment>(const Segment& s) {
                RETURN_WIDE_STRING(s.toString().c_str());
            }
        }
    }
}

namespace PGlitchTest
{		
	TEST_CLASS(PolygonTest)
	{
	public:
		TEST_METHOD(testInnerLine)
		{
            vector<Vector2f> points = { Vector2f(0,0), Vector2f(1,0), Vector2f(.5, .5f) };
            Polygon triangle(points);
            Segment outerLine(Vector2f(0, .1f), Vector2f(.25f, .1f));

            pair<bool, Segment> innerLine = triangle.findInnerLine(outerLine);
            Assert::IsTrue(innerLine.first);
            Logger::WriteMessage(innerLine.second.toString().c_str());
		}

        TEST_METHOD(testFindInnerLineCompletelyContained) {
            Polygon rect(FloatRect(0, 0, 5, 5));
            Segment outerLine(Vector2f(1, 1), Vector2f(3, 3));

            pair<bool, Segment> innerLine = rect.findInnerLine(outerLine);
            Assert::IsTrue(innerLine.first);
            Assert::AreEqual(outerLine, innerLine.second);
        }

        TEST_METHOD(testFindInnerLineCompareToSegment) {
            vector<Vector2f> points = { Vector2f(), Vector2f(1,1), Vector2f(2,0) };
            FloatRect rect(0, 0, 5, 5);
            Polygon pRect(rect);
            Segment outerSegment(Vector2f(-1, -1), Vector2f(5, 6));

            pair<bool, Segment> innerLine = pRect.findInnerLine(outerSegment);
            Assert::IsTrue(innerLine.first);

            pair<bool, Segment> innerLine2 = outerSegment.findInnerLine(rect);
            Assert::IsTrue(innerLine2.first);
            Logger::WriteMessage(innerLine.second.toString().c_str());
            Logger::WriteMessage(innerLine2.second.toString().c_str());
        }

        TEST_METHOD(testTransform) {
            Polygon rect(FloatRect(0, 0, 5, 6));
            Segment line(Vector2f(2, -2), Vector2f(2, 2));
  
            Polygon transformedRect = rect.transform(Transform().rotate(90, Vector2f(2.5f, 3)));
            pair<bool, Segment> innerLine = transformedRect.findInnerLine(line);
            Assert::IsTrue(innerLine.first);
            Assert::AreEqual(Vector2f(2, .5f), innerLine.second.start());
            Assert::AreEqual(Vector2f(2, 2), innerLine.second.end());
        }
	};
}
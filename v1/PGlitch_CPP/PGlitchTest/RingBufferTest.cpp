#include "stdafx.h"
#include "CppUnitTest.h"
#include <iostream>
#include "../PGlitch_CPP/RingBuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PGlitchTest
{
    // WindowArray Test
    //
    // Partitions:
    // add()    -> added elements == size; added elements >= size
    // []        -> i >= 0; i < 0                                    //
    //                                                                //
    TEST_CLASS(RingBufferTest)
    {
    public:

        TEST_METHOD(testAdd)
        {
            RingBuffer<int> A =  RingBuffer<int>(3);
            A.add(1);
            A.add(2);
            A.add(3);

            Assert::AreEqual(1, A[0]);
            Assert::AreEqual(2, A[1]);
            Assert::AreEqual(3, A[2]);
        }

        TEST_METHOD(testAddWindowing) {
            RingBuffer<int> A = RingBuffer<int>(2);
            A.add(1);
            A.add(2);
            A.add(3);

            Assert::AreEqual(2, A[0]);
            Assert::AreEqual(3, A[1]);
        }

        TEST_METHOD(testNegativeIndexing) {
            RingBuffer<int> A = RingBuffer<int>(5);
            for (int i = 1; i <= 10; ++i) {
                A.add(i);
                Logger::WriteMessage(A.toString().c_str());

                Assert::AreEqual(i, A[-1]);
            }
               
        }

        TEST_METHOD(testNegativeIndexingNotAtCapacity) {
            RingBuffer<int> A = RingBuffer<int>(10);
            A.add(1);
            A.add(2);

            Assert::IsFalse(A.atCapacity());
            Assert::AreEqual(2, A[-1]);
            Assert::AreEqual(2, A[1]);
            Assert::AreEqual(1, A[-2]);
        }

        TEST_METHOD(testLength) {
            RingBuffer<int> A = RingBuffer<int>(3);
            A.add(1);
            A.add(2);

            Assert::IsFalse(A.atCapacity());
            Assert::AreEqual(2U, A.length());
            A.add(3);
            Assert::IsTrue(A.atCapacity());
            Assert::AreEqual(A.capacity(), A.length());
        }

        TEST_METHOD(testCapacity) {
            RingBuffer<int> A = RingBuffer<int>(2);
            RingBuffer<int> B = RingBuffer<int>(1);

            Assert::AreEqual(2U, A.capacity());
            Assert::AreEqual(1U, B.capacity());
        }

        TEST_METHOD(testToString) {
            RingBuffer<int> A = RingBuffer<int>(3);
            A.add(1);
            A.add(2);

            Assert::AreEqual("RingBuffer!@capacity:<1,2>", A.toString().c_str());
            A.add(3);
            Assert::AreEqual("RingBuffer@capacity:<1,2,3>", A.toString().c_str());
            A.add(4);
            Assert::AreEqual("RingBuffer@capacity:<2,3,4>", A.toString().c_str());
            A.add(5);
            Assert::AreEqual("RingBuffer@capacity:<3,4,5>", A.toString().c_str());
            A.add(6);
            Assert::AreEqual("RingBuffer@capacity:<4,5,6>", A.toString().c_str());
        }
    };
}
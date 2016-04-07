#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/FrameInput.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<PlayerInput>(const PlayerInput& input)
            {
                RETURN_WIDE_STRING(input.toString().c_str());
            }
        }
    }
}

namespace PGlitchTest
{		
    TEST_CLASS(PlayerInputTest) {
    public:
        TEST_METHOD(testEquals) {
            PlayerInput pi1 = PlayerInput(InputCode::RIGHT, InputType::PRESS);
            PlayerInput pi2 = PlayerInput(InputCode::LEFT, InputType::PRESS);
            PlayerInput pi3 = PlayerInput(InputCode::LEFT, InputType::HOLD);
            PlayerInput pi4 = PlayerInput(InputCode::RIGHT, InputType::PRESS);

            Assert::AreEqual(pi1, pi1);
            Assert::AreNotEqual(pi1, pi2);
            Assert::AreNotEqual(pi2, pi3);
            Assert::AreEqual(pi1, pi4);
        }

        TEST_METHOD(testEqualsNone) {
            PlayerInput pi1 = PlayerInput(InputCode::RIGHT, InputType::PRESS);

            Assert::AreEqual(PlayerInput::none(), PlayerInput::none());
            Assert::AreNotEqual(PlayerInput::none(), pi1);
        }

        TEST_METHOD(testToString) {
            PlayerInput pi1 = PlayerInput(InputCode::RIGHT, InputType::PRESS);
            PlayerInput pi2 = PlayerInput(InputCode::DOWN, InputType::RELEASE);
            PlayerInput pi3 = PlayerInput(InputCode::LEFT, InputType::HOLD);

            Assert::AreEqual("R(100)", pi1.toString().c_str());
            Assert::AreEqual("[-]D(100)", pi2.toString().c_str());
            Assert::AreEqual("[+]L(100)", pi3.toString().c_str());
        }

        TEST_METHOD(testRetype) {
            PlayerInput pi1 = PlayerInput(InputCode::RIGHT, InputType::PRESS);

            Assert::AreEqual(PlayerInput(InputCode::RIGHT, InputType::HOLD), pi1.retype(InputType::HOLD));
            Assert::AreEqual(3, pi1.retype(InputType::HOLD).retype(InputType::HOLD).duration());
            Assert::AreEqual("[+2]R(100)", pi1.retype(InputType::HOLD).retype(InputType::HOLD).toString().c_str());
        }

        TEST_METHOD(testRetune) {
            PlayerInput pi = PlayerInput(InputCode::RIGHT, InputType::PRESS, 100.f);

            Assert::AreEqual(PlayerInput(InputCode::RIGHT, InputType::PRESS, 5.f), pi.retune(5.f));
        }
    };

    TEST_CLASS(FrameInputTest)
	{
	public:
		TEST_METHOD(testEmpty)
		{
            FrameInput empty = FrameInput();

            Assert::AreEqual(0U, empty.count());
            Assert::IsTrue(empty.contains(PlayerInput::none()));
		}

        TEST_METHOD(testAdd) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::AreEqual(1U, FI.count());
            Assert::IsFalse(FI.contains(PlayerInput::none()));
        }

        TEST_METHOD(testAddChangeStrength) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS, 5.f));
            FI.add(PlayerInput(InputCode::LEFT, InputType::HOLD, 10.f));
            
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS, 10.f));
            FI.add(PlayerInput(InputCode::LEFT, InputType::HOLD, 5.f));

            Logger::WriteMessage(FI.toString().c_str());
            Assert::AreEqual(2U, FI.count());
            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS, 10.f)));
            Assert::IsTrue(FI.contains(PlayerInput(InputCode::LEFT, InputType::HOLD, 10.f)));
        }

        TEST_METHOD(testAddChangeType) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::RELEASE));
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::AreEqual(1U, FI.count());
            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS)));

            FI.add(PlayerInput(InputCode::RIGHT, InputType::HOLD));
            FI.add(PlayerInput(InputCode::RIGHT, InputType::RELEASE));
            Assert::AreEqual(1U, FI.count());
            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::HOLD)));
        }
        TEST_METHOD(testAddNone) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            FI.add(PlayerInput::none());

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS))); 
            Assert::AreEqual(1U, FI.count());
        }

        TEST_METHOD(testContainsIgnoreStrength) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS)));
            Assert::IsFalse(FI.contains(PlayerInput(InputCode::LEFT, InputType::PRESS)));
        }

        TEST_METHOD(testContainsIgnoreStrengthRepeatAdd) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS)));
            Assert::AreEqual(1U, FI.count());
        }

        TEST_METHOD(testIsDown) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS, 10.f));
            FI.add(PlayerInput(InputCode::DOWN, InputType::HOLD, 50.f));
            FI.add(PlayerInput(InputCode::UP, InputType::RELEASE, 0.f));

            Assert::IsTrue(FI.isDown(InputCode::RIGHT));
            Assert::IsTrue(FI.isDown(InputCode::DOWN));
            Assert::IsFalse(FI.isDown(InputCode::UP));
        }

        TEST_METHOD(testContains) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS, 5.f));

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS, 5.f)));
            Assert::IsFalse(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS, 5.1f)));
            Assert::IsFalse(FI.contains(PlayerInput::none()));
        }

        TEST_METHOD(testModifyWithPreviousHold) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            
            FrameInput previous = FrameInput();
            previous.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            FrameInput modified = FI.modifyWithPrevious(previous);

            Assert::IsTrue(modified.containsIgnoreStrength(PlayerInput(InputCode::RIGHT, InputType::HOLD)));
            Assert::IsFalse(modified.containsIgnoreStrength(PlayerInput(InputCode::RIGHT, InputType::PRESS)));
        }

        TEST_METHOD(testModifyWithPreviousRelease) {
            FrameInput previous = FrameInput();
            previous.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            FrameInput modified = FrameInput().modifyWithPrevious(previous);

            Assert::AreEqual(1U, modified.count());
            Assert::IsTrue(modified.containsIgnoreStrength(PlayerInput(InputCode::RIGHT, InputType::RELEASE)));
        }
	};
}
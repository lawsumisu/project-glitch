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

            Assert::AreEqual("R", pi1.toString().c_str());
            Assert::AreEqual("^D", pi2.toString().c_str());
            Assert::AreEqual("_L", pi3.toString().c_str());
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

        TEST_METHOD(testAddNone) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            FI.add(PlayerInput::none());

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS))); 
            Assert::AreEqual(1U, FI.count());
        }

        TEST_METHOD(testContains) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS)));
            Assert::IsFalse(FI.contains(PlayerInput(InputCode::LEFT, InputType::PRESS)));
        }

        TEST_METHOD(testContainsRepeatAdd) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));

            Assert::IsTrue(FI.contains(PlayerInput(InputCode::RIGHT, InputType::PRESS)));
            Assert::AreEqual(1U, FI.count());
        }

        TEST_METHOD(testIsDown) {
            FrameInput FI = FrameInput();
            FI.add(PlayerInput(InputCode::RIGHT, InputType::PRESS));
            FI.add(PlayerInput(InputCode::DOWN, InputType::HOLD));
            FI.add(PlayerInput(InputCode::UP, InputType::RELEASE));

            Assert::IsTrue(FI.isDown(InputCode::RIGHT));
            Assert::IsTrue(FI.isDown(InputCode::DOWN));
            Assert::IsFalse(FI.isDown(InputCode::UP));
        }
	};
}
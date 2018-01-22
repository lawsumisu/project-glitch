#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/KeyboardBinding.h"
#include "../PGlitch_CPP/JoystickBinding.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<> static std::wstring ToString<KeyboardBinding>(const KeyboardBinding& binding)
            {
                RETURN_WIDE_STRING(binding.toString().c_str());
            }

            template<> static std::wstring ToString<ButtonBinding>(const ButtonBinding& binding)
            {
                RETURN_WIDE_STRING(binding.toString().c_str());
            }
        }
    }
}

namespace PGlitchTest
{		
	TEST_CLASS(KeyboardBindingTest)
	{
	public:
		TEST_METHOD(testEqualsExplicit)
		{
            KeyboardBinding KB1 = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);
            KeyboardBinding KB2 = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);
            KeyboardBinding KB3 = KeyboardBinding(sf::Keyboard::Right, InputCode::RIGHT);

            Assert::AreEqual(KB1, KB2);
            Assert::AreNotEqual(KB1, KB3);
            Assert::AreEqual(KB1.hashCode(), KB2.hashCode());
		}

        TEST_METHOD(testEqualsImplicit) {

            InputBinding& IB = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);
            KeyboardBinding KB = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);

            Assert::IsTrue(KB == IB);
        }

        TEST_METHOD(testToString) {
            KeyboardBinding KB1 = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);

            Assert::AreEqual("[KeyboardBinding: binding = L]", KB1.toString().c_str());
        }
	};

    TEST_CLASS(JoystickBindingTest) {
        TEST_METHOD(testEquals) {
            ButtonBinding JB1 = ButtonBinding(0, 0, InputCode::LEFT);
            ButtonBinding JB2 = ButtonBinding(0, 0, InputCode::LEFT);

            Assert::AreEqual(JB1, JB2);
            Assert::AreEqual(JB1.hashCode(), JB2.hashCode());
        }
        TEST_METHOD(testToString) {
            ButtonBinding JB1 = ButtonBinding(0, 0, InputCode::LEFT);

            Assert::AreEqual("[JoystickBinding(Button): binding = L]", JB1.toString().c_str());
        }
    };
}
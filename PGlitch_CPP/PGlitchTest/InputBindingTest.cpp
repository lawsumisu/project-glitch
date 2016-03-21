#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PGlitch_CPP/KeyboardBinding.h"

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

            std::unique_ptr<InputBinding> IB (new KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT));
            std::unique_ptr<KeyboardBinding> KB(new KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT));

            Assert::IsTrue((*IB)==KB.get());
        }

        TEST_METHOD(testToString) {
            KeyboardBinding KB1 = KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT);

            Assert::AreEqual("[KeyboardBinding: binding = L]", KB1.toString().c_str());
        }
	};
}
#include <cmath>
#include <cstdint>
#include <iostream>

uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus)
{
    uint32_t answer = 0;
    uint32_t multiplier = 1;

    value2 %= modulus;

    while (value1 > 0) {
        if (value1 & 0b1) {
            answer += value2 * multiplier;
        }
        multiplier <<= 1;
        value1 >>= 1;
    }

    answer %= modulus;

    return answer;
}

uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus)
{
    uint32_t answer = 1;
    uint32_t pow_x = base;

    while (exponent > 0) {
        if (exponent & 0b1) {
            answer = mulmod(answer, pow_x, modulus);
        }
        exponent >>= 1;
        pow_x = mulmod(pow_x, pow_x, modulus);
    }

    return answer;
}

uint32_t their_powmod(uint32_t x, uint32_t pow, uint32_t m)
{
    // you will make these uint32_t types
    // for the final submission
    uint64_t ans = 1;
    uint64_t pow_x = x;

    // NOTE: in the full assignment you will have to
    // replace the 64-bit types with an algorithm that
    // performs multiplication modulo a 31-bit number while
    // only using 32-bit types.

    while (pow > 0) {
        if (pow & 1 == 1) {
            // will replace the following line with a "mulmod" call
            // discussed on Nov 7 (see also the worksheet posted then)
            ans = (ans * pow_x) % m;
        }

        // as well as this line (i.e. call mulmod instead)
        pow_x = (pow_x * pow_x) % m;

        pow >>= 1; // divides by 2
    }

    return ans;
}

bool verify_powmod(uint64_t expected, uint32_t actual, uint32_t base, uint32_t power, uint32_t mod)
{
    if (expected != actual) {
        std::cout << "FAILED CASE:" << std::endl;
        std::cout << "Base: ";
        std::cout << base;
        std::cout << " Power: ";
        std::cout << power;
        std::cout << " Mod: ";
        std::cout << mod << std::endl;
        std::cout << "Expected: ";
        std::cout << expected;
        std::cout << " Actual: ";
        std::cout << actual << std::endl;
        return false;
    }
    return true;
}

void test_powmod()
{
    const uint32_t power = (uint32_t)std::pow(2.0, 31) - 1;
    const uint32_t mod = (uint32_t)std::pow(2.0, 31) - 1;
    for (uint32_t i = 0; i < (uint32_t)std::pow(2.0, 31) - 1; i++) {
        auto expected = (i * power) % mod;
        uint32_t actual = mulmod(i, power, mod);
        verify_powmod(expected, actual, i, power, mod);
    }
    std::cout << "TESTS ENDED" << std::endl;
}

int main()
{
    std::cout << powmod('a', 7, 95477);
}
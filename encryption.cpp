#include "encryption.h"

uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus)
{
    uint32_t answer = 0;

    value2 %= modulus;

    while (value1 > 0) {
        if (value1 & 0b1) {
            answer = (answer + value2) % modulus;
        }

        value2 <<= 1;
        value2 %= modulus;
        value1 >>= 1;
    }

    answer %= modulus;

    return answer;
}

uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus)
{
    uint32_t answer = 1;
    uint32_t pow_x = base % modulus;

    while (exponent > 0) {
        if (exponent & 0b1) {
            answer = mulmod(answer, pow_x, modulus);
        }
        exponent >>= 1;
        pow_x = mulmod(pow_x, pow_x, modulus);
    }

    return answer;
}
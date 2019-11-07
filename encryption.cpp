#include "encryption.h"

uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus)
{
    uint32_t answer = 0;
    for (auto i = 0; i < 31; i++) {
        answer += ((((value1 >> i) & 0b1) * value2) << i) % modulus;
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

uint32_t encryptChar(char decrypted)
{
    return 0;
}

char decryptChar(uint32_t encrypted)
{
    return 'A';
}
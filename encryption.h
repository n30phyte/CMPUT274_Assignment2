#pragma once

#include <stdint.h>

uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus);

uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus);

uint32_t encryptChar(char decrypted, uint32_t otherPubKey, uint32_t otherModulus);

char decryptChar(uint32_t encrypted, uint32_t thisPrivKey, uint32_t thisModulus);
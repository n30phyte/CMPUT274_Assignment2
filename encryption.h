/******************************************************************************
 *
 *      Student 1:          Benjamin Kong -- 1573684
 *      Student 2:          Michael Kwok  -- 1548454
 *      CMPUT 274:          Fall 2019
 *
 *      Assignment 2:       Encrypted Communication
 *
 *****************************************************************************/

#pragma once

#include <stdint.h>

/**
 * 32 bit Modular Exponentiation.
 *
 * @param base the base of exponentiation.
 * @param exponent the exponent of exponentiation.
 * @param modulus the modulus of the operation.
 * @return answer the result of fast expoenetiation. 
*/

uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus);

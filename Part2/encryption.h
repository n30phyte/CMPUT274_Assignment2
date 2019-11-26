/******************************************************************************
 *
 *      Student 1:          Benjamin Kong -- 1573684
 *      Student 2:          Michael Kwok  -- 1548454
 *      CMPUT 274:          Fall 2019
 *
 *      Assignment 2:       Encrypted Communication
 *
 * This header file contains some of functions defined in encryption.cpp
 * Functions exposed here are only ones useful in the main encrypted_communication.cpp
 * file.
 * 
 * Written to make testing our code from desktop C++ much easier, and to allow
 * the main file to be cleaner.
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

/**
 * Primality test. Returns true if the number is a prime.
 *
 * @param number the number to test.
 * @return true if prime, false if composite.
 */
bool isPrime(uint16_t number);

/**
 * Calculate the Greatest Common Divisor
 *
 * @param value1 the first value.
 * @param value2 the second value.
 * @return Calculated GCD of the number.
 */
uint32_t gcd(uint32_t value1, uint32_t value2);

void extendedEuclideanAlgorithm(uint32_t& x, uint32_t& y, uint32_t e, uint32_t totient);
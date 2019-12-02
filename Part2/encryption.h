/******************************************************************************
 *
 *      Student 1:          Benjamin Kong -- 1573684
 *      Student 2:          Michael Kwok  -- 1548454
 *      CMPUT 274:          Fall 2019
 *
 *      Assignment 2:       Encrypted Communication
 *
 * This header file contains some of functions defined in encryption.cpp
 * Functions exposed here are only ones useful in the main
 *encrypted_communication.cpp file.
 *
 * Written to make testing our code from desktop C++ much easier, and to allow
 * the main file to be cleaner.
 *****************************************************************************/

#pragma once

#include <stdint.h>

/**
 * 32 bit Modular Exponentiation.
 *
 * Calculates (base^exponent) % modulus. Procedure is described in the class
 * notes.
 *
 * @param[in]   base        Base of exponentiation.
 * @param[in]   exponent    Exponent of exponentiation.
 * @param[in]   modulus     Modulus of the operation.
 * @return      answer      Result of fast expoenetiation.
 */
uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus);

/**
 * Primality test. Returns true if the number is a prime.
 *
 * @param[in] number  The number to test.
 * @return            True if prime, false if composite.
 */
bool isPrime(uint16_t number);

/**
 * Calculate the greatest common divisor.
 *
 * @param[in] value1  The first number.
 * @param[in] value2  The second number.
 *
 * @return            The calculated GCD.
 */
uint32_t gcd(uint32_t value1, uint32_t value2);

/**
 * Extended Euclidean Algorithm to find factors.
 *
 * @param[out]  x       Private Key
 * @param[in]   e       The public key.
 * @param[in]   totient The totient.
 */
void extendedEuclideanAlgorithm(uint32_t& x, uint32_t e, uint32_t totient);
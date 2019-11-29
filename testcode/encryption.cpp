/******************************************************************************
 *
 *      Student 1:          Benjamin Kong -- 1573684
 *      Student 2:          Michael Kwok  -- 1548454
 *      CMPUT 274:          Fall 2019
 *
 *      Assignment 2:       Encrypted Communication
 *
 *****************************************************************************/
#include "encryption.h"

#include <math.h>

/**
 * 32 bit Modular Multiplication.
 *
 * Calculates the modular multplication of 2 values without requiring more than
 * 32 bits at any point during computation.
 *
 * @param value1 the first number to multiply.
 * @param value2 the second number to multiply.
 * @param modulus the modulus of the operation.
 * @return the result of modular multiplication.
 */
uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus) {
  uint32_t answer = 0;

  // Take the modulus of `value2' to prevent overflow. This is justified
  // through modulus rules.
  value2 %= modulus;

  while (value1 > 0) {
    // If the current bit is a one
    if (value1 & 0b1) {
      answer = (answer + value2) % modulus;
    }

    // Multiply 'value2' by two with bit shift, then take the modulus.
    value2 = (value2 << 1) % modulus;

    // Divide 'value1' by two by bit shift
    value1 >>= 1;
  }

  // The last step is to take the modulus of the answer.
  answer %= modulus;

  return answer;
}

/**
 * 32 bit Modular Exponentiation.
 *
 * Calculates (base^exponent) % modulus. Procedure is described in the class
 * notes.
 *
 * @param[in]   base        Base of exponentiation.
 * @param[in]   exponent    Exponent of exponentiation.
 * @param[in]   modulus     Modulus of the operation.
 * @return answer the result of fast expoenetiation.
 */
uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus) {
  uint32_t answer = 1;
  uint32_t pow_x = base % modulus;

  while (exponent > 0) {
    if (exponent & 0b1) {
      answer = mulmod(answer, pow_x, modulus);
    }

    // Essentially divide the exponent by two using bitwise operators.
    exponent >>= 1;
    pow_x = mulmod(pow_x, pow_x, modulus);
  }

  return answer;
}
/**
 * Swap two ints
 *
 * @param[in, out]  a   First number
 * @param[in, out]  b   Second number
 */
void swap(uint32_t& a, uint32_t& b) {
  auto temp = a;
  a = b;
  b = temp;
}

/**
 * Primality test. Returns true if the number is a prime.
 *
 * @param[in] number  The number to test.
 * @return            True if prime, false if composite.
 */
bool isPrime(uint16_t number) {
  for (uint16_t i = 2; i < sqrt(number); i++) {
    if ((number % i) == 0) {
      return false;
    }
  }
  return true;
}

uint32_t gcd(uint32_t value1, uint32_t value2) {
  while (value2 > 0) {
    value1 = value1 % value2;
    swap(value1, value2);
  }

  return value1;
}

uint32_t reduce_mod(int32_t x, uint32_t totient) {
  if (x < 0) {
    auto z = (-x) / (totient + 1);
    return (x + (z * totient)) % totient;
  } else {
    return (x % totient);
  }
}

/**
 * Extended Euclidean Algorithm to find factors.
 *
 * @param[out]  x       Private Key
 * @param[in]   e       The public key.
 * @param[in]   totient The totient.
 */
void extendedEuclideanAlgorithm(uint32_t& x, uint32_t e, uint32_t totient) {
  int32_t r[40];
  int32_t s[40];
  int32_t t[40];

  r[0] = e;
  r[1] = totient;

  s[0] = 1;
  s[1] = 0;

  t[0] = 0;
  t[1] = 1;

  auto i = 1;

  while (r[i] > 0) {
    auto q = r[i - 1] / r[i];
    r[i + 1] = r[i - 1] % r[i];
    s[i + 1] = s[i - 1] - (q * s[i]);
    t[i + 1] = t[i - 1] - (q * t[i]);
    i++;
  }
  x = s[i - 1];
}
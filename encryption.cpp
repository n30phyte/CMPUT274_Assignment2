//==//--//==//--//==//--//==//--//==//--//==//--//==//--//==//--//==//--//==//
//    
//      Student 1:          Benjamin Kong -- 1573684
//      Student 2:          Michael Kwok  -- 1548454
//
//      CMPUT 274:          Fall 2019
//
//      Assignment 2:       Encrypted Communication
//
//--//==//--//==//--//==//--//==//--//==//--//==//--//==//--//==//--//==//--//

#include "encryption.h"

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
uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus)
{
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
 * Calculates (base^exponent) % modulus. Procedure is described in the class notes.
 *
 * @param base the base of exponentiation.
 * @param exponent the exponent of exponentiation.
 * @param modulus the modulus of the operation.
 * @return answer the result of fast expoenetiation. 
*/
uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus)
{
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
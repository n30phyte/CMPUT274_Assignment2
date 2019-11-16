#include "encryption.h"

/*
    Description: Modular multiplication. Calculates 
        (value1 * value2) % modulus without exceeding 32 bits at any point 
        during computation. 

    Arguments: 
        * value1 (uint32_t): the first number to multiply.
        * value2 (uint32_t): the second number to multiply.
        * modulus (uint32_t): the modulus of the operation.
    
    Returns:
        * answer (uint32_t): the result of modular multiplication. 
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

/*
    Description: Fast exponentiation. Calculates (base^exponent) % modulus.
        Procedure is described in the class notes.

    Arguments: 
        * base (uint32_t): the base of exponentiation.
        * exponent (uint32_t): the exponent of exponentiation.
        * modulus (uint32_t): the modulus of the operation.
    
    Returns:
        * answer (uint32_t): the result of fast expoenetiation. 
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
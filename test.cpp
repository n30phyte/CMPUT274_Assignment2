#include <iostream>
#include <stdint.h>

uint32_t mulmod(uint32_t a, uint32_t b, uint32_t modulus)
{
    return (a * b) % modulus;
}

uint32_t mulmod_fast(uint32_t a, uint32_t b, uint32_t modulus)
{

}

int main()
{
    std::cout << "mulmod: " << mulmod(52, 1000000, 3) << ". mulmod_fast: " << mulmod_fast(52, 1000000, 3) << std::endl;
}
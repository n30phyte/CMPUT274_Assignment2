#include <cstdlib>
#include <iostream>

#include "encryption.h"

uint16_t generateCoprime(uint32_t totient) {
  auto number = 10;

  // 2^{minSize+1} - 1
  const uint16_t overflowLimit = (1U << (15 + 1)) - 1;

  while (gcd(number, totient) != 1) {
    if (number == overflowLimit) {
      number = (1U << 15);
    } else {
      number++;
    }
  }

  return number;
}
int main() {
  auto serverPublicKey = 11;
  auto serverModulus = 95477;
  auto serverPrivateKey = 27103;
  auto serverTotient = 94860;
  std::cout << "Server totient: " << 94860 << std::endl;
  uint32_t calculatedKey = 0;
  extendedEuclideanAlgorithm(calculatedKey, serverPublicKey, 94860);
  std::cout << calculatedKey << std::endl;
}
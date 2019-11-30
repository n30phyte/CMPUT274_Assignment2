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
  auto serverPublicKey = 1525;
  auto serverModulus = 106459361;
  auto serverPrivateKey = 52485661;
  auto serverTotient = 106437012;
  uint32_t calculatedKey = 0;
  extendedEuclideanAlgorithm(calculatedKey, serverPublicKey, serverTotient);
  std::cout << calculatedKey << std::endl;
}
#include <Arduino.h>

const int serverPublicKey = 7;
const int serverPrivateKey = 27103;
const int serverModulus = 95477;

const int clientPublicKey = 11;
const int clientPrivateKey = 38291;
const int clientModulus = 84823;

uint32_t d;
uint32_t n;
uint32_t e;
uint32_t m;

bool isServer = false;

constexpr uint32_t mulmod(uint32_t value1, uint32_t value2, uint32_t modulus)
{
    return (value1 * value2) % modulus;
}

uint32_t powmod(uint32_t base, uint32_t exponent, uint32_t modulus)
{
    uint32_t answer = 1;
    uint32_t pow_x = base % modulus;

    while (exponent > 0) {
        if (exponent & 0b1) {
            answer = mulmod(answer, pow_x, modulus);
        }
        exponent >>= 1;
        pow_x = mulmod(pow_x, pow_x, modulus);
    }

    return answer;
}

constexpr uint32_t encryptChar(char decrypted)
{
}

constexpr char decryptChar(uint32_t encrypted)
{

}

void receive() {};

void send(char message)
{
    auto encrypted = encryptChar(message);
    Serial3.write((char)(encrypted >> 0));
    Serial3.write((char)(encrypted >> 8));
    Serial3.write((char)(encrypted >> 16));
    Serial3.write((char)(encrypted >> 24));
}

void setup()
{

    if (isServer) {

        d = serverPrivateKey;
        n = serverModulus;
        e = serverPublicKey;
        m = clientModulus;

    } else {

        d = clientPrivateKey;
        n = clientModulus;
        e = clientPublicKey;
        m = serverModulus;
    }
}

int main()
{

    setup();
    Serial.println("Ready for input.");

    while (true) {
        if (Serial.available() > 0) {
            char input = Serial.read();
            if (input == '\r') {
                send('\r');
                send('\n');
            } else {
                send(input);
            }
            Serial3.flush();
        }
    }
    return 0;
}

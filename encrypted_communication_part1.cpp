#include <Arduino.h>

#include "encryption.h"

const int SERVER_PUBLIC_KEY = 7;
const int SERVER_PRIVATE_KEY = 27103;
const int SERVER_MODULUS = 95477;

const int CLIENT_PUBLIC_KEY = 11;
const int CLIENT_PRIVATE_KEY = 38291;
const int CLIENT_MODULUS = 84823;

const int ID_PIN = 13;

bool isServer = false;

// Communication Code start

char receive(uint32_t thisPrivKey, uint32_t thisModulus)
{
    uint32_t message = 0;
    message = message | ((uint32_t)Serial3.read()) << 0;
    message = message | ((uint32_t)Serial3.read()) << 8;
    message = message | ((uint32_t)Serial3.read()) << 16;
    message = message | ((uint32_t)Serial3.read()) << 24;

    return decryptChar(message, thisPrivKey, thisModulus);
};

void send(char message, uint32_t otherPubKey, uint32_t otherModulus)
{
    auto encrypted = encryptChar(message, otherPubKey, otherModulus);

    Serial3.write((char)(encrypted >> 0));
    Serial3.write((char)(encrypted >> 8));
    Serial3.write((char)(encrypted >> 16));
    Serial3.write((char)(encrypted >> 24));
}

// Communication Code End

void setup()
{
    init();
    pinMode(ID_PIN, INPUT);

    isServer = digitalRead(ID_PIN);
}

int main()
{

    setup();
    Serial.println("Ready for input.");

    while (true) {
        if (isServer) {
            // Server side
            if (Serial3.available() > 0) {
                Serial.print(receive(SERVER_PRIVATE_KEY, SERVER_MODULUS));
            }

        } else {
            // Client side
            if (Serial.available() > 0) {

                char input = Serial.read();
                Serial.print(input);
                Serial.flush();

                if (input == '\r') {
                    send('\r', SERVER_PUBLIC_KEY, SERVER_MODULUS);
                    send('\n', SERVER_PUBLIC_KEY, SERVER_MODULUS);
                } else {
                    send(input, SERVER_PUBLIC_KEY, SERVER_MODULUS);
                }

                Serial3.flush();
            }
        }
    }
    return 0;
}

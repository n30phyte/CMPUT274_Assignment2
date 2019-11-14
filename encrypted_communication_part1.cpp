#include <Arduino.h>

#include "encryption.h"

const uint32_t SERVER_PUBLIC_KEY = 7;
const uint32_t SERVER_PRIVATE_KEY = 27103;
const uint32_t SERVER_MODULUS = 95477;

const uint32_t CLIENT_PUBLIC_KEY = 11;
const uint32_t CLIENT_PRIVATE_KEY = 38291;
const uint32_t CLIENT_MODULUS = 84823;

const int ID_PIN = 13;

bool isServer = false;

// Communication Code start

union Converter {
    uint32_t message = 0;
    char buffer[4];
};

char receive(uint32_t thisPrivKey, uint32_t thisModulus)
{
    Converter conv;

    Serial3.readBytes(conv.buffer, 4);

    return powmod(conv.message, thisPrivKey, thisModulus);
};

void send(char message, uint32_t otherPubKey, uint32_t otherModulus)
{
    uint32_t encrypted = powmod(message, otherPubKey, otherModulus);

    Serial3.write((char)(encrypted >> 0));
    Serial3.write((char)(encrypted >> 8));
    Serial3.write((char)(encrypted >> 16));
    Serial3.write((char)(encrypted >> 24));
}

// Communication Code End

void setup()
{
    init();
    Serial.begin(9600);
    Serial3.begin(9600);

    pinMode(ID_PIN, INPUT_PULLUP);

    isServer = !digitalRead(ID_PIN);
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
                Serial.flush();

                if (input == '\r') {
                    Serial.println();
                    send('\r', SERVER_PUBLIC_KEY, SERVER_MODULUS);
                    send('\n', SERVER_PUBLIC_KEY, SERVER_MODULUS);
                } else {
                    Serial.print(input);
                    send(input, SERVER_PUBLIC_KEY, SERVER_MODULUS);
                }

                Serial3.flush();
            }
        }
    }
    return 0;
}

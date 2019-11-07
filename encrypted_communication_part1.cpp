#include <Arduino.h>

#include "encryption.h"

const int SERVER_PUBLIC_KEY = 7;
const int SERVER_PRIVATE_KEY = 27103;
const int SERVER_MODULUS = 95477;

const int CLIENT_PUBLIC_KEY = 11;
const int CLIENT_PRIVATE_KEY = 38291;
const int CLIENT_MODULUS = 84823;

uint32_t d;
uint32_t n;
uint32_t e;
uint32_t m;

bool isServer = false;

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
    init();

    if (isServer) {
    } else {
    }
}

int main()
{

    setup();
    Serial.println("Ready for input.");

    while (true) {
        if (isServer) {
            // Server side

        } else {
            // Client side

            if (Serial.available() > 0) {

                char input = Serial.read();
                Serial.print(input);

                if (input == '\r') {
                    send('\r');
                    send('\n');
                } else {
                    send(input);
                }

                Serial3.flush();
            }
        }
    }
    return 0;
}

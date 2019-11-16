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

// Store the read bytes in buffer, directly get uint32_t in message.
union Converter {
    uint32_t message = 0;
    char buffer[4];
};

/*
    Description: Receives an encrypted message, decrypts it, and returns the
        decrypted character.

    Arguments: 
        * thisPrivKey (uint32_t): is either the server or client key, depending
        on which arduino is receiving.
        * thisModulus (uint32_t): is either the server or client modulus,
        depending on which arduino is receiving.
    
    Returns:
        * decrypted (char): the result of decrypting the received message.
*/
char receive(uint32_t thisPrivKey, uint32_t thisModulus)
{
    Converter conv;

    // Write 4 bytes to the `Converter' union.
    Serial3.readBytes(conv.buffer, 4);

    char decrypted = powmod(conv.message, thisPrivKey, thisModulus);

    return decrypted;
};

/*
    Description: Encrypts a message, then sends it as four bytes to the other
        arduino.

    Arguments: 
        * message (char): the message to be encrypted and sent to the other 
        arduino.
        * otherPubKey (uint32_t): the public key of the other arduino.
        * otherModulus (uint32_t): the modulus of the other arduino.
*/
void send(char message, uint32_t otherPubKey, uint32_t otherModulus)
{
    uint32_t encrypted = powmod(message, otherPubKey, otherModulus);

    Serial3.write((char)(encrypted >> 0));
    Serial3.write((char)(encrypted >> 8));
    Serial3.write((char)(encrypted >> 16));
    Serial3.write((char)(encrypted >> 24));
}

// Communication Code End

/*
    Description: Setup function. Initializes Serial and Serial3, which are 
        needed for communication between the two arduinos.
*/
void setup()
{
    init();
    Serial.begin(9600);
    Serial3.begin(9600);

    pinMode(ID_PIN, INPUT_PULLUP);

    // INPUT_PULLUP defines 5V in as LOW and no voltage as HIGH, so flip the reading.
    isServer = !digitalRead(ID_PIN);
}

/*
    Description: Each arduino is continiously running the while loop. 

    If the arduino is acting as the server, it will listen for a message from
    the other arduino. If it detects a message, it will print the message after
    it is decrypted in the `receive' function.

    If the arduino is acting as the client, it will listen for input from the
    user. If a message is inputted, the arduino will send the message to the
    other arduino once it has been encrypted in the `send' function.
*/
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

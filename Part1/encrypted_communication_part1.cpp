/******************************************************************************
 *
 *      Student 1:          Benjamin Kong -- 1573684
 *      Student 2:          Michael Kwok  -- 1548454
 *      CMPUT 274:          Fall 2019
 *
 *      Assignment 2:       Encrypted Communication
 *
 *****************************************************************************/
#include <Arduino.h>

#include "encryption.h"

const uint32_t SERVER_PUBLIC_KEY = 7;
const uint32_t SERVER_PRIVATE_KEY = 27103;
const uint32_t SERVER_MODULUS = 95477;

const uint32_t CLIENT_PUBLIC_KEY = 11;
const uint32_t CLIENT_PRIVATE_KEY = 38291;
const uint32_t CLIENT_MODULUS = 84823;

const int ID_PIN = 13;

/**
 * Store the read bytes in buffer, directly get uint32_t in message.
 */
union Converter {
    uint32_t message = 0;
    char buffer[4];
};

/**
 * Stores private keys, public keys and moduli.
 */
struct ArduinoConstants {
    const uint32_t thisPrivateKey;
    const uint32_t thisModulus;
    const uint32_t otherPublicKey;
    const uint32_t otherModulus;
};

/**
 * Receives an encrypted message, and returns the decrypted character.
 * 
 * Reads 4 bytes from the Serial3 stream, storing it in union that allows a
 * char[4] to be accessed as a uint32_t.
 * 
 * The provided uint32_from_serial3() function did not seem to work, and this
 * was our solution.
 * 
 * @param &properties: A reference to the "constants" of this Arduino
 * @return The decrypted character.
 */
char receive(const ArduinoConstants& constants)
{
    Converter conv;

    // Read 4 bytes into the `Converter' union.
    Serial3.readBytes(conv.buffer, 4);

    char decrypted = powmod(conv.message, constants.thisPrivateKey, constants.thisModulus);

    return decrypted;
};

/**
 * Encrypts a message, then sends it as four bytes to the other Arduino.
 * 
 * 
 * The provided uint32_from_serial3() function did not seem to work, and this
 * was our solution.
 * @param message: the message to be encrypted and sent to the other Arduino
 * @param &properties: A reference to the "constants" of this Arduino.
 */
void send(char message, const ArduinoConstants& constants)
{
    Converter conv;

    conv.message = powmod(message, constants.otherPublicKey, constants.otherModulus);

    Serial3.write(conv.buffer, 4);
}

/**
 * Client processing loop.
 * 
 * Wait for input from the user. If there is a message, the Arduino will send the
 * message to the other Arduino once it has been encrypted in the 'send' function.
 * 
 * @param &properties: A reference to the "constants" of this Arduino.
 */
void clientLoop(const ArduinoConstants& constants)
{
    if (Serial.available() > 0) {
        char input = Serial.read();
        Serial.flush();

        if (input == '\r') {
            send('\r', constants);
            send('\n', constants);
            Serial.println();
        } else {
            Serial.print(input);
            send(input, constants);
        }

        Serial3.flush();
    }
}

/**
 * Server processing loop.
 * 
 * Wait for a message from the other Arduino. As soon as a new message is available,
 * it will print the message after it is decrypted in the 'receive' function.
 * 
 * @param &properties: A reference to the "constants" of this Arduino.
 */
void serverLoop(const ArduinoConstants& constants)
{
    if (Serial3.available() > 0) {
        Serial.print(receive(constants));
    }
}

/**
 * Setup function.
 * 
 * Initializes Serial for communication to the computer
 * and Serial3, which is used for communication between the two arduinos.
 * Calls Arduino initialization code (init())
 * Sets up pins to be on input mode for the server determination pin.
 */
void setup()
{
    init();
    Serial.begin(9600);
    Serial3.begin(9600);

    pinMode(ID_PIN, INPUT_PULLUP);
}

/**
 * Main function of program.
 */
int main()
{

    setup();

    bool serverPin = digitalRead(ID_PIN);

    if (serverPin) {
        Serial.println("This is server.");
        // Setup private keys, public keys and moduli.
        ArduinoConstants consts = {
            .thisPrivateKey = SERVER_PRIVATE_KEY,
            .thisModulus = SERVER_MODULUS,
            .otherPublicKey = CLIENT_PUBLIC_KEY,
            .otherModulus = CLIENT_MODULUS
        };

        while (true) {
            serverLoop(consts);
        }
    } else {
        Serial.println("This is client.");
        ArduinoConstants consts = {
            .thisPrivateKey = CLIENT_PRIVATE_KEY,
            .thisModulus = CLIENT_MODULUS,
            .otherPublicKey = SERVER_PUBLIC_KEY,
            .otherModulus = SERVER_MODULUS
        };

        while (true) {
            clientLoop(consts);
        }
    }

    return 0;
}

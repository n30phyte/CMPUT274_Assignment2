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

const int ID_PIN = 13;

/**
 * Stores private keys, public keys and moduli.
 */
struct ArduinoConstants {
  uint32_t thisPrivateKey;
  uint32_t thisModulus;
  uint32_t thisPublicKey;
  uint32_t otherPublicKey;
  uint32_t otherModulus;
};

/**
 * Store the read bytes in buffer, directly get uint32_t in message.
 */
union Converter {
  uint32_t message = 0;
  char buffer[4];
};

/**
 * The provided uint32_from_serial3() function did not seem to work, and this
 * was our solution.
 */
uint32_t receiveUint32() {
  Converter conv;

  // Read 4 bytes into the "buffer". This will be read from "message".
  Serial3.readBytes(conv.buffer, 4);

  return conv.message;
}

/**
 * The provided uint32_to_serial3() function did not seem to work, and this
 * was our solution.
 */
void sendUint32(uint32_t number) {
  Converter conv;

  conv.message = number;

  // Write 4 bytes from the "buffer".
  Serial3.write(conv.buffer, 4);
}

/**
 * Receives an encrypted message, and returns the decrypted character.
 *
 * Reads 4 bytes from the Serial3 stream, storing it in union that allows a
 * char[4] to be accessed as a uint32_t.
 *
 * The provided uint32_from_serial3() function did not seem to work, and this
 * was our solution.
 *
 * @param constants: A reference to the "constants" of this Arduino
 *
 * @return The decrypted character.
 */
char receive(const ArduinoConstants& constants) {
  auto number = receiveUint32();
  char decrypted =
      powmod(number, constants.thisPrivateKey, constants.thisModulus);

  return decrypted;
};

/**
 * Encrypts a message, then sends it as four bytes to the other Arduino.
 *
 * @param message: the message to be encrypted and sent to the other Arduino
 * @param constants: A reference to the "constants" of this Arduino.
 */
void send(char message, const ArduinoConstants& constants) {
  sendUint32(powmod(message, constants.otherPublicKey, constants.otherModulus));
}

/**
 * Communication processing loop.
 *
 * First check if receiving a message from the other arduino. If this is the
 * case, decrypt the message and print it.
 *
 * Then, check for input from the user. If there is a message, the Arduino
 * will send the message to the other Arduino once it has been encrypted
 * in the 'send' function.
 *
 * @param constants: A reference to the "constants" of this Arduino.
 */
void communication(const ArduinoConstants& constants) {
  // Check for user input.
  if (Serial.available() > 0) {
    char input = Serial.read();

    if (input == '\r') {
      send('\r', constants);
      send('\n', constants);
      Serial.println();
    } else {
      Serial.print(input);
      send(input, constants);
    }
  }

  // Check if receiving a message.
  if (Serial3.available() > 3) {
    Serial.print(receive(constants));
  }
}

/**
 * Generates a random number that is at least as large as 2^minSize.
 *
 * The 'randomness' is a result of reading an unused arduino pin (in
 * this case, the 1st pin is unused) using analogRead, which returns
 * a random integer between 0 and 1023. We take the least significant
 * bit as many times from this random number as needed to generate a
 * number at least as large as 'minSize'.
 *
 * @param minSize: the minimum size of the random number we want (2^minSize)
 *
 * @return the random number generated.
 */
uint16_t generateNumber(int minSize) {
  uint16_t number = 0;

  for (int i = 0; i < minSize; i++) {
    auto LSB = analogRead(1) & 0b1;
    number |= (LSB << i);
    delay(5);
  }

  return number;
}

/**
 * Generates a random prime number that is at least as large as 2^minSize.
 *
 * We generate a random number of at least minSize using generateNumber, then
 * increment it until it is a prime number. We also handle the case where the
 * number may overflow by wrapping around from 2^(k+1)-1 back to 2^k.
 *
 * @param minSize: the minimum size of the prime number we want (2^minSize)
 *
 * @return the random prime number generated.
 */
uint16_t generatePrime(int minSize) {
  auto number = generateNumber(minSize);

  // 2^{k+1} -1
  const uint16_t overflowLimit = (1U << (minSize + 1)) - 1;

  while (!isPrime(number)) {
    if (number == overflowLimit) {
      number = (1U << minSize);
    } else {
      number++;
    }
  }

  return number;
}

/**
 * Generates a random number that is coprime to 'totient' that is at least
 * as large as 2^minSize.
 *
 * We generate a random number of at least minSize using generateNumber,
 * then increment it until it is coprime to 'totient' (number is coprime
 * if gcd(number, totient) = 1). We also handle the case where the number
 * may overflow by wrapping around from 2^(k+1)-1 back to 2^k.
 *
 * @param minSize: the minimum size of the prime number we want (2^minSize)
 * @param totient: the totient target.
 *
 * @return the random coprime number generated.
 */
uint16_t generateCoprime(int minSize, uint32_t totient) {
  auto number = generateNumber(minSize);

  // 2^{minSize+1} - 1
  const uint16_t overflowLimit = (1U << (minSize + 1)) - 1;

  while (gcd(number, totient) != 1) {
    if (number == overflowLimit) {
      number = (1U << minSize);
    } else {
      number++;
    }
  }

  return number;
}

/**
 * Waits for a certain number of bytes on Serial3 or timeout.
 *
 * @param nbytes: the number of bytes we want
 * @param timeout: timeout period (ms); specifying a negative number
 * turns off timeouts (the function waits indefinitely
 * if timeouts are turned off).
 *
 * @return True if the required number of bytes have arrived.
 */
bool wait_on_serial3(uint8_t nbytes, long timeout) {
  unsigned long deadline = millis() + timeout;  // wraparound not a problem
  while (Serial3.available() < nbytes && (timeout < 0 || millis() < deadline)) {
    delay(1);  // be nice , no busy loop
  }
  return Serial3.available() >= nbytes;
}

/**
 * Setup function.
 *
 * Initializes Serial for communication to the computer
 * and Serial3, which is used for communication between the two arduinos.
 * Calls Arduino initialization code (init())
 * Sets up pins to be on input mode for the server determination pin.
 */
void setup() {
  init();
  Serial.begin(9600);
  Serial3.begin(9600);

  pinMode(ID_PIN, INPUT_PULLUP);
}

/**
 * Generates keys using pass-by-reference.
 *
 * First, generate p and q. Then, calculate the modulus and totient.
 * Next, we generate the public and private keys.
 *
 * @param constants: A reference to the "constants" of this Arduino.
 */
void generateKeys(ArduinoConstants& constants) {
  uint16_t p = generatePrime(14);
  // typed to force a promotion on multiplication
  uint32_t q = generatePrime(15);

  // Calculate some constants we use.
  constants.thisModulus = p * q;
  uint32_t totient = (p - 1) * (q - 1);

  constants.thisPublicKey = generateCoprime(15, totient);

  extendedEuclideanAlgorithm(constants.thisPrivateKey, constants.thisPublicKey,
                             totient);
}

/**
 * Main function of program.
 *
 * First, generate the modulus, totient, private key, and public key.
 * Determines if the arduino running the code is the server or not.
 *
 * Then, follow through with the procedure for the client and server
 * respectively according to the outline given in the assignment
 * description.
 */
int main() {
  setup();

  enum States { Start, WaitForKey, WaitForAck, DataExchange };

  bool serverPin = digitalRead(ID_PIN);

  ArduinoConstants consts;
  States currentState = Start;
  generateKeys(consts);

  if (serverPin) {
    // SERVER CODE //
    Serial.println("This is server.");

    bool sentKey = false;

    while (true) {
      switch (currentState) {
        case Start:

          if (wait_on_serial3(1, 1000)) {
            if (Serial3.read() == 'C') {
              // Request
              currentState = WaitForKey;
            }
          }

          break;

        case WaitForKey:

          if (wait_on_serial3(8, 1000)) {
            consts.otherPublicKey = receiveUint32();
            consts.otherModulus = receiveUint32();
            if (!sentKey) {
              Serial3.write('A');
              sendUint32(consts.thisPublicKey);
              sendUint32(consts.thisModulus);
              currentState = WaitForAck;
              sentKey = true;
            }
          } else {
            sentKey = false;
            currentState = Start;
          }
          break;

        case WaitForAck:

          if (wait_on_serial3(1, 1000)) {
            char response = Serial3.read();
            if (response == 'A') {
              currentState = DataExchange;

            } else if (response == 'C') {
              currentState = WaitForKey;
            }
          } else {
            sentKey = false;
            currentState = Start;
          }

          Serial3.flush();

          while (Serial3.available()) {
            Serial3.read();
          }

          break;

        case DataExchange:
          communication(consts);
          break;

        default:
          sentKey = false;
          currentState = Start;
          break;
      }
    }
  } else {
    // CLIENT CODE //
    Serial.println("This is client.");

    bool exchangeSucceed = false;

    while (true) {
      switch (currentState) {
        case Start:

          Serial3.write('C');
          sendUint32(consts.thisPublicKey);
          sendUint32(consts.thisModulus);
          currentState = WaitForAck;
          break;

        case WaitForAck:

          if (wait_on_serial3(9, 1000)) {
            if (Serial3.read() == 'A') {
              // Ack
              consts.otherPublicKey = receiveUint32();
              consts.otherModulus = receiveUint32();
              Serial3.write('A');
              exchangeSucceed = true;
            } else {
              currentState = Start;
            }
          }

          if (exchangeSucceed) {
            currentState = DataExchange;
          } else {
            // if more than 1000 ms has passed and exchange has not succeeded
            currentState = Start;
          }

          Serial.flush();
          Serial3.flush();

          while (Serial3.available()) {
            Serial3.read();
          }
          break;

        case DataExchange:
          communication(consts);
          break;

        default:
          currentState = Start;
          break;
      }
    }
  }

  return 0;
}

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
 * @param &properties: A reference to the "constants" of this Arduino
 * @return The decrypted character.
 */
char receive(const ArduinoConstants& constants) {
  char decrypted =
      powmod(receiveUint32(), constants.thisPrivateKey, constants.thisModulus);

  return decrypted;
};

/**
 * Encrypts a message, then sends it as four bytes to the other Arduino.
 *
 * @param message: the message to be encrypted and sent to the other Arduino
 * @param &properties: A reference to the "constants" of this Arduino.
 */
void send(char message, const ArduinoConstants& constants) {
  sendUint32(powmod(message, constants.otherPublicKey, constants.otherModulus));
}

/**
 * Client message processing loop.
 *
 * Wait for input from the user. If there is a message, the Arduino will send
 * the message to the other Arduino once it has been encrypted in the 'send'
 * function.
 *
 * @param &properties: A reference to the "constants" of this Arduino.
 */
void clientLoop(const ArduinoConstants& constants) {
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

uint16_t generateNumber(int minSize) {
  uint16_t number = 0;
  for (int i = 0; i < minSize; i++) {
    auto LSB = analogRead(1) & 0b1;
    number |= (LSB << i);
    delay(5);
  }
  return number;
}

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
 * Waits for a certain number of bytes on Serial3 or timeout
 *
 *
 *
 * @param nbytes : the number of bytes we want
 * @param timeout : timeout period (ms); specifying a negative number
 * turns off timeouts (the function waits indefinitely
 * if timeouts are turned off).
 *
 * @return True if the required number of bytes have arrived .
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
 * Main function of program.
 */
int main() {
  setup();

  enum States { Start, WaitForKey, WaitForAck, DataExchange };

  bool serverPin = digitalRead(ID_PIN);

  ArduinoConstants consts;
  States currentState = Start;

  Serial.println("Generating primes...");

  uint16_t p = generatePrime(14);
  // typed to force a promotion on multiplication
  uint32_t q = generatePrime(15);

  // Calculate some constants we use.
  consts.thisModulus = p * q;
  uint32_t totient = (p - 1) * (q - 1);

  Serial.println("Generating coprime...");
  consts.thisPublicKey = generateCoprime(15, totient);

  extendedEuclideanAlgorithm(consts.thisPrivateKey, consts.thisPublicKey,
                             totient);

  Serial.print("This private key: ");
  Serial.println(consts.thisPrivateKey);
  Serial.print("This public key: ");
  Serial.println(consts.thisPublicKey);
  Serial.print("This modulus: ");
  Serial.println(consts.thisModulus);

  if (serverPin) {
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
              Serial3.write("A");
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
          break;
        case DataExchange:
          while (true) {
            if (Serial3.available() > 0) {
              Serial.print(receive(consts));
            }
          }
          break;
        default:
          sentKey = false;
          currentState = Start;
          break;
      }
    }
  } else {
    Serial.println("This is client.");

    // Variable reused for different time operations.
    bool exchangeSucceed = false;

    while (true) {
      switch (currentState) {
        case Start:

          Serial3.write("C");
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
              Serial3.write("A");
              exchangeSucceed = true;
            }
          }

          if (exchangeSucceed) {
            currentState = DataExchange;
          } else {
            // if more than 1000 ms has passed and exchange has not succeeded
            currentState = Start;
          }

          break;
        case DataExchange:
          Serial.println("Ready for input.");
          while (true) {
            clientLoop(consts);
          }
          break;
        default:
          currentState = Start;
          break;
      }
    }
  }

  return 0;
}

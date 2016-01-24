// Compiles on Arduino IDE 1.6.8 2016/01/19 with http://digistump.com/package_digistump_index.json
// Simple infrared receiver that is attached to a computer
// as a hid-generic input,hidraw "USB HID v1.01 Keyboard"
// Infrared receiver soldered to Micronucleus
//
//                   |-----|
//                   | ( ) |
//                   |-----|
//                   |  |  |
//                   |  |  |
//    Digispark Pins 0  1  2    <-- TSOP can be soldered directly to the board
// 0 = Data In
// 1 = GND (pin pulled low)
// 2 = VCC (pin pulled high)

#include "DigiKeyboard.h"
// not all keys are mapped in the DigiKeyboard.h file.
// you have to map it here
#define KEY_HOME   0x4A
#define KEY_PAGE_UP   0x4B
#define KEY_PAGE_DOWN   0x4E
#define KEY_ESCAPE   0x29
#define KEY_UP_ARROW   0x52
#define KEY_DOWN_ARROW   0x51
#define KEY_LEFT_ARROW   0x50
#define KEY_RIGHT_ARROW   0x4F

#define KEY_VOLUME_UP   0x80
#define KEY_VOLUME_DOWN   0x81

int irPin = 0; //Sensor pin connect to digital pin2 (ATINY85 pin7)
int start_bit = 2200; //Start bit threshold (Microseconds)
int bin_1 = 1000; //Binary 1 threshold (Microseconds)
int bin_0 = 400; //Binary 0 threshold (Microseconds)
const byte BIT_PER_BLOCK = 32;

void setup() {

  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  digitalWrite(1, 0);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);

  // don't need to set anything up to use DigiKeyboard
  DigiKeyboard.delay(1000);
}


void loop() {

  int key = getIRKey();

  if (key != 0)
  {
    DigiKeyboard.print("#");  //uncomment this if you want to
    DigiKeyboard.print(key); //print out the value of the button

    switch (key)
    {
      case 17: DigiKeyboard.println("1"); break;
      case 18: DigiKeyboard.println("2"); break;
      case  4: DigiKeyboard.sendKeyStroke(KEY_VOLUME_UP); break;
      case  5: DigiKeyboard.sendKeyStroke(KEY_VOLUME_UP); break;
    }
  }

  DigiKeyboard.sendKeyStroke(0);

}

/////////////////////////////////////////////////////////////
// decode infrared signal
// Code from http://www.instructables.com/id/DIY-USB-IR-receiver/
// TODO: Replace with IRLib
/////////////////////////////////////////////////////////////
int getIRKey() {
  int data[BIT_PER_BLOCK];
  int i;
  while (pulseIn(irPin, HIGH) < start_bit); // Wait for a start bit
  for (i = 0 ; i < BIT_PER_BLOCK ; i++)
    data[i] = pulseIn(irPin, HIGH); // Start measuring bits, I only want HIGH pulses

  delay(100);
  for (i = 0 ; i < BIT_PER_BLOCK ; i++) // Parse them
  {
    if (data[i] > bin_1) //is it a 1?
      data[i] = 1;
    else if (data[i] > bin_0) //is it a 0?
      data[i] = 0;
    else
      return -1; // Flag the data as invalid; Return -1 on invalid data
  }

  // Based on NEC protocol, command data started from bit 16
  // and end with bit 24 (8 bits long)
  int result = 0;
  for (i = 16 ; i < 24; i++) {
    // DigiKeyboard.print(data[i]); // print out the value of button in binary form
    if (data[i] == 1) result |= (1 << i - 16); // Convert data bits to integer
  }
  return result; // Return key number
}


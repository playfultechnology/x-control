/*
 * XControl-1 (c) 2020 Playful Technology
 * A custom joystick interface with native USB connectivity based on the Thrustmaster FCS Mark I
 * Deploy this code to an ATMega32U4-based board (e.g. Arduino Pro Micro)
 */

// INCLUDES
// HID-Project can be installed through Arduino IDE Library Manager, 
// or via https://github.com/NicoHood/HID
#include "HID-Project.h"

// CONSTANTS
// Pins to which inputs are attached
const byte buttonPins[] = {2,3,4,5}; // Digital momentary buttons
const byte axisPins[] = {A0,A1}; // Analog axes (linear potentiometer)
const byte hatPin = A3; // Hat switch (digital, but read via analog line as uses resistor tree) 

const uint16_t SampleRate = 60; // How frequently to poll inputs (Hz)
const uint16_t SampleDelay = (1000/SampleRate); // How long to wait before next polling input (ms)

// SETUP
void setup() {
  // Initialise digital pins within internal pullup resistor
  for(int i=0;i<4;i++){
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Send a clean report to the host
  Gamepad.begin();
}

// LOOP
void loop() {
  // Start with a blank slate...
  Gamepad.releaseAll(); 

  // Check which digital buttons have been pressed
  for(int i=0;i<4;i++){
    if (digitalRead(buttonPins[i]) == LOW) {
      // Note that button numbers are 1-based not 0-based
      Gamepad.press(i+1);
    }
  }

  // The value of the x axis sent to the host will end up in the range {-32768,32767}, 
  // which can be expressed as an int16_t. However, we'll calculate use int32_t in case 
  // we go out of bounds during calculation
  // Raw value lies in range 0-1023
  int32_t X = analogRead(axisPins[0]);
  // Only the central section of the pot is used, so scale to fill the whole extent  
  X = map(X, 982, 478, -32768, 32767);
  // Ensure value doesn't exceed limits
  X = constrain(X, -32768, 32767);
  // Assign to primary x axis
  Gamepad.xAxis(X);

  // Raw value lies in range 0-1023
  int32_t Y = analogRead(axisPins1]);
  // Only the central section of the pot is used, so scale to fill the whole extent  
  Y = map(Y, 982, 478, -32768, 32767);
  // Ensure value doesn't exceed limits
  Y = constrain(Y, -32768, 32767);
  // Assign to primary y axis
  Gamepad.yAxis(Y);

  // Take raw reading of hat pin
  int hat = analogRead(hatPin);
  // When voltage divider is created with 47kΩ resistor, values read should be
  // Simple: 371(unpressed), 443(left), 545(down), 714(right), 1019 (up)
  // Compound: 586 (down-left), 759 (down-right)
  // We'll switch logic based on midpoints between these ideal values
  if(hat>880) {
    Gamepad.dPad1(GAMEPAD_DPAD_UP);
  }
  else if(hat>737) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);
  }
  else if(hat>650) {
    Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
  }
  else if(hat>565) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
  }
  else if(hat>494) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
  }
  else if(hat>407) {
    Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
  }
  else {
    Gamepad.dPad1(GAMEPAD_DPAD_CENTERED);
  }

  // Functions used above only set values on the gamepad object
  // Now write the report to the host
  Gamepad.write();

  // Pause 
  delay(SampleDelay);
}

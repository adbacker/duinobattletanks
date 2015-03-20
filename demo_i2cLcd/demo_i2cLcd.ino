/* YourDuinoStarter Example: nRF24L01 Transmit controller values
 - WHAT IT DOES: Reads Analog values on A0, A1 and transmits
   them over a nRF24L01 Radio Link to another transceiver.
 - SEE the comments after "//" on each line below
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
   - 
   Analog controller or two 10K potentiometers:
   GND to Arduino GND
   VCC to Arduino +5V
   X Pot to Arduino A0
   Y Pot to Arduino A1
   
 - V1.00 11/26/13
   Based on examples at http://www.bajdi.com/
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include "printf.h"

/*-----( Declare Constants and Pin Numbers )-----*/

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

int secondCounter=0;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(57600);
  printf_begin();
  SPI.begin();
  lcd.begin(20,2); // initialize the lcd for 20 chars 4 lines and turn on backlight
}//--(end setup )---


void loop() {  /****** LOOP: RUNS CONSTANTLY ******/

  unsigned long now=millis();
  if (now%1000==0) {
  secondCounter = now/1000;

  lcd.setCursor(0,0);
  lcd.print("Counting up...");
  //String seconds = String(secondCounter);
  String line2 = "*** " + String(secondCounter) + " ***";

  lcd.setCursor(0,1);
  lcd.print(line2);
  }
}

/*-----( Declare User-written Functions )-----*/


//*********( THE END )***********



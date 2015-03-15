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
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "printf.h"
#include "Tank.h"
#include "Statuslcd.h"

/*-----( Declare Constants and Pin Numbers )-----*/

//#define DEBUG //debug - serial.start switch

#define CONTROLLERA
//#define CONTROLLERB

#define CE_PIN   9
#define CSN_PIN 10
#define controller_X 0
#define controller_Y 1

#define trigger_pin 6
#define weapswitch_pin 7

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
// Network uses that radio
RF24Network network(radio);

#ifdef CONTROLLERA
//controller/tank set A
const uint16_t this_node = 00;
const uint16_t other_node = 02;
Tank tank(1,100,99); //init tank: id, ms energy recharge interval, max energy
#endif

#ifdef CONTROLLERB
//tank/controller set B
const uint16_t this_node = 01;
const uint16_t other_node = 011;
Tank tank(2,100,99); //init tank: id, ms energy recharge interval, max energy
#endif


/*-----( Declare Variables )-----*/

long controller[3];  // 3 element array holding controller readings & fire state

/*
struct controller {
  int x;
  int y;
  long encodedShot;
}*/

long hit[2]; //2 element array holding the encoded hit, and the side that was hit

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
Statuslcd statuslcd;

long firingValue=0L; //value to send for firing - 0 (not firing) or a weapon/power/player encoded long
boolean firing=false;
boolean switchingweapons=false;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  #ifdef DEBUG
  Serial.begin(57600);
  #endif
  
  pinMode(trigger_pin,INPUT_PULLUP);
  pinMode(weapswitch_pin,INPUT_PULLUP);
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  lcd.begin(20,2); // initialize the lcd for 20 chars 4 lines and turn on backlight
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{  
  network.update();     // Pump the network regularly
  if (! tank.isDead()) {     
    handleFire(); 
    handleWeaponSwitch();
    handleControllerUpdate(); //send controller & firing values
    tank.regen();  //update tank energy
    tank.regenShield(); //this *does* work.  WTH?!
    check_network(); //check the network queue to see if we've received anything
  } 
  statuslcd.update(tank,lcd); //update player status lcd  
  //debugController();
  //debugTank();
  //--(end main loop )---
}

/*-----( Declare User-written Functions )-----*/

void handleControllerUpdate()
{
  //controller cmd structure: 0 & 1 == controller x & y, 2 == encoded fire cmd
  //Serial.println("handle controller update called");
  //read the controller x & y values
  controller[0] = long(analogRead(1));
  controller[1] = long(analogRead(0));
  controller[2] = firingValue;

  //send our controller values
  RF24NetworkHeader header(/*to node*/ other_node);
  network.write(header,&controller,sizeof(controller));
  //debugController();
}


void handleFire()
{  
  //read the trigger switch
  if ( digitalRead(trigger_pin)==LOW ) {
    //only fire if this is the first time through
    //ie, the fire button was just pushed
    if ( firing==false ) {
      firingValue=tank.fire();
      Serial.print("FIRE!!******\t");
      Serial.println(firingValue);
      firing=true;
    }   
  }
  //reset once the fire button is released
  else {
    firingValue=0L;
    firing=false;    
  }
}


//handle all the cruft having to do with switching weapons
void handleWeaponSwitch()
{
  //read the weapon switch 
  if (digitalRead(weapswitch_pin)==LOW) {
    //only switch if this is the first time through
    //ie, the weap switch button was just pushed
    //this insures one switch per press
    if (switchingweapons==false) {
      tank.nextWeapon();
      switchingweapons=true;
    }  
  }
  //reset once the weap switch button is released
  else {
    switchingweapons=false;
  }   
}

void check_network()
{
  // Is there anything ready for us?
  while ( network.available() ) {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&hit,sizeof(hit));
    tank.registerHit(long(hit[0]),long(hit[1]));
  }
}

void debugTank()
{
  Serial.print("id");
  Serial.print(tank.getId());
  Serial.print("\tcurrentWeap:");
  Serial.print(tank.getCurrentWeapon().getId());
  Serial.print("\tweaponEcost:");
  Serial.print(tank.getCurrentWeapon().getEcost());
  Serial.print("\tenergy:");
  Serial.println(tank.getEnergy());
}

void debugController()
{
  Serial.print("x:");
  Serial.print(controller[0]);
  Serial.print("\ty:");
  Serial.print(controller[1]);
  Serial.print("\tshot:");
  Serial.print(controller[2]);
  Serial.print("\tweaponEcost:");
  Serial.print(tank.getCurrentWeapon().getEcost());
  Serial.print("\tweaponDamage:");
  Serial.println(tank.getCurrentWeapon().getDamage());
}

//*********( THE END )***********



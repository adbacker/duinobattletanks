/* YourDuinoStarter Example: nRF24L01 Receive Joystick values

 - WHAT IT DOES: Receives data from another transceiver with
   2 Analog values from a Joystick or 2 Potentiometers
   Displays received values on Serial Monitor
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
   
 - V1.00 11/26/13
   Based on examples at http://www.bajdi.com/
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <IRremote.h>

/*-----( Declare Constants and Pin Numbers )-----*/


/////////////////////////////////////////
// IR pin assignments BEGIN
/////////////////////////////////////////
int FRONT_SHIELD_PIN = 4;
int LEFT_SHIELD_PIN = 8;
int RIGHT_SHIELD_PIN = 7;
int BACK_SHIELD_PIN = 2;

IRrecv shield_front(FRONT_SHIELD_PIN);
//IRrecv shield_back(BACK_SHIELD_PIN);
IRrecv shield_left(LEFT_SHIELD_PIN);
IRrecv shield_right(RIGHT_SHIELD_PIN);
decode_results front_results;
//decode_results back_results;
decode_results left_results;
decode_results right_results;

decode_results results;

unsigned long _lastHitTime = 0L;

/////////////////////////////////////////
// IR pin assignments END
/////////////////////////////////////////


bool justHit=false;


void setup()   /****** SETUP: RUNS ONCE ******/
{
  
  Serial.begin(57600);
  delay(1000);
  
  // IR setup BEGIN  
  Serial.println("starting IR...");
  shield_front.enableIRIn(); // Start the receiver
  //shield_back.enableIRIn(); // Start the receiver
  shield_left.enableIRIn(); // Start the receiver
  shield_right.enableIRIn(); // Start the receiver
  // IR setup END
  
  Serial.println("Nrf24L01 Receiver Starting");
  
}//--(end setup )---




void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  pollReceivers();
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/



/*-----( Declare User-written Functions )-----*/
void registerHit(long encodedshot, int side)
{
  //check to see if it's been more than 100ms since the last hit
  //if so, reject it out of hand
  unsigned long now = millis();
  unsigned long timeSinceLastHit = now - _lastHitTime;
  if (timeSinceLastHit > 200)
  {
    _lastHitTime = now;
    if (encodedshot > 0L && encodedshot < 4096L)
    {  
      Serial.print("dumping...just shot with:");
      Serial.print(encodedshot);
      Serial.print(" on the ");
      Serial.println(side);
    }
  }
}


void pollReceivers()
{
  if (shield_front.decode(&front_results)) {
    //Serial.println(front_results.value, HEX);
    //dump(&front_results,0,'f');
    //if (front_results.decode_type == SONY) {
    long encoded = front_results.value;
    registerHit(encoded,0);
    front_results.value = -1;
   // }
    shield_front.resume(); // Receive the next value
  }
  /*
  if (shield_back.decode(&back_results)) {
    //Serial.println(back_results.value, HEX);
//    dump(&back_results,1,'b');
    //if (front_results.decode_type == SONY) {

    long encoded = back_results.value;
    registerHit(encoded,1);
    back_results.value = -1;
   // }
    shield_back.resume(); // Receive the next value
  }*/
  
  if (shield_left.decode(&left_results)) {
    //Serial.println("shield_left hit");
    //Serial.println(left_results.value, HEX);
    //dump(&left_results,2,'l');
    //if (front_results.decode_type == SONY) {

    registerHit(left_results.value,2);
    left_results.value = -1;
   // }
    shield_left.resume(); // Receive the next value
  }
  
  if (shield_right.decode(&right_results)) {
    //Serial.println("shield_right hit");
    //Serial.println(right_results.value, HEX);
    //dump(&right_results,3,'r');
    //if (front_results.decode_type == SONY) {

    registerHit(right_results.value,3);
    right_results.value = -1;
    //}  
    shield_right.resume(); // Receive the next value
    }
  
}

//*********( THE END )***********


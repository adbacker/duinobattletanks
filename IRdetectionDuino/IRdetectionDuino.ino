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
#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>


/////////////////////////////////////////////
//  EasySend software serial setup BEGIN
/////////////////////////////////////////////

SoftwareSerial mySerial(4, 5);
SoftEasyTransfer ET; 

//create object

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  long encoded;
  int side;

};
SEND_DATA_STRUCTURE mydata;

/////////////////////////////////////////////
//  EasySend software serial setup BEGIN
/////////////////////////////////////////////


/////////////////////////////////////////
// IR pin assignments BEGIN
/////////////////////////////////////////
IRsend irsend; // irsend == cannon!

int FRONT_SHIELD_PIN = 6;
int BACK_SHIELD_PIN = 7;
int LEFT_SHIELD_PIN = 8;
int RIGHT_SHIELD_PIN = 9;

IRrecv shield_front(FRONT_SHIELD_PIN);
IRrecv shield_back(BACK_SHIELD_PIN);
IRrecv shield_left(LEFT_SHIELD_PIN);
IRrecv shield_right(RIGHT_SHIELD_PIN);

decode_results front_results;
decode_results back_results;
decode_results left_results;
decode_results right_results;


unsigned long _lastHitTime = 0L;

/////////////////////////////////////////
// IR pin assignments END
/////////////////////////////////////////


bool justHit=false;


void setup()   /****** SETUP: RUNS ONCE ******/
{

  
  // IR setup BEGIN  
  shield_front.enableIRIn(); // Start the receiver
  shield_back.enableIRIn(); // Start the receiver
  shield_left.enableIRIn(); // Start the receiver
  shield_right.enableIRIn(); // Start the receiver
  // IR setup END
  //Serial.begin(57600);

   Serial.println("starting mySerial..");
   mySerial.begin(9600);
   ET.begin(details(mydata), &mySerial);


  
}//--(end setup )---




void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  
  unsigned long now = millis();
  if (now%1000 == 0) {
    Serial.print("polling...");
    Serial.print(now/1000);
    Serial.println("sec up..");   
  }
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
    //make sure we're sending good values
    //if (encodedshot > 0L && encodedshot < 4096L)
    //{  
      mydata.encoded = encodedshot;
      mydata.side = side;
      Serial.print("encoded: ");
      Serial.print(mydata.encoded);
      Serial.print("\t side: ");
      Serial.println(mydata.side); 
      //send the data
      ET.sendData();
    //}
  }
}


void pollReceivers()
{
  if (shield_front.decode(&front_results)) {
    if (front_results.decode_type == SONY) { //if we can't decode the type, it's not a good packet...
      long encoded = front_results.value;
      registerHit(encoded,0);
    }
    shield_front.resume(); // Receive the next value
  }
  
  if (shield_back.decode(&back_results)) {
    if (back_results.decode_type == SONY) { //if we can't decode the type, it's not a good packet...
      long encoded = back_results.value;
      registerHit(encoded,1);
    }
    shield_back.resume(); // Receive the next value
  }
  
  if (shield_left.decode(&left_results)) {
    if (left_results.decode_type == SONY) { //if we can't decode the type, it's not a good packet...
      long encoded = left_results.value;
      registerHit(left_results.value,2);
    }
    shield_left.resume(); // Receive the next value
  }
  
  
  if (shield_right.decode(&right_results)) {
    if (right_results.decode_type == SONY) { //if we can't decode the type, it's not a good packet...
      long encoded = right_results.value;
      registerHit(right_results.value,3);
    }
    shield_right.resume(); // Receive the next value
  }
  
}

//*********( THE END )***********


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
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <IRremote.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SoftEasyTransfer.h>
SoftwareSerial mySerial(5, 6);

/*   For Arduino 22 and older, do this:   */
//#include <NewSoftSerial.h>
//NewSoftSerial mySerial(2, 3);



//create object
SoftEasyTransfer ET;

struct RECEIVE_DATA_STRUCTURE{
	//put your variable definitions here for the data you want to send
	//THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
	int blinks;
	int pause;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

/*-----( Declare Constants and Pin Numbers )-----*/


/////////////////////////////////////////
// Motor controller pin assignments BEGIN
/////////////////////////////////////////
#define ENA 5
#define ENB 6

#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19


//joystick tweaking values
#define YAXIS_DEADZONE 40
#define XAXIS_DEADZONE 40
#define YAXIS_CENTER 560
#define XAXIS_CENTER 500

 //PWM value at which point he motor actually starts to move
 //so don't bother to try and set the speed lower..you'll just
 //get PWM whine
#define MIN_MOTOR_SPEED 60
#define MAX_TURN_SPEED 180 //maximum speed at which to turn. ie: max turn speed when stopped

int xmin,xmax,ymin,ymax;

long xthrottle;
long ythrottle;
int turnmax=0; //maximum turn rate, calculated dynamically based on speed

float speedpercent;
/////////////////////////////////////////
//   MOTOR CONTROLLER assignments END
/////////////////////////////////////////


/////////////////////////////////////////
// IR pin assignments BEGIN
/////////////////////////////////////////
IRsend irsend; // irsend == cannon!

int FRONT_SHIELD_PIN = 8;
int LEFT_SHIELD_PIN = 4;
int RIGHT_SHIELD_PIN = 7;
int BACK_SHIELD_PIN = 2;

//IRrecv shield_front(FRONT_SHIELD_PIN);
//IRrecv shield_back(BACK_SHIELD_PIN);
//IRrecv shield_left(LEFT_SHIELD_PIN);
//IRrecv shield_right(RIGHT_SHIELD_PIN);
//decode_results front_results;
//decode_results back_results;
//decode_results left_results;
//decode_results right_results;
unsigned long _lastHitTime = 0L;


/////////////////////////////////////////
// IR pin assignments END
/////////////////////////////////////////


/////////////////////////////////////////
// RADIO assignments BEGIN
/////////////////////////////////////////

#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
RF24Network network(radio);
// Address of our node
const uint16_t this_node = 3;
// Address of the other node
const uint16_t other_node = 0;

long controller[3];  // 3 element array holding Joystick readings
long hit[2]; //2 element array holding the encoded hit, and the side that was hit

int lastIrHit=0;
/////////////////////////////////////////
// RADIO assignments END
/////////////////////////////////////////

bool justHit=false;


void setup()   /****** SETUP: RUNS ONCE ******/
{

  Serial.begin(57600);
  delay(1000);
  
  // MOTOR CONTROLLER setup BEGIN
  //pinMode(ENA, OUTPUT);
  //pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  // MOTOR CONTROLLER setup END
  
  //Easy Transfer library setup
  mySerial.begin(9600);
  //start the library, pass in the data details and the name of the serial port.
  ET.begin(details(mydata), &mySerial);
  //END Easy transfer library setup

  //Serial.println("starting IR...");
  //shield_front.enableIRIn(); // Start the receiver
  //shield_back.enableIRIn(); // Start the receiver
  //shield_left.enableIRIn(); // Start the receiver
  //shield_right.enableIRIn(); // Start the receiver
  // IR setup END
  
  
  //setup joystick min/max values for the 
  //"dead zone" around the center
  xmin = XAXIS_CENTER-XAXIS_DEADZONE;
  xmax = XAXIS_CENTER+XAXIS_DEADZONE;
  ymin = YAXIS_CENTER-YAXIS_DEADZONE;
  ymax = YAXIS_CENTER+YAXIS_DEADZONE;
  
  
  Serial.println("Nrf24L01 Receiver Starting");
  
  // radio setup BEGIN
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  
  controller[0]=0;
  controller[1]=0;
  controller[2]=0;
  // radio setup END
  allMotorStop();
}//--(end setup )---




void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  // Pump the network regularly
  network.update(); //network pump
  check_network();  //check for network updates
  if (controller[0] == 0)
  {
    //allMotorStop();
  }
  //check to see if we've been hit
  pollReceivers();

}//--(end main loop )---



/*-----( Declare User-written Functions )-----*/

void receive(int numBytes) {}

void registerHit(long encodedshot, int side)
{
  //check to see if it's been more than 100ms since the last hit
  //if so, reject it out of hand
  unsigned long now = millis();
  unsigned long timeSinceLastHit = now - _lastHitTime;
  if (timeSinceLastHit >200)
  {
    //if (encodedshot > 0L && encodedshot < 4096L)
    //{
      _lastHitTime = now;
      Serial.print("dumping...just shot with:");
      Serial.print(encodedshot);
      Serial.print(" on the ");
      Serial.println(side);
      hit[0]=encodedshot;
      hit[1]=side;
      RF24NetworkHeader header(other_node);
      bool ok = network.write(header,&hit,sizeof(hit));
    //}
  }
}


void pollReceivers()
{
	if (ET.receiveData()){
		//this is how you access the variables. [name of the group].[variable name]
		//since we have data, we will blink it out. 
		Serial.print("pause: ");
		Serial.print(mydata.pause);
		Serial.print("\t blink: ");
		Serial.println(mydata.blinks);
	}
	
	
	
	/*if (shield_front.decode(&front_results)) {
    //Serial.println(front_results.value, HEX);
    //dump(&front_results,0,'f');
    //if (front_results.decode_type == SONY) {
    long encoded = front_results.value;
    registerHit(encoded,0);
    front_results.value = -1;
   // }
    shield_front.resume(); // Receive the next value
  }
  
  if (shield_back.decode(&back_results)) {
    //Serial.println(back_results.value, HEX);
//    dump(&back_results,1,'b');
    //if (front_results.decode_type == SONY) {

    long encoded = back_results.value;
    registerHit(encoded,1);
    back_results.value = -1;
   // }
    shield_back.resume(); // Receive the next value
  }
  
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
  */
}

void throttlemix(int xval, int yval) {
//xval == left/right (steering) , yval == fwd/back (throttle)

  //first, figure out how fast we're going forwards or back
  int ymix=0;
  if (yval < ymin) { //if < 500, we're goin backwards
    ymix = map(yval,550,50,-MIN_MOTOR_SPEED,-255); // 0 to -255 for reverse
  }
  else if (yval > ymax) {
    ymix = map(yval,570,1023,MIN_MOTOR_SPEED,255);
  }
  Serial.print("\tymix:\t");
  Serial.print(ymix);
  
  //set the base motor speed values to whatever we calculated above
  int rmotor=ymix;
  int lmotor=ymix;
  Serial.print("\trmotor raw:\t");
  Serial.print(rmotor);
  Serial.print("\tlmotor raw:\t");
  Serial.print(lmotor);
  
  
  int turnmagnitude=0;
  int turndir=0; //1 = left, 0 = right
  if (xval < xmin) {
    //turning left
    turnmagnitude = map(xval,490,50,0,MAX_TURN_SPEED);
    turndir = -1;
  }
  else if (xval > xmax) {
    //turning right
    turnmagnitude = map(xval,520,1023,0,MAX_TURN_SPEED);
    turndir = 1;
  }
  
  int maxTurnAtCurrentSpeed=map(abs(ymix),MIN_MOTOR_SPEED,255,MAX_TURN_SPEED,MIN_MOTOR_SPEED);
  int scaledTurnMagnitude=map(turnmagnitude,MIN_MOTOR_SPEED,MAX_TURN_SPEED,MIN_MOTOR_SPEED,maxTurnAtCurrentSpeed);
  
  Serial.print("\tturnmag:\t");
  Serial.print(turnmagnitude);
  Serial.print("\tscaledturnmag:\t");
  Serial.print(scaledTurnMagnitude);
  Serial.print("\tturndir:\t");
  Serial.print(turndir);

  
  //apply calculated "turn magnitude" changes to the left and right motor magnitudes
  if (turndir == 1) { //if turning left
    lmotor = lmotor - scaledTurnMagnitude; //the left wheel goes slower
    rmotor = rmotor + scaledTurnMagnitude; //the right wheel goes faster
  } else if (turndir == -1) { //turning to the right
    lmotor = lmotor + scaledTurnMagnitude; //left wheel goes faster
    rmotor = rmotor - scaledTurnMagnitude; //right wheel goes slower
  }
    
  Serial.print("\tl:\t");
  Serial.print(lmotor);
  Serial.print("\tr\t");
  Serial.println(rmotor);
  
    
  //set lmotor direction
  if (lmotor < 0) { //motor's goin backwards
    digitalWrite(IN1,LOW); //motora, reverse
    digitalWrite(IN2,HIGH); //motora, reverse 
  } 
  else { //motor's goin forwards
    digitalWrite(IN1,HIGH); //motora, fwd
    digitalWrite(IN2,LOW); //motora, fwd
  }
  //analogWrite probably handles values > 255, but let's catch it just in case
  if (lmotor > 255 || lmotor < -255) { 
    lmotor = 255; 
  }
  analogWrite(ENA,abs(lmotor)); //finally, set the speed for the left motor
  
  
  //set rmotor direction
  if (rmotor < 0) { //motor's goin backwards
    digitalWrite(IN3,HIGH); //motorb, reverse
    digitalWrite(IN4,LOW); //motorb, reverse 
  } 
  else {
    digitalWrite(IN3,LOW); //motorb fwd
    digitalWrite(IN4,HIGH); //motorb fwd
  }
  //analogWrite probably handles values > 255, but let's catch it just in case
  if (rmotor > 255 || rmotor < -255) { 
    rmotor = 255; 
  } 
  analogWrite(ENB,abs(rmotor)); //finally, set the speed for the right motor
}

void allMotorStop()
{
  digitalWrite(IN1,LOW); //motorb fwd
  digitalWrite(IN2,LOW); //motorb fwd
  digitalWrite(IN3,LOW); //motorb fwd
  digitalWrite(IN3,LOW); //motorb fwd
}


void check_network() {
  // Is there anything ready for us?
  while ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&controller,sizeof(controller));
  }
  if (controller[0] !=0 || controller[1] !=0)
  {
    //throttlemix(controller[0],controller[1]);
  }
  if (controller[2] !=0)
  {
    //Fire!
    long encoded = long(controller[2]);
    //send the shot back to the sending tank for testing purposes
    //hit[0]=encoded;
    //justHit=true;
    irsend.sendSony(encoded,32);  
  }
  
}

void debugController()
{
  Serial.print("X = ");      
  Serial.print(controller[0]);
  Serial.print("\tY =\t");      
  Serial.print(controller[1]);
  Serial.print("\tFIRE! =\t");      
  Serial.println(controller[2]);
}

//*********( THE END )***********


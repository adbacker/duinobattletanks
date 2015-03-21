
/*-----( Import needed libraries )-----*/

#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>
#include "printf.h"


//#define myuno
#define mymini


/////////////////////////////////////////////
//  EasySend software serial setup BEGIN
/////////////////////////////////////////////


//I connected different ports on each arduino..
#ifdef mymini
SoftwareSerial mySerial(4, 5); //just ports that I chose, 
                               //you can choose any!
#endif

#ifdef myuno
SoftwareSerial mySerial(7, 8);
#endif

SoftEasyTransfer ET; 

struct XFER_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int encoded;
  int side;

};
XFER_DATA_STRUCTURE myData;

unsigned long now;
int sec;
int incomingByte = 0;   // for incoming serial data


void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(57600);
  printf_begin();

  Serial.println("starting mySerial..");
  mySerial.begin(57600);
  ET.begin(details(myData), &mySerial);

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //heartbeat..useful for making sure things initialized
  /*
  now = millis();
  if (now%10000 == 0) {
    sec = now/1000;
    //Serial.println(sec);
    printf("%i sec elapsed",sec);
  }*/
  
  //receive data
  if (ET.receiveData()){
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out. 
    printf("RECEIVED: encoded: %i, side: %i\n",myData.encoded, myData.side);   
  }

  if (Serial.available() > 0) { //send if someone types into the serial console
    incomingByte = Serial.read();

    //send data
    myData.encoded = random(255);
    myData.side = random(4);
    printf("SENDING: encoded: %i, side: %i\n",myData.encoded, myData.side);   
    //send the data
    ET.sendData();
  }
  //you should make this delay shorter then your transmit delay or else messages could be lost
  //delay(250);
  

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/



//*********( THE END )***********



#define NODE1
//#define NODE2


/*-----( Import needed libraries )-----*/
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "printf.h"

//create object
struct NET_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int controller_x;
  int controller_y;
};

//give a name to the group of data
NET_DATA_STRUCTURE myNetData;

/*-----( Declare Constants and Pin Numbers )-----*/

/////////////////////////////////////////
// RADIO assignments BEGIN
/////////////////////////////////////////
#define CE_PIN 9
#define CSN_PIN 10

#ifdef NODE1
#define this_node 00
#define other_node 01
#endif

#ifdef NODE2
#define this_node 01
#define other_node 00
#endif

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
RF24Network network(radio);
/////////////////////////////////////////
// RADIO assignments END
/////////////////////////////////////////



void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(57600);
  
  Serial.println("Nrf24L01 Receiver Starting");
  // radio setup BEGIN
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  
}//--(end setup )---




void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  // Pump the network regularly
  network.update(); //network pump
  check_network();  //check for network updates
  
  if (Serial.available() > 0) { //if someone hits a key..
    myNetData.controller_x = random(1024);
    myNetData.controller_y = random(1024);
    printf("sending xval: %i, yval: %i to %i",myNetData.controller_x, myNetData.controller_y, other_node);
    RF24NetworkHeader header(other_node);
    bool ok = network.write(header,&myNetData,sizeof(myNetData));
    if (ok) {
      Serial.println("send succeeded!");
    } else {
      Serial.println("ooohh...you FAILED! (well, your send did anyway)");
    }
  }
}//--(end main loop )---



/*-----( Declare User-written Functions )-----*/

void check_network() {
  // Is there anything ready for us?
  while ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&myNetData,sizeof(myNetData));
    printf("Received xval: %i, yval: %i from %i",myNetData.controller_x, myNetData.controller_y, other_node);
  }
}


//*********( THE END )***********


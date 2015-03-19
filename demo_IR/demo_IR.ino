//IRdemocode.ino

#include <IRremote.h>
#include "printf.h"

//below line only necessary if sending
IRsend irsend; // irsend == IR led _always_ on pin 3

//below 3 lines are only necessary if receiving
int IR_RECEIVE_PIN = 6;
IRrecv irreceiv(IR_RECEIVE_PIN);
decode_results results;

//decoding the sent value?
boolean decodeit = true;
byte incomingByte=0; //necessary for serial read
long irSentValue=0L;
long encoded=0L;

int badcount=0;
int totalcount=0;

void setup() {
	Serial.begin(57600);
  printf_begin();
  randomSeed(analogRead(0));
  Serial.println("starting IR ...");
	irreceiv.enableIRIn();  //start the IR receiver
}

void loop() {  //BEGIN main loop

//        if (millis()%2000 == 0) {
//          printf("up %i seconds\n",millis()/1000);
//        }
  if (irreceiv.decode(&results)) {

      //decode the sent value
      byte dec_tankId=0;
      byte dec_weapId=0;
      byte dec_power=0;
      byte dec_checksum=0;
      
      decodeShot(results.value,&dec_tankId, &dec_weapId, &dec_power, &dec_checksum);
      
      //calculate what the checksum should be...
      byte calc_checksum = dec_tankId ^ dec_weapId;
      calc_checksum ^= dec_power;

      if (calc_checksum != dec_checksum || (!results.decode_type == SONY)) {
        badcount++;
        //printf("*** BAD!! *** calc checksum %i does not match decoded checksum %i!\n",calc_checksum,dec_checksum);
      } else {
        //int percentbad = float(badcount)/float(totalcount) * 100;
        //printf("tank:%i\tweap:%i\tpow:%i\tdcs:%i\tccs:%i\tttl:%i\tbad:%i\tpercent:%i\n",dec_tankId, dec_weapId, dec_power, dec_checksum, calc_checksum, totalcount, badcount, percentbad);
        printf("tank:%i\t weapon:%i\t power:%i\t decoded checksum:%i\t calculated checksum:%i\n",dec_tankId, dec_weapId, dec_power, dec_checksum, calc_checksum);
      }
    irreceiv.resume(); // Receive the next value
  }
// *** RECEIVE IR stuff END ***
// *** SEND IR stuff BEGIN ***

  //if someone types into serial console
  //send 12345
/*  if (Serial.available() > 0) {
    incomingByte = Serial.read(); //gotta do this, else the byte stays in the buffer
    //just got something
    long sendme = 12345L;
    printf("Sending %i\n",sendme);
    irsend.sendSony(sendme,32);
  }

  //if someone types into serial console
  //send a pre-defined shot
  if (millis()%1000 == 0) {
    //if (Serial.available() > 0) {
    incomingByte = Serial.read(); //gotta do this, else the byte stays in the buffer
    byte tankId = 2;//random(4);
    byte weapId = 3;//random(16);
    byte power = 4; //random(10);
    long encodedshot = encodeShot(tankId, weapId, power);
    printf("sending tank: %i, weap: %i, power: %i encoded to %u\n",tankId,weapId,power,encodedshot);
    irsend.sendSony(encodedshot,32);
  }*/

// *** SEND IR stuff END ***
} // **** END MAIN LOOP ****



//basic encoding, 3 8 bit values + 8 bit shotId + 8 bit checksum
//note that the checksum must be <= the sum of the first 3 < 255 ...
//not a problem, as we don't have hundreds of tanks, weapons, or power values
long encodeShot(byte tankId, byte weapId, byte power)
{
  long _encoded = 0; 
  byte checksum = tankId ^ weapId; //xor the first two
  checksum ^= power; //then the last
  //final structure -> first 4 == player, second 4 == weapon, third 4 == power, last 8 == checksum
  _encoded = _encoded + tankId; 
  //now shift to the left to make room for weapon
  _encoded = _encoded << 4;  
  //now and in the weapon value
  _encoded = _encoded | weapId;
  //shift everything to the left again 4 spots
  _encoded = _encoded << 4; 
  //and add the power
  _encoded = _encoded | power;
  //bump left again
  _encoded = _encoded << 4;
  _encoded = _encoded | checksum;
  return _encoded;
}



//basic decoding
void decodeShot(long encoded,byte *tankId, byte *weapId, byte *power, byte *checksum)
{
  *checksum = B00001111 & encoded;
  encoded = encoded >> 4;
  *power = B00001111 & encoded;  //grab the power ... remember, we only used 4 bits!
  encoded = encoded >> 4;   //shift the weapon into the next 4
  *weapId = B00001111 & encoded;   //grab that weapon
  encoded = encoded >> 4;   //shift player into the next 4
  *tankId = B00001111 & encoded; 
}



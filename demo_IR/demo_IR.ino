//IRdemocode.ino

#include <IRremote.h>


//below line only necessary if sending
IRsend irsend; // irsend == IR led _always_ on pin 3


//below 3 lines are only necessary if receiving
int IR_RECEIVE_PIN = 6;
IRrecv irreceiv(IR_RECEIVE_PIN);
decode_results ir_decode_results;


//decoding the sent value?
boolean decodeit = false;


void setup() {
	Serial.begin(57600);
	irreceiv.enableIRIn();  //start the IR receiver
}



void loop() {  //BEGIN main loop

	// *** RECEIVE IR stuff BEGIN ***

	long irSentValue = checkForIr();
	//if our return value was nonzero...process!
	if (irSentValue != 0) {
		Serial.print("Received: ");
		Serial.println(irSentValue);

		if (decodeit) {
			//decode the sent value
			byte dec_tankId=0;
			byte dec_weapId=0;
			byte dec_power=0;
			byte dec_checksum=0;

			decodeShot(irSentValue,&dec_tankId, &dec_weapId, &dec_power, &dec_checksum);
			//now double-check that the received shot was good
			byte calculatedChecksum = dec_tankId + dec_weapId + dec_power;
			if (calculatedChecksum != dec_checksum) {			
				printf("bad checksum. sent: %i, calculated: %i\n",dec_checksum,calculatedChecksum);
			}
			else {
				printf("tank: %i, weap: %i, power: %i\n",dec_tankId, dec_weapId, dec_power);
			}
		}

	}
	// *** RECEIVE IR stuff END ***



	// *** SEND IR stuff BEGIN ***

	//if someone types into serial console
	//send 12345
	if (Serial.available() > 0) {
		//just got something
		Serial.println("Sending...");
		irsend.sendSony(12345L,32);
	}

	//if someone types into serial console
	//send a pre-defined shot
	if (Serial.available() > 0) {
		byte tankId = 3;
		byte weapId = 2;
		byte power = 10;
		unsigned long encodedshot = encodeShot(tankId, weapId, power);
		irsend.sendSony(encodedshot,sizeof(encodedshot));
	}
	
	// *** SEND IR stuff END ***
} // **** END MAIN LOOP ****


long checkForIr() {
	long encoded = 0L;
	if (irreceiv.decode(&ir_decode_results)) {  //do we got some IR data waiting for us?
	    if (ir_decode_results.decode_type == SONY) { //we used the sendSony() method, so it 
	    											 //should decode as the SONY type..
	    											 //this helps cut down on spurious input
	      encoded = ir_decode_results.value;
	    }
	}
    irreceiv.resume(); // start listening again
    return encoded; //send back 0 (or whatever we got)
}



//basic encoding, 3 8 bit values + 8 bit shotId + 8 bit checksum
//note that the checksum must be <= the sum of the first 3 < 255 ...
//not a problem, as we don't have hundreds of tanks, weapons, or power values
unsigned long encodeShot(byte tankId, byte weapId, byte power)
{
  unsigned long _encoded = 0; 
  byte checksum = tankId + weapId + power;

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
  //last shift..this time by 8, because checksum needs 
  //a full 8 bits
  _encoded = _encoded << 8;
  //add a calculated checksum to the end
  _encoded = _encoded | checksum;
  return _encoded;
}



//basic decoding
void decodeShot(unsigned int encoded,byte *tankId, byte *weapId, byte *power, byte *checksum)
{
  //first grab the checksum off the end
  *checksum = B11111111 & encoded;
  //shift everything over by 8, dropping checksum off
  encoded = encoded >> 8;
  //grab the power ... remember, we only used 4 bits!
  *power = B00001111 & encoded;
  //shift the weapon into the next 4
  encoded = encoded >> 4;
  //grab that weapon
  *weapId = B00001111 & encoded;
  //shift player into the last 4
  encoded = encoded >> 4;
  *tankId = encoded;
}



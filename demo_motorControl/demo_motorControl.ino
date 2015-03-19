#include <Arduino.h>
#include "motor.h" // see motor.h for custom e
#include "printf.h"

/////////////////////////////////////////
// Motor controller pin assignments BEGIN
/////////////////////////////////////////
#define ENA 5 // enable - ie turn on - motor A
#define ENB 6 // enable motor B

#define IN1 16 // motor A input 1
#define IN2 17 // motor A input 2
#define IN3 18 // motor B input 1
#define IN4 19 // motor B input 2

int motora_speed=0;
int motorb_speed=0;


/////////////////////////////////////////
//   MOTOR CONTROLLER assignments END
/////////////////////////////////////////
int serialIn=0; //necessary for serial read

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(57600);
  printf_begin();

  // MOTOR CONTROLLER setup BEGIN
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  // MOTOR CONTROLLER setup END
  Serial.println("stopping all motors");
  allMotorStop();
  printDir();

}//--(end setup )---




void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  
  //set motor A's DIRECTION
  digitalWrite(IN1,LOW); //motora rev
  digitalWrite(IN2,HIGH); //motora rev
  
  //set motor A's SPEED.  (0-255)
  analogWrite(ENA,70);
  Serial.println("motor @ 70 fwd");
  delay(3000); //wait a second...

  analogWrite(ENA,150);
  Serial.println("motor @ 150 fwd");
  delay(3000); //wait another second...
  
  analogWrite(ENA,255); //full tilt boogie!
  Serial.println("motor @ 255 fwd");
  delay(3000);

  //now we reverse
  //first, stop the motor
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  Serial.println("motor stop, regardless of ENA status");
  delay(3000);

  //then reverse it
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(ENA,255);
  Serial.println("motor @ 255 rev");
  delay(3000);

  //then STOP
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  Serial.println("motor stop, regardless of ENA status...");
  delay(3000);
  
  
  
  
  
  
  
  /*
  for (int i=0;i<255;i++) {
    motora_speed = i;
    forward(MOTORA,motora_speed);
    printDir();
    delay(40);
  }
  for (int i=255;i>0;i--) {
    motora_speed = i;
    forward(MOTORA,motora_speed);
    printDir();
    delay(40); 
  }
  */
  
  
  
  
  
  /*
  
  if (Serial.available() > 0) {
    serialIn=Serial.parseInt();
  }

  if (serialIn == 5) {
    allMotorStop();
    printDir();
    serialIn=0;
  }

  if (serialIn == 1 || serialIn == 2) {
    if (serialIn == 1) {
      motora_speed += 30;
    } else {
      motora_speed -= 30;
    }
    
    if (motora_speed > 0) {
      forward(MOTORA, motora_speed);
    } else {
      reverse(MOTORA, abs(motora_speed));
    } 
    printDir();
    serialIn=0;
  }

  
  if (serialIn == 3 || serialIn == 4) {
    if (serialIn == 3) {
      motorb_speed += 30;
    } else {
      motorb_speed -= 30;
    }
    
    if (motorb_speed > 0) {
      forward(MOTORB, motorb_speed);
    } else {
      reverse(MOTORB, abs(motorb_speed));
    } 
    printDir();
    serialIn=0;
  }
  */

}//--(end main loop )---



/*-----( Declare User-written Functions )-----*/


void reverse(motor whichmotor, int howfast) {
  switch (whichmotor) {
    case MOTORA: 
      digitalWrite(IN1,LOW); //motora rev
      digitalWrite(IN2,HIGH); //motora rev
      analogWrite(ENA,howfast);
      break;
    case MOTORB:
      digitalWrite(IN3,HIGH); //motorb rev
      digitalWrite(IN4,LOW); //motorb rev
      analogWrite(ENB,howfast);
      break;
    default:
      break;
  }
}

void forward(motor whichmotor, int howfast) {
  switch (whichmotor) {
    case MOTORA: 
      digitalWrite(IN1,HIGH); //motora fwd
      digitalWrite(IN2,LOW); //motora fwd
      analogWrite(ENA,howfast);
      break;
    case MOTORB:
      digitalWrite(IN3,LOW); //motorb fwd
      digitalWrite(IN4,HIGH); //motorb fwd
      analogWrite(ENB,howfast);
      break;
    default:
      break;
  }
}

void allMotorStop()
{
  // writing all control pins HIGH or LOW will
  // bring the motor to a stop
  // usually all high == brake, all low == coast
  digitalWrite(IN1,LOW); //motora 
  digitalWrite(IN2,LOW); //motora 
  digitalWrite(IN3,LOW); //motorb 
  digitalWrite(IN4,LOW); //motorb
}

void printDir() {
  printf("A:%i\tB:%i\t\t",motora_speed,motorb_speed);
  Serial.println("5::allstop\t1::A speed up\t2::A slow down\t3::B speed up\t4::B slow down");
}



//*********( THE END )***********


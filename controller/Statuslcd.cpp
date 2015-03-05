#include "Arduino.h"
#include "Statuslcd.h"
#include "Tank.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


Statuslcd::Statuslcd()
{
  //lcd.backlight();
  //lcd.setCursor(0,0);
  //lcd.print("starting...");
  //Serial.println("status lcd initialized");
}


void Statuslcd::update(Tank _tank, LiquidCrystal_I2C &lcd)
{
  unsigned long now = millis();
  unsigned long msSince = now - _lastTime;
  //how often to regen a point of energy
  //hardcoded 200ms lcd refresh interval
  if (msSince > 100)
  {
    //reset the timer
    _lastTime = now;
    //lcd.clear();
    if (_tank.isDead()==false)
    {
      lcd.setCursor(0,0);
      lcd.print(buildWeaponLine(_tank.getCurrentWeapon().getId(),_tank.getEnergy()));
      lcd.setCursor(0,1);
      lcd.print(buildShieldLine(_tank.getShield().getF(),_tank.getShield().getB(),_tank.getShield().getL(),_tank.getShield().getR()));
      //Serial.println(buildShieldLine(_tank.getShield().getF(),_tank.getShield().getB(),_tank.getShield().getL(),_tank.getShield().getR()));
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("*** KABOOM!! ***");
      lcd.setCursor(0,1);
      lcd.print("**YOU ARE DEAD**");
    }
  }
}


String Statuslcd::buildWeaponLine(byte activeGun, int currentEnergy)
{
  String weaponline;
  //0 = Machinegun, 1 = Laser, 2 = Plasma cannon
  if (B0 == activeGun) {
    weaponline = "PulseE20:D10 E";
  } else if (byte(1) ==activeGun) {
    weaponline = "LaserE40:D20 E";
  } else if (byte(2) == activeGun) {
    weaponline = "GammaE90:D40 E";
  }
  String energy = String(currentEnergy);
  weaponline = weaponline + energy;
  
  return weaponline;
}

//front, rear, left, and right current shield values
String Statuslcd::buildShieldLine(int f, int b, int l, int r) {
  String front= String(f);
  String back =  String(b);
  String left =  String(l);
  String right = String(r);
  String shieldLine = "F" + front + " B" + back + " L" + left + " R" + right;
  return shieldLine;
}


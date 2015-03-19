#include "Arduino.h"
#include "Tank.h"
#include "Weapon.h"
#include "Shield.h"
#include "Shot.h"

//default no-constructor initializer
Tank::Tank()
{
}

Tank::Tank(byte id,int recharge, int maxEnergy)// : _shield(0,0),_weap1(B0,"A",'a',1,1),_weap2(B0,"A",'a',1,1),_weap3(B0,"A",'a',1,1)
{
  _id = id;
  _recharge = recharge;
  _maxEnergy = maxEnergy;
  _energy = maxEnergy;

  _shield = Shield(99,2000L);   //initialize the shield.  recharge @ 1 unit/every 2 sec

  //load yer weapons!
  _weap1 = Weapon(byte(0), "Pulse",'P',20,10);
  _weap2 = Weapon(byte(1), "Laser",'L',40,20);
  _weap3 = Weapon(byte(2), "Gamma",'G',90,40);
  _pCurrentWeapon = &_weap1;
  _lastEnergyChargeTime=0L;
  _isDead = false;  //I'm not dead yet..
  
  Serial.println("tank instantiated...");
}

byte Tank::getId()
{
  return _id;
}

int Tank::getEnergy()
{
  return _energy;
}

Shield Tank::getShield()
{
  return _shield;
}

void Tank::regenShield()
{
  _shield.regen();
}

Weapon Tank::getCurrentWeapon()
{
  return *_pCurrentWeapon;
}

long Tank::fire()
{
  //check to see if the tank has enough energy to fire the weapon
  if (_pCurrentWeapon->getEcost() < _energy) {
    //subtract weapon ecost from current energy
    _energy = _energy - _pCurrentWeapon->getEcost();
    //calc damage - we divide by 10 because IR encoding is a byte (4 bits)
    //so has a max value of 16.  Don't worry, we multiply by 10 on the receiving end
    int damage=_pCurrentWeapon->getDamage()/10;
    Shot shot(_id,_pCurrentWeapon->getId(),byte(damage));
    return shot.getEncoded();
  }
  //tank didn't have enough energy to fire, return 0;
  return 0L;
}

//very awkward way to switch weapons, frankly
void Tank::nextWeapon()
{
  if (_pCurrentWeapon == &_weap1) {
    _pCurrentWeapon = &_weap2;
  }
  else if (_pCurrentWeapon == &_weap2)
  {
    _pCurrentWeapon = &_weap3;
  }
  else
  {
    _pCurrentWeapon = &_weap1;
  }
}


void Tank::registerHit(long encoded, int side)
{
  Shot shotReceived(encoded);
  
  //debug
/*  Serial.print("\tencoded:\t");
  Serial.print(encoded);
  Serial.print("\tplayerid:\t");
  Serial.print(shotReceived.getPlayerId());
  Serial.print("\tshotWeapId:\t");
  Serial.print(shotReceived.getWeaponId());
  Serial.print("\tshotPower:");
  Serial.println(shotReceived.getPower());*/

  //multiply power by 10 because it was divided by 10 on the 
  //shooting side.  recordHit returns TRUE if it's a killing shot eg:
  //takes the shield power to < 0
  
  //and only record a hit if it's *NOT* this tank...as the IR seems to be happy
  //to self-tag ...
  if (shotReceived.getPlayerId() != getId()) {
  boolean killHit = _shield.recordHit(side,int(shotReceived.getPower()*10));
  _isDead = killHit;
  }
}

//regenerate tank weapon energy
void Tank::regen()
{
  unsigned long now = millis();
  unsigned long echargetime = now - _lastEnergyChargeTime;
  //how often to regen a point of energy
  if (echargetime > _recharge )
  {
    //reset the timer
    _lastEnergyChargeTime = now;
    //update energy
    if (_energy < _maxEnergy) //only update if we're low 
    {
      _energy += 1;
    }
  }
}

boolean Tank::isDead()
{
  return _isDead;
}


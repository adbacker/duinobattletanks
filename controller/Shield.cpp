#include "Arduino.h"
#include "Shield.h"

//default no-constructor initializer
Shield::Shield()
{
}

Shield::Shield(int maxs, int rechargeRate)
{
  _max = maxs;
  _f=_max;
  _b=_max;
  _r=_max;
  _l=_max;
  _rechargeRate = rechargeRate;
  _lastShieldChargeTime=0L;
}

int Shield::getF() {
  return _f;
}

int Shield::getB() {
  return _b;
}

int Shield::getR() {
  return _r;
}

int Shield::getL() {
  return _l;
}


void Shield::regen()
{
  unsigned long now = millis();
  unsigned long schargetime = now - _lastShieldChargeTime;

  if (schargetime > _rechargeRate)
  {  
    _lastShieldChargeTime=now;
    _f+=1;
    _b+=1;
    _r+=1;
    _l+=1;
    if (_f>_max) { _f=_max; }
    if (_b>_max) { _b=_max; }
    if (_l>_max) { _l=_max; }
    if (_r>_max) { _r=_max; }
  }  
}
//records hit against shields.  if shield taken
//to below 0, return TRUE
boolean Shield::recordHit(int side, int damage) 
{
  boolean dead=false;
  //all these if statements are supremely sub-optimal...but they
  //do clearly demonstrate what's going on.
  //that's my story and I'm sticking to it...

  if (side == 0) 
  {
    _f = _f - damage;
    if (_f < 0) 
    {
      dead=true;
      _f = 0;
    }
  }
  else if (side == 1) {
    _b = _b - damage;
    if (_b < 0) 
    {
      dead=true;
      _b = 0;
    }
  }
  if (side == 3) {
    _r = _r - damage;
    if (_r < 0)
    {
      dead=true;
      _r = 0;
    }
  }
  if (side == 2) {
    _l = _l - damage;
    if (_l < 0) 
    {
      dead=true;
      _l = 0;
    }
  }
  return dead;
}


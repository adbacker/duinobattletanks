#include <Arduino.h>

#include "Shot.h"

Shot::Shot()
{
}

Shot::Shot(long encoded)
{
  _encoded = encoded;
  decodeShot();
}

Shot::Shot(byte playerId, byte weaponId, byte power)
{
  _playerId = playerId;
  _weaponId = weaponId;
  _power = power;
  encodeShot();
}

byte Shot::getPlayerId()
{
  return _playerId;
}

byte Shot::getWeaponId()
{
  return _weaponId;
}

byte Shot::getPower()
{
  return _power;
}

long Shot::getEncoded()
{
  return _encoded;
}

void Shot::encodeShot()
{
  _encoded = 0; 
  //final structure -> first four == player, middle 4 == weapon, last 4 == power
  _encoded = _encoded + _playerId; 
  //now shift to the left to make room for weapon
  _encoded = _encoded << 4;  
  //now and in the weapon value
  _encoded = _encoded | _weaponId;
  //shift everything to the left again 4 spots
  _encoded = _encoded << 4; 
  //and add the power
  _encoded = _encoded | _power;
}

void Shot::decodeShot()
{
  //first grab the power
  _power = B00001111 & _encoded;
  //shift the weapon into the last 4
  _encoded = _encoded >> 4;
  //grab that weapon
  _weaponId = B00001111 & _encoded;
  //shift player into the last 4
  _encoded = _encoded >> 4;
  _playerId = _encoded;
}

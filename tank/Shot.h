#ifndef Shot_h
#define Shot_h

#include <Arduino.h>

class Shot
{
  public:
    Shot();
    Shot(long encoded);
    Shot(byte playerId, byte weaponId, byte power);
    byte getPlayerId();
    byte getWeaponId();
    byte getPower();
    long getEncoded();
  private:
    long _encoded;
    byte _playerId;
    byte _weaponId;
    byte _power;
    void encodeShot();
    void decodeShot();
};


#endif

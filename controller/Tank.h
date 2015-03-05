#ifndef Tank_h
#define Tank_h

#include "Arduino.h"
#include "Weapon.h"
#include "Shield.h"
#include "Shot.h"

class Tank
{
  public:
    Tank();
    Tank(byte id,int recharge, int maxEnergy);
    byte getId(); //return tank's ID (eg player id)
    int getEnergy(); //returns tank's current energy level
    void nextWeapon(); //cycles to the next weapon
    Weapon getCurrentWeapon();
    long fire(); //encode current weapon info & return it for IR fire
    void registerHit(long encoded, int side);  //decode encoded shot & apply damage
    void regen();  //regenerate shield levels based on time, call once per game cycle
    boolean isDead(); //am I dead yet?
    Shield getShield(); //returns the tank's shield object
    void regenShield();
  private:
    byte _id;
    int _recharge;
    int _energy;
    int _maxEnergy;
    boolean _isDead;
    void initialize();
    Shield _shield;
    unsigned long _lastEnergyChargeTime; //timestamp for last time energy was refreshed
    Shot _shot;
    Weapon _weap1;
    Weapon _weap2;
    Weapon _weap3;
    Weapon* _pCurrentWeapon;
};

#endif

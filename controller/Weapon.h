#ifndef Weapon_h
#define Weapon_h

#include "Arduino.h"

class Weapon
{
  public:
    Weapon();
    Weapon(byte id, String name, char abbr, int ecost, int damage);
    byte getId();
    String getName();
    char getAbbr();
    int getEcost();
    int getDamage();
  private:
    byte _id;
    String _name;
    char _abbr;
    int _ecost;
    int _damage;
};


#endif

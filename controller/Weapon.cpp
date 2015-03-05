#include "Arduino.h"
#include "Weapon.h"

//default parameterless constructor
Weapon::Weapon()
{
}

Weapon::Weapon(byte id, String name, char abbr, int ecost, int damage)
{
  _id = id;
  _name = name;
  _abbr = abbr;
  _ecost = ecost;
  _damage = damage;
}

byte Weapon::getId() {
  return _id;
}

String Weapon::getName() {
  return _name;
}

char Weapon::getAbbr() {
  return _abbr;
}

int Weapon::getEcost() {
  return _ecost;
}

int Weapon::getDamage() {
  return _damage;
}



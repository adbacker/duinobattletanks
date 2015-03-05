#ifndef Statuslcd_h
#define Statuslcd_h

#include <LiquidCrystal_I2C.h>
#include "Arduino.h"
#include "Tank.h"

class Statuslcd
{
  public:
    Statuslcd();
    void update(Tank _tank, LiquidCrystal_I2C &lcd);
  private:
    unsigned long _lastTime;
    String buildWeaponLine(byte activeGun, int currentEnergy);
    String buildShieldLine(int f, int b, int l, int r);
};


#endif

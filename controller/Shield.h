#ifndef Shield_h
#define Shield_h

#include "Arduino.h"

class Shield
{
  public:
    Shield();
    Shield(int maxs, int rechargeRate);
    //records hit against shields.  if shield taken
    //to below 0, return TRUE
    boolean recordHit(int side, int damage);
    int getF();
    int getB();
    int getR();
    int getL();
    int getRechargeRate();
    void regen(); //regen 1 shield point for every rechargeRate
  private:
    int _l;
    int _r;
    int _f;
    int _b;
    int _max;
    int _rechargeRate;
    unsigned long _lastShieldChargeTime;
};


#endif

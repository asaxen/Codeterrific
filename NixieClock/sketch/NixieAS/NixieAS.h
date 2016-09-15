

/*
  NixieAS.h - Library for driving Nixie Clock.
  Based on K155ID1 drivers and IN-12A displays
  
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef NixieAS_h
#define NixieAS_h

#include "Arduino.h"
#define NUMBEROFPINS 11

class NixieAS
{
  public:
    NixieAS(int* pinLayout); 
    void showDigits(int* digits,int ms_d);
  private:
	void setDigit(int num, int chan);
    int * _pinLayout;
	int * _digits;
};

#endif
#ifndef LEDTimedAction_H
#define LEDTimedAction_H

#include "Arduino.h"
#include "CDEFO.h"

#define NO_PREDELAY 0


class LEDTimedAction {
  
  public:
    LEDTimedAction(unsigned long interval,void (*function)(Adafruit_NeoPixel*, LED*), Adafruit_NeoPixel* strip, LED* led);
    LEDTimedAction(unsigned long prev,unsigned long interval,void (*function)(Adafruit_NeoPixel*, LED*), Adafruit_NeoPixel* strip, LED* led);
	
	void reset();
	void disable();
	void enable();
	void check();
	
	void setInterval( unsigned long interval );

  private: 
    bool active;
    unsigned long previous;
    unsigned long interval;
    void (*execute)(Adafruit_NeoPixel*, LED*);
	Adafruit_NeoPixel* strip_t;
	LED* led_t;
		
};

#endif
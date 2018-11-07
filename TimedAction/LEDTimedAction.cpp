#include "LEDTimedAction.h"

/*
|| <<constructor>>
*/
LEDTimedAction::LEDTimedAction(unsigned long intervl,void (*function)(Adafruit_NeoPixel*, LED*), Adafruit_NeoPixel* strip, LED* led){
    active = true;
	previous = 0;
	interval = intervl;
	execute = function;
	strip_t = strip;
	led_t = led;
}

/*
|| <<constructor>>
*/
LEDTimedAction::LEDTimedAction(unsigned long prev,unsigned long intervl,void (*function)(Adafruit_NeoPixel*, LED*), Adafruit_NeoPixel* strip, LED* led){
    active = true;
	previous = prev;
	interval = intervl;
	execute = function;
	strip_t = strip;
	led_t = led;
}

void LEDTimedAction::reset(){
    previous = millis();
}

void LEDTimedAction::disable(){
    active = false;
}

void LEDTimedAction::enable(){
	active = true;
}

void LEDTimedAction::check(){
  if ( active && (millis()-previous >= interval) ) {
    previous = millis();
    execute(strip_t, led_t);
  }
}

void LEDTimedAction::setInterval( unsigned long intervl){
	interval = intervl;
}
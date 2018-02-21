#ifndef CDEFO_h
#define CDEFO_h
#include <arduino/Arduino.h>
#include <cstdint>
#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>

#define PIN      6
#define N_LEDS 30

class cdefo
{
public:
	cdefo();
	void led(int r, int g, int b, int pattern);
	void visit_website(char url[]);
	void play_spotify(char song[]);
	void play_video(char location[]);
	void record_audio(char location[]);
	void play_audio(char location[]);
	Adafruit_NeoPixel strip;

private:
	void chase(uint32_t c); //RGB lighting patterns
};

#endif

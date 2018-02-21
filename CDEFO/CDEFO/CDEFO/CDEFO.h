#ifndef CDEFO_H
#define CDEFO_H
#include <arduino/Arduino.h>
#include <cstdint>
#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>

#define PIN      6
#define N_LEDS 30

class cdefo
{
public:
	cdefo();
	static void led(Adafruit_NeoPixel strip, int r, int g, int b, int pattern);
	static void visit_website(char url[]);
	static void play_spotify(char song[]);
	static void play_video(char location[]);
	static void record_audio(char location[]);
	static void play_audio(char location[]);

private:
	static void chase(Adafruit_NeoPixel strip, uint32_t c); //RGB lighting patterns
};

#endif

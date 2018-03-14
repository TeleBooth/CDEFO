#ifndef CDEFO_H
#define CDEFO_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <NfcAdapter.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed

#define PIN      6
#define N_LEDS 30
#define CHASE_INTERVAL 30
#define BREATHE_INTERVAL 75
#define BREATHE_SCALE 1.10

class cdefo
{
public:
	cdefo();

	//Startup methods
	static void start_lights(Adafruit_NeoPixel* strip);

	//Read methods
	static void visit_website(char* line);
	static void play_spotify(char* song);
	static void play_video(char* location);
	static void record_audio(char* location);
	static void play_audio(char* location);
	static void stop(NfcAdapter* nfc);
	static void read_ndef(NfcTag* tag, char*** loadAsString, int* depth);

	//LED methods
	static void drive_lights(Adafruit_NeoPixel* strip, char** mini, int* light_number);
	static void light_script(char* script, char*** mini, int *light_number);

	//Write methods
	static void write_records(NfcAdapter* nfc);

private:

	//LED Patterns
	static void breathe(Adafruit_NeoPixel *strip, uint32_t* c);
	static void chase(Adafruit_NeoPixel* strip, uint32_t* c); //RGB lighting patterns

	//default color that the strips will use when starting up, written as a packed 32-bit int
	static const uint32_t def_mood = ((uint32_t)5 << 16) | ((uint32_t)5 << 8) | 20;
};

#endif

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

class cdefo
{
public:
	cdefo();

	//Read methods
	static void chase(Adafruit_NeoPixel *strip, uint32_t c); //RGB lighting patterns
	static void visit_website(String line);
	static void play_spotify(String song);
	static void play_video(String location);
	static void record_audio(String location);
	static void play_audio(String location);
	static void stop(NfcAdapter *nfc);
	static void run_records(NfcAdapter *nfc);

	//Write methods
	static void write_records(NfcAdapter *nfc);
private:

};

#endif

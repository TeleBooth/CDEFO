#ifndef CDEFO_H
#define CDEFO_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <NfcAdapter.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed
#include <TimedAction.h>

#define MOOD_PIN 6
#define MUSIC_PIN 5
#define N_LEDS 30
#define INTERVAL_LED 25

class cdefo
{
public:
	cdefo(NfcTag *tag);

	//Get/set methods
	String get_current_payload();
	void reset_payload();

	//Read methods
	void read_ndef(); //reads and stores the next payload in the Ndef message

	//Script methods
	void visit_website(String line);
	void play_spotify(String song);
	static void play_video(String location);
	static void record_audio(String location);
	static void play_audio(String location);
	void stop(NfcAdapter *nfc);

	//Module methods
	void mood_script(String script);
	//drives all of the lights that are connected to the Arduino
	void drive_lights();


	//Write methods
	static void write_records(NfcAdapter *nfc);

private:
	//Mood Lighting Patterns
	void chase(uint32_t c);

	//NeoPixel Strips
	Adafruit_NeoPixel mood_strip;
	//Adafruit_NeoPixel music;

	NdefMessage message;
	// If you have more than 1 Message then it wil cycle through them
	int record_count = 0;
	int rcounter = 0;

	//used to keep track of how far into the LED script it is
	int led_counter = 0;

	//user for timing of LEDS
	long led_millis = 0;

	//TURN THESE INTO CHARACTER ARRAYS TO HELP SAVE RAM
	String current_payload = "";

	//stores the mood lighting script in here so that it can run until the tag is removed
	String mood = "";
	String mini = "";
};

#endif

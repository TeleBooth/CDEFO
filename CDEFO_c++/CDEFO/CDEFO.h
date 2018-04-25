#ifndef CDEFO_H
#define CDEFO_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <NfcAdapter.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed

#define PIN      12
#define MUSIC_PIN 6
#define N_LEDS 30
#define CHASE_INTERVAL 30
#define BREATHE_INTERVAL 75
#define BREATHE_SCALE 1.10

#define RISE_RATE     0.13    //(0 to 1) higher values mean livelier display
#define FALL_RATE 0.04 //(0 to 1) higher values mean livelier display

#define AUDIO_PIN A0  //Pin for the envelope of the sound detector
#define LED_TOTAL 60  //Change this to the number of LEDs in your strand.
#define LED_HALF  LED_TOTAL/2

//Structures
typedef struct {
	//STRUCTURE FOR EQ
	uint16_t gradient; //Used to iterate and loop through each color palette gradually

	float maxVol;    //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
	float avgBump;    //Holds the "average" volume-change to trigger a "bump."

	uint8_t volume;   //Holds the volume level read from the sound detector.
	uint8_t last;     //Holds the value of volume from the previous loop() pass.

	uint8_t palette;  //Holds the current color palette.

	bool bump;     //Used to pass if there was a "bump" in volume

	//STRUCTURE FOR MOOD
	char **mini; //used to store the lighting scripts

	int light_number; //stores the number of lighting scripts to store
	int light_pointer; //checks which script is executing
	int light2_pointer; //checks which portion of each script is executing


	uint32_t c; //stores which color the script is using
	int finish2; //stores the execution state of each individual colors in the script and whether its finished
	int finish; //stores the executiong state of each full script
	int led_pointer; //controls the location of the actual LED

	char lighting_pattern;

	double scale; //keeps track of the intensity of each pixel during breathe
} LED;

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
	//static void drive_lights(Adafruit_NeoPixel* strip, char** mini, int* light_number);
	static void drive_lights(Adafruit_NeoPixel* strip, LED* mood);

	//static void light_script(char* script, char*** mini, int *light_number);
	static void light_script(char* script, LED *led);

	//EQ method
	static void drive_eq(Adafruit_NeoPixel* strand, LED *eq);
	static void pulse(Adafruit_NeoPixel *strand, LED *eq);

	//Write methods
	static void write_records(NfcAdapter* nfc);

private:

	//EQ helpers
	static double smoothVol(uint8_t last, uint8_t volume);
	static uint8_t split(uint32_t *color, uint8_t i);
	static void fade(Adafruit_NeoPixel *strand, double damper);
	static uint32_t Rainbow(Adafruit_NeoPixel *strand, unsigned int *i);

	//LED Patterns
	//static void breathe(Adafruit_NeoPixel *strip, uint32_t* c);
	//static void chase(Adafruit_NeoPixel* strip, uint32_t* c); //RGB lighting patterns

	static void chase(Adafruit_NeoPixel* strip, LED *mood);
	static void breathe(Adafruit_NeoPixel* strip, LED *mood);

	//default color that the strips will use when starting up, written as a packed 32-bit int
	static const uint32_t def_mood = ((uint32_t)5 << 16) | ((uint32_t)5 << 8) | 20;
	static uint16_t thresholds[6];
};

#endif

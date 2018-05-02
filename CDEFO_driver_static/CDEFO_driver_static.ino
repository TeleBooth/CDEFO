#include <Adafruit_NeoPixel.h>
#include <CDEFO.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed
#include <NfcAdapter.h>
#include <LEDTimedAction.h>

#define MOOD_PIN  6
#define MUSIC_PIN  12
#define AUDIO_PIN A0  //Pin for the envelope of the sound detector

Adafruit_NeoPixel mood = Adafruit_NeoPixel(MOOD_LEDS, MOOD_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel music = Adafruit_NeoPixel(MUSIC_LEDS, MUSIC_PIN, NEO_GRB + NEO_KHZ800);
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using
//LED structure
LED led;
//LED thread
LEDTimedAction eq_driver = LEDTimedAction(20, cdefo::drive_eq, &music, &led);
LEDTimedAction mood_driver = LEDTimedAction(15, cdefo::drive_lights, &mood, &led);
//ANALOG PINS 0, 4, AND 5 ARE BEING USED CURRENTLY
//DIGITAL PINS 2, 6, AND 12 ARE BEING USED CURRENTLY


void setup() {
  Serial.begin(9600);
  nfc.begin();
  mood.begin();
  music.begin();
  
  //initialize the LED structure
  led.gradient = 0; //Used to iterate and loop through each color palette gradually
  led.maxVol = 175;    //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
  led.avgBump = 0;    //Holds the "average" volume-change to trigger a "bump."
  led.volume = 0;   //Holds the volume level read from the sound detector.
  led.last = 0;     //Holds the value of volume from the previous loop() pass.
  led.palette = 0;  //Holds the current color palette.
  led.bump = false;     //Used to pass if there was a "bump" in volume
  led.light_number = 0;
  led.light2_pointer = 1;
  led.light_pointer = 0;
  led.c = 0;
  led.finish = 0;
  led.finish2 = 1;
  led.led_pointer = 0;
  led.scale = BREATHE_SCALE;
  led.start_col = 0;

  cdefo::start_lights(&mood, &led.start_col);

  delay(100);
  
  eq_driver.disable();
  mood_driver.disable();
}

void loop()
{
  if (nfc.tagPresent(10))
  {
    Serial.print("tag is found\n");
    NfcTag tag = nfc.read();
    //2-d character array that will store the lighting scripts
    //4 possible lighting patterns = 4 wide 2-d array
    led.mini = (char **)malloc(sizeof(char *) * 4);

    //string array storing all of the payloads on the chip
    char **payloads;
    //the number of payloads in the tag
    int payload_depth;
    cdefo::read_ndef(&tag, &payloads, &payload_depth);

    for (int i = 0; i < payload_depth; i++)
    {
      //cdefo::drive_lights(&mood, &mini, &light_number);
  
      //:Stop:
      if (payloads[i][2] == 't')
      {
        //used to check whether or not the NFC tag is still present for latency sake
        int j = 0;
        unsigned long nfcTimerCurr = 0;
        unsigned long nfcTimerPrev = 0;
        eq_driver.enable();
        eq_driver.reset();
        mood_driver.enable();
        mood_driver.reset();
        Serial.print(":End:\n");
        while (!j)
        {
          nfcTimerCurr = millis();
          //cdefo::drive_lights(&mood, mini, &light_number);
          eq_driver.check();
          mood_driver.check();
          //cdefo::drive_eq(&music, &eq);
            if(led.finish2){
              if(!nfc.tagPresent(10)){
                j = 1;
              }
            }
        }
      }

      //:L:
      if (payloads[i][2] == 'L')
      {
        cdefo::light_script(payloads[i], &led);
        cdefo::start_lights(&mood, &led.start_col);
      }

      //:W: URL :WE:
      if (payloads[i][1] == 'W')
      {
        cdefo::visit_website(payloads[i]);
      }

      //:S:
      if (payloads[i][1] == 'S' && payloads[i][2] != 't')
      {
        cdefo::play_spotify(payloads[i]);
      }
      //:P:
      if (payloads[i][1] == 'P')
      {
        cdefo::play_audio(payloads[i]);
      }
      //:R:
      if (payloads[i][1] == 'R')
      {
        cdefo::record_audio(payloads[i]);
      }
    }
    //FREE THE ALLOCATED MEMORY/CLEANUP
    //free the mini lighting script
    for (int i = 0; i < led.light_number; i++)
    {
      free(led.mini[i]);
    }

    //free the payloads
    for (int i = 0; i < payload_depth; i++)
    {
      free(payloads[i]);
    }
    led.start_col = 0; //resets the resting color
    Serial.print("Cleanup finished :Stop:\n");
  }
  //eq_driver.check();
  Serial.print("place a tag.\n");
  cdefo::start_lights(&mood, &led.start_col);
}

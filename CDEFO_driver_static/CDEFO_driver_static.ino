#include <Adafruit_NeoPixel.h>
#include <CDEFO.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed
#include <NfcAdapter.h>
#include <TimedAction.h>

Adafruit_NeoPixel mood = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel music = Adafruit_NeoPixel(LED_TOTAL, MUSIC_PIN, NEO_GRB + NEO_KHZ800);
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using
//EQ structure
EQ eq;
//EQ thread
TimedAction eq_driver = TimedAction(30, cdefo::drive_eq, &music, &eq);
//ANALOG PINS 0, 4, AND 5 ARE BEING USED CURRENTLY
//DIGITAL PINS 2, 6, AND 12 ARE BEING USED CURRENTLY


void setup() {
  Serial.begin(9600);
  nfc.begin();
  mood.begin();
  music.begin();
  cdefo::start_lights(&mood);

  delay(100);
  
  //initialize the EQ structure
  eq.gradient = 0; //Used to iterate and loop through each color palette gradually
  eq.maxVol = 100;    //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
  eq.avgBump = 0;    //Holds the "average" volume-change to trigger a "bump."
  eq.volume = 0;   //Holds the volume level read from the sound detector.
  eq.last = 0;     //Holds the value of volume from the previous loop() pass.
  eq.palette = 0;  //Holds the current color palette.
  eq.bump = false;     //Used to pass if there was a "bump" in volume
  eq_driver.disable();
}

void loop()
{
  //eq_driver.check();
  if (nfc.tagPresent())
  {
    Serial.println("tag is found");
    NfcTag tag = nfc.read();
    //2-d character array that will store the lighting scripts
    //4 possible lighting patterns = 4 wide 2-d array
    char **mini = (char **)malloc(sizeof(char *) * 4);
    //stores the number of different lighting scripts
    int light_number = 0;

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
        int j = 0;
        unsigned long nfcTimerCurr = 0;
        unsigned long nfcTimerPrev = 0;
        eq_driver.enable();
        eq_driver.reset();
        while (!j)
        {
          nfcTimerCurr = millis();
          //cdefo::drive_lights(&mood, mini, &light_number);
          eq_driver.check();
          //cdefo::drive_eq(&music, &eq);
          if(nfcTimerCurr - nfcTimerPrev > 10000) {
            if(!nfc.tagPresent()){
              j = 1;
            }
            nfcTimerPrev = nfcTimerCurr;
          }
        }
      }

      //:L:
      if (payloads[i][1] == 'L')
      {
        cdefo::light_script(payloads[i], &mini, &light_number);
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
    for (int i = 0; i < light_number; i++)
    {
      free(mini[i]);
    }

    //free the payloads
    for (int i = 0; i < payload_depth; i++)
    {
      free(payloads[i]);
    }
    Serial.println("Cleanup finished");
  }
  Serial.println("place a tag.");
}

#include <Adafruit_NeoPixel.h>
#include <CDEFO.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed
#include <NfcAdapter.h>

Adafruit_NeoPixel mood = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using

//ANALOG PINS 4 AND 5 ARE BEING USED CURRENTLY
//DIGITAL PINS 2 AND 6 ARE BEING USED CURRENTLY

void setup() {
  Serial.begin(9600);
  nfc.begin();
  mood.begin();
  cdefo::start_lights(&mood);
}

void loop()
{
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
        boolean message = true;
        while (nfc.tagPresent())
        {
          cdefo::drive_lights(&mood, mini, &light_number);
          if (message)
          {
            Serial.println("Experience has finished executing");
            message = false;
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
  delay(1000);
  Serial.println("place a tag.");
}

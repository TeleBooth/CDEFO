#include <Adafruit_NeoPixel.h>
#include <CDEFO.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>   // The following files are included in the libraries Installed
#include <NfcAdapter.h>
#include <TimedAction.h> // for multithreading

//ANALOG PINS 4 AND 5 ARE BEING USED CURRENTLY
//DIGITAL PINS 2 AND 6 ARE BEING USED CURRENTLYS
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using

void setup() {
  Serial.begin(9600);
  nfc.begin();

}

void loop() {
  if (nfc.tagPresent())
  {
    Serial.println("tag is found");
    NfcTag tag = nfc.read();
    cdefo driver = cdefo(&tag);
    Serial.println("cdefo is created");
    while (nfc.tagPresent())
    {
      Serial.println("Tag still present");
      Serial.println(driver.get_current_payload());
      if (driver.get_current_payload().equals(""))
      {
        Serial.println("\"\" equals" + driver.get_current_payload());
        Serial.println("got the ndef");
        driver.read_ndef(); //read the next message
      }
      if (driver.get_current_payload().indexOf(":Stop:") != -1)
      {
        Serial.println("Blocking");
        driver.stop(&nfc);
        Serial.println("Stopped Blocking");
      }
      
      if (driver.get_current_payload().indexOf(":L;") != -1)
      {
        Serial.println("Running lights");
        driver.mood_script(driver.get_current_payload());
        driver.drive_lights();
        driver.reset_payload();
      }
      
      if (driver.get_current_payload().indexOf(":W:") != -1)
      {
        Serial.println("Visiting Website");
        driver.visit_website(driver.get_current_payload());
        driver.reset_payload();
      }

      if (driver.get_current_payload().indexOf(":S:") != -1)
      {
        Serial.println("Playing Spotify");
        driver.play_spotify(driver.get_current_payload());
        driver.reset_payload();
      }
    }
    delay(200);
    Serial.println("Tag still present after blocking4");
  }
  Serial.println("place a tag.");
}


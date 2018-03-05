    #include <Adafruit_NeoPixel.h>
    #include <CDEFO.h>
    #include <Wire.h>
    #include <PN532_I2C.h>
    #include <PN532.h>   // The following files are included in the libraries Installed
    #include <NfcAdapter.h>
     
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
    PN532_I2C pn532_i2c(Wire);
    NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using
     
    void setup() {
      Serial.begin(9600);
      nfc.begin();
      strip.begin();
    }
     
    void loop() {
      if (nfc.tagPresent())
        {
          Serial.println("tag is found");
          NfcTag tag = nfc.read();
          if (tag.hasNdefMessage()) // If your tag has a message
            {
              NdefMessage message = tag.getNdefMessage();
               
              // If you have more than 1 Message then it wil cycle through them
              int recordCount = message.getRecordCount();
              for (int i = 0; i < recordCount; i++)
                {
                  NdefRecord record = message.getRecord(i);

                  int payloadLength = record.getPayloadLength();
                  byte payload[payloadLength];
                  record.getPayload(payload);

                  String payloadAsString = ""; // Processes the message as a string vs as a HEX value
                  for (int c = 3; c < payloadLength; c++) 
                    {
                      payloadAsString += (char)payload[c];
                      //Serial.print((char)payloadAsString[c]);
                    }
                  if (payloadAsString.indexOf(":Stop:") != -1)
                    {
                      cdefo::stop(&nfc);
                    }
                  if (payloadAsString.indexOf(":L:") != -1)
                    {
                      cdefo::chase(&strip, strip.Color(255, 255, 255));
                      cdefo::chase(&strip, strip.Color(0, 255, 0));
                      cdefo::chase(&strip, strip.Color(0, 0, 255));
                    }
                  if (payloadAsString.indexOf(":W:") != -1)
                    {
                      cdefo::visit_website(payloadAsString);
                    }
                  if (payloadAsString.indexOf(":S:") != -1)
                    {
                      cdefo::play_spotify(payloadAsString);
                    }
                }
          }
      }
      delay(3000);
      Serial.println("place a tag.");
  }

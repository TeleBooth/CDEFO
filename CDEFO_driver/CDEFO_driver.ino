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
      Serial.println("NFC TAG READER"); // Header used when using the serial monitor
      nfc.begin();
      strip.begin();
    }
     
    void loop() {
      if (nfc.tagPresent())
        {
          NfcTag tag = nfc.read();
          if (tag.hasNdefMessage()) // If your tag has a message
            {
              NdefMessage message = tag.getNdefMessage();
               
              // If you have more than 1 Message then it wil cycle through them
              int recordCount = message.getRecordCount();
              for (int i = 0; i < recordCount; i++)
                {
                  Serial.print("\nNDEF Record: ");
                  NdefRecord record = message.getRecord(i);

                  int payloadLength = record.getPayloadLength();
                  byte payload[payloadLength];
                  record.getPayload(payload);

                  String payloadAsString = ""; // Processes the message as a string vs as a HEX value
                  for (int c = 0; c < payloadLength; c++) 
                    {
                      payloadAsString += (char)payload[c];
                      //Serial.print((char)payloadAsString[c]);
                    }
                  Serial.println(payloadAsString);
                  if (payloadAsString.indexOf("CDEFO") != -1)
                    {
                      cdefo::chase(&strip, strip.Color(255, 255, 255));
                      cdefo::chase(&strip, strip.Color(0, 255, 0));
                      cdefo::chase(&strip, strip.Color(0, 0, 255));
                      Serial.println("\nfound lights");
                    }
                  if (payloadAsString.indexOf("CDEFO") != -1)
                    {
                      char url[payloadLength];
                      payloadAsString.substring(payloadAsString.indexOf("CDEFO")).toCharArray(url, payloadLength);
                      cdefo::visit_website(url);
                      Serial.println("\nfound website");
                    }
                
                  String uid = record.getId();
                  if (uid != "")
                    {
                      Serial.print("  ID: ");Serial.println(uid); // Prints the Unique Identification of the NFC Tag
                    }
                }
          }
      }
      delay(5000);
  }

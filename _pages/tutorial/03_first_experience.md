---
layout: page
title: Writing Your First Experience
category: tutorial
---

Loading up the `write_static.ino` into the Arduino IDE, we see a fairly short and sweet Arduino script.

{% highlight c++ %}
#include <CDEFO.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);  // Indicates the Shield you are using

                  
void setup() {
    Serial.begin(9600);
    Serial.println("NFC Tag Writer"); // Serial Monitor Message
    nfc.begin();
}

void loop() {
    Serial.println("\nPlace an NFC Tag that you want to Record these Messages on!"); // Command for the Serial Monitor
    if (nfc.tagPresent()) {
        NdefMessage message = NdefMessage();
        message.addTextRecord(":S:Rope Foo");
        message.addTextRecord("G:L:cRGBRGB;bBGY;yRGB;tRGB;");
        message.addTextRecord(":P:Foo");
        message.addTextRecord(":Stop:");
        boolean success = nfc.write(message);
        if (success) {
            Serial.println("Good Job, the experience is now ready!"); // if it works you will see this message 
        } else {
            Serial.println("Write failed"); // If the the rewrite failed you will see this message
        }
    }
    delay(5000);
}
{% endhighlight %}

What happens is that the sketch creates a `PN532_I2C` object, which is the type of NFC reader chipset that I was using, 
and then we have an `NfcAdapter` object, which the PN532 is passed so that it can be used to write messages to our NFC tags.

It does this after checking `if(nfc.tagPresent())`, and if the tag is present, it will go and write five `TextRecords` in a row,
with the last Record being a `":Stop:"`. It's worth emphasizing here that **all user experiences must end with this `":Stop:"` tag.**
Otherwise they will not work properly and you will be sad and confused.

Now, the first command to note is the `":S:Rope Foo"`. When the driver detects this, it calls the library function `cdefo::play_music()` which prints that same string over Serial.
Ideally, on the other end of the Serial Port, there's the CDEFO Python script running, and it will filter that Serial input process it such that it will play the song `Rope` by the `Foo Fighters` in a Spotify webplayer.
This is outside o0f the scope of this particular tutorial, but it's worth keeping in mind that this whole platform extends passed the Arduino.

The second command `"G:L:cR;bG;yB;tRGB;"` which is just a really long "Mood Lighting" script. 
The full syntax with more video examples for this will be covered in the documentation, so
right now I'm just going to go through this and guide you through writing your own lighting script.

The first letter `G`, signifies the default color of the Mood Lighting, the color that it uses when it "starts up". Which is that little delayed fade-in propagation type pattern.

The `:L:` signifies to the Arduino that it is running the "Mood Lighting" commands, this is technically the actuall command call, since all of the commands in CDEFO are formatted like `:{id}:`

After the `:L:`, the lower case letter represent patterns, and the upper case letters represent colors.
Each semicolon block represents a "mini script" and each script will be run in the exact sequence they are written.

Breaking down this actual command, we have `cR`, which calls the `chase` pattern with the color `Red`, 
`bG` calls the `breathe` pattern with the color `Green`, `yB` calls the `cylon` pattern with the color `Blue`, 
and `tRGB` calls the `twinkle` pattern with the colors `Red`, `Green`, and `Blue`.

Running these commands all together with the included driver sketch looks something like this. 
Of course, this only works if you have setup an LED strip, which it is assumed you have if you've made it this far,
and have read the Adafruit wiring guide that I linked in the `Getting Started` page.
{% include youtubePlayer.html id="EoOCdzWXQj4" %}

The last command `:P:Foo`, like `:S:Rope Foo`, is used to control the Python Script primarily, 
and it includes special bindings I used for demonstrations that aren't worth discussing here.
We can discuss them in the next part of the tutorial, the Python Script!


#include "CDEFO.h"


cdefo::cdefo() = default;

void cdefo::visit_website(String line)
{
	Serial.print(line);
}

void cdefo::play_spotify(String song)
{
	Serial.print(song);
}

void cdefo::play_video(String location)
{
}

void cdefo::record_audio(String location)
{
}

void cdefo::play_audio(String location)
{
}

void cdefo::chase(Adafruit_NeoPixel *strip, uint32_t c)
{
	Serial.print(":L:");
	for (uint16_t i = 0; i < strip->numPixels() + 4; i++)
	{
		strip->setPixelColor(i, c); // Draw new pixel
		strip->setPixelColor(i - 4, 0); // Erase pixel a few steps back
		strip->show();
		delay(25);
	}
}

void cdefo::stop(NfcAdapter *nfc)
{
	boolean message = true;
	while (nfc->tagPresent())
	{
		if (message) {
			Serial.println("Experience has finished executing, place the next tag :Stop:");
			message = false;
		}
	}
}

void cdefo::run_records(NfcAdapter *nfc)
{
	
}

void cdefo::write_records(NfcAdapter *nfc)
{
	
}

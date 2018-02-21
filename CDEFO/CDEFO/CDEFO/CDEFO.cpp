#include "CDEFO.h"


cdefo::cdefo() = default;

void cdefo::led(Adafruit_NeoPixel strip, int r, int g, int b, int pattern)
{
	chase(strip, Adafruit_NeoPixel::Color(r, 0, 0)); // Red
	chase(strip, Adafruit_NeoPixel::Color(0, g, 0)); // Green
	chase(strip, Adafruit_NeoPixel::Color(0, 0, b)); // Blue
}

void cdefo::visit_website(char url[])
{

}

void cdefo::play_spotify(char song[])
{
	return;
}

void cdefo::play_video(char location[])
{
}

void cdefo::record_audio(char location[])
{
}

void cdefo::play_audio(char location[])
{
}

void cdefo::chase(Adafruit_NeoPixel strip, uint32_t c)
{
	for (uint16_t i = 0; i < strip.numPixels() + 4; i++)
	{
		strip.setPixelColor(i, c); // Draw new pixel
		strip.setPixelColor(i - 4, 0); // Erase pixel a few steps back
		strip.show();
		delay(25);
	}
}

#include "CDEFO.h"


cdefo::cdefo() = default;

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

void cdefo::chase(Adafruit_NeoPixel *strip, uint32_t c)
{
	for (uint16_t i = 0; i < strip->numPixels() + 4; i++)
	{
		strip->setPixelColor(i, c); // Draw new pixel
		strip->setPixelColor(i - 4, 0); // Erase pixel a few steps back
		strip->show();
		delay(25);
	}
}

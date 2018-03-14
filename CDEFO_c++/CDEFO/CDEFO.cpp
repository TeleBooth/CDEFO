#include "CDEFO.h"


cdefo::cdefo() = default;

void cdefo::start_lights(Adafruit_NeoPixel* strip)
{
	for (int i = 0; i <= strip->numPixels(); i++)
	{
		strip->setPixelColor(i, def_mood); // Draw new pixel
		strip->show();
		i++;
	}
}

void cdefo::read_ndef(NfcTag* tag, char*** payloads, int* depth)
{
	if (tag->hasNdefMessage())
	{
		NdefMessage message = tag->getNdefMessage();
		// If you have more than 1 Message then it wil cycle through them
		int recordCount = message.getRecordCount();
		*depth = recordCount;
		*payloads = (char **)malloc(sizeof(char *) * recordCount);

		for (int i = 0; i < recordCount; i++)
		{
			NdefRecord record = message.getRecord(i);

			int payloadLength = record.getPayloadLength();
			byte payload[payloadLength];
			record.getPayload(payload);

			(*payloads)[i] = (char *)malloc(sizeof(char) * payloadLength - 2);
			for (int c = 3; c < payloadLength; c++)
			{
				(*payloads)[i][c - 3] = (char)payload[c];
			}
			(*payloads)[i][payloadLength - 3] = '\0';
		}
	}
}

void cdefo::visit_website(char* line)
{
	Serial.println(line);
}

void cdefo::play_spotify(char* song)
{
	Serial.println(song);
}

void cdefo::play_video(char* location)
{
}

void cdefo::record_audio(char* location)
{
}

void cdefo::play_audio(char* location)
{
}

void cdefo::light_script(char* script, char*** mini, int* light_number)
{
	//translate the script into actual lighting
	//starts at index = 3 because of the ":L:" at the beginning
	script += 3;
	//used for indexing after the 
	int i = 0;
	while (*script != '\0')
	{
		//looks for the dividing character to split up the scripts
		if (*script == ';')
		{
			//dynamically allocates mini to be the right size to hold the script
			//+ 1 for the NULL character
			(*mini)[*light_number] = (char *)malloc(sizeof(char) * (i) + 1);
			//copy the proper mini string over to the 2d array
			strncpy((*mini)[*light_number], (script - i), i);
			(*mini)[*light_number][i] = '\0';

			(*light_number)++;
			//reset i so there is no overlap
			i = -1;

		}
		//increment the index
		i++;
		//move the pointer along
		script++;
	}
}

void cdefo::drive_lights(Adafruit_NeoPixel* strip, char** mini, int* light_number)
{
	//process the mini script
	for (int i = 0; i < *light_number; i++)
	{
		int j = 1;
		while (mini[i][j] != '\0')
		{
			//POTENTIALLY ADD MULTITHREAD METHODS HERE
			uint32_t c = 0;
			//Sets the color to be passed in
			if (mini[i][j] == 'R')
				//Red
				c = Adafruit_NeoPixel::Color(255, 0, 0);
			if (mini[i][j] == 'G')
				//Green
				c = Adafruit_NeoPixel::Color(0, 255, 0);
			if (mini[i][j] == 'B')
				//Blue
				c = Adafruit_NeoPixel::Color(0, 0, 255);
			if (mini[i][j] == 'Y')
				//Yellow
				c = Adafruit_NeoPixel::Color(255, 255, 0);
			if (mini[i][j] == 'T')
				//Turquoise
				c = Adafruit_NeoPixel::Color(0, 255, 255);
			if (mini[i][j] == 'P')
				//Purple
				c = Adafruit_NeoPixel::Color(255, 0, 255);
			if (mini[i][j] == 'W')
				//White
				c = Adafruit_NeoPixel::Color(255, 255, 255);

			if (mini[i][0] == 'c')
			{
				chase(strip, &c);
			}

			if (mini[i][0] == 'b')
			{
				delay(500);
				breathe(strip, &c);
			}

			j++;
		}
	}
}


//The lights chase each other down both ends of the strips
void cdefo::chase(Adafruit_NeoPixel* strip, uint32_t* c)
{
	int i = 0;
	unsigned long previousMillis = 0;
	unsigned long currentMillis = 0;
	//chase out
	while (i < (N_LEDS / 2) + 4)
	{
		currentMillis = millis();

		if (currentMillis - previousMillis > CHASE_INTERVAL)
		{
			previousMillis = currentMillis;
			strip->setPixelColor((N_LEDS / 2) + i, *c); // Draw new pixel
			strip->setPixelColor((N_LEDS / 2) - i, *c); // Erase pixel a few steps back
			if (i >= 4)
			{
				strip->setPixelColor((N_LEDS / 2) + i - 4, def_mood);
				strip->setPixelColor((N_LEDS / 2) - i + 4, def_mood);
			}
			strip->show();
			i++;
		}
	}
	//chase in
	i = 0;
	while (i < (N_LEDS / 2) + 4 + 1)
	{
		currentMillis = millis();

		if (currentMillis - previousMillis > CHASE_INTERVAL)
		{
			previousMillis = currentMillis;
			if (i <= (N_LEDS / 2))
			{
				strip->setPixelColor(i, *c); // Draw new pixel
				strip->setPixelColor(N_LEDS - i, *c); // Erase pixel a few steps back
			}

			if (i >= 4)
			{
				strip->setPixelColor(i - 4, def_mood);
				strip->setPixelColor(N_LEDS - i + 4, def_mood);
			}
			
			strip->show();
			i++;
		}
	}
}

//The lights breathe, but it fades towards the edges
void cdefo::breathe(Adafruit_NeoPixel* strip, uint32_t* c)
{
	double scale = BREATHE_SCALE;
	int i = 0;
	unsigned long previousMillis = 0;
	unsigned long currentMillis = 0;

	//breathe in
	while (i <= (N_LEDS / 2) + 1)
	{
		currentMillis = millis();
		
		if (currentMillis - previousMillis > BREATHE_INTERVAL)
		{
			previousMillis = currentMillis;
			currentMillis = 0;
			
			//the pixels in between get lit
			for(int j = i; j <= N_LEDS -i; j++)
			{
				strip->setPixelColor(j, Adafruit_NeoPixel::Color((int)5 * scale, (int)5 * scale, (int)20 * scale));
			}

			strip->show();
			i++;
			scale *= BREATHE_SCALE;
		}
	}
	
	//breathe out
	scale /= BREATHE_SCALE;
	i = (N_LEDS / 2) + 1;
	while (i >= 0)
	{
		currentMillis = millis();

		if (currentMillis - previousMillis > BREATHE_INTERVAL)
		{
			previousMillis = currentMillis;
			currentMillis = 0;

			//the pixels in between get lit
			for (int j = i; j <= N_LEDS - i; j++)
			{
				strip->setPixelColor(j, Adafruit_NeoPixel::Color((int)5 * scale, (int)5 *scale, (int)20 * scale));
			}

			strip->show();
			i--;
			scale /= BREATHE_SCALE;
		}
	}
}


void cdefo::stop(NfcAdapter* nfc)
{
	boolean message = true;
	while (nfc->tagPresent())
	{
		if (message)
		{
			Serial.println("Experience has finished executing, place the next tag :Stop:");
			message = false;
		}
		Serial.println("Still blocking...");
	}
}

void cdefo::write_records(NfcAdapter* nfc)
{
}

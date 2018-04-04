#include "CDEFO.h"


cdefo::cdefo() = default;

uint16_t cdefo::thresholds[] = {1529, 1019, 764, 764, 764, 1274 };

void cdefo::start_lights(Adafruit_NeoPixel* strip)
{
	for (int i = 0; i <= strip->numPixels(); i++)
	{
		strip->setPixelColor(i, def_mood); // Draw new pixel
		strip->show();
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
	Serial.println(location);
}

void cdefo::play_audio(char* location)
{
	Serial.println(location);
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

void cdefo::drive_eq(Adafruit_NeoPixel * strand, EQ *eq)
{
	eq->volume = analogRead(AUDIO_PIN);       //Record the volume level from the sound detector

	//This is where "gradient" is modulated to prevent overflow.
	if (eq->gradient > thresholds[eq->palette]) {
		eq->gradient %= thresholds[eq->palette] + 1;

		//Everytime a palette gets completed is a good time to readjust "maxVol," just in case
		//  the song gets quieter; we also don't want to lose brightness intensity permanently
		//  because of one stray loud sound.
	}

	//If there is a decent change in volume since the last pass, average it into "avgBump"
	if (eq->volume - eq->last > 10)
		eq->avgBump = (eq->avgBump + (eq->volume - eq->last)) / 2.0;

	//If there is a notable change in volume, trigger a "bump"
	//  avgbump is lowered just a little for comparing to make the visual slightly more sensitive to a beat.
	eq->bump = (eq->volume - eq->last > eq->avgBump * .9);

	pulse(strand, eq);   //Calls the appropriate visualization to be displayed with the globals as they are.

	(eq->gradient)++;    //Increments gradient

	eq->last = eq->volume; //Records current volume for next pass

}

void cdefo::pulse(Adafruit_NeoPixel *strand, EQ *eq)
{
		fade(strand, 0.85);   //Listed below, this function simply dims the colors a little bit each pass of loop()

					  //Advances the palette to the next noticeable color if there is a "bump"
		if (eq->bump)
			eq->gradient += thresholds[eq->palette] / 24;

		//If it's silent, we want the fade effect to take over, hence this if-statement
		if (eq->volume > 0) {
			uint32_t col = Rainbow(strand, &(eq->gradient)); //Our retrieved 32-bit color

											
			double newVol = smoothVol(eq->last, eq->volume);
			//These variables determine where to start and end the pulse since it starts from the middle of the strand.
			//  The quantities are stored in variables so they only have to be computed once (plus we use them in the loop).
			int start = LED_HALF - (LED_HALF * (newVol / eq->maxVol));
			int finish = LED_HALF + (LED_HALF * (newVol / eq->maxVol)) + strand->numPixels() % 2;
			//Listed above, LED_HALF is simply half the number of LEDs on your strand. ↑ this part adjusts for an odd quantity.

			for (int i = start; i < finish; i++) {
				//      for (int i = 0; i < strip.numPixels() / 2, i++) {

				//"damp" creates the fade effect of being dimmer the farther the pixel is from the center of the strand.
				//  It returns a value between 0 and 1 that peaks at 1 at the center of the strand and 0 at the ends.
				float damp = sin((i - start) * PI / float(finish - start));

				//Squaring damp creates more distinctive brightness.
				damp = pow(damp, 2.0);

				//This is the only difference from Pulse(). The color for each pixel isn't the same, but rather the
				//  entire gradient fitted to the spread of the pulse, with some shifting from "gradient".
				int val = thresholds[eq->palette] * (i - start) / (finish - start);
				val += eq->gradient;

				uint32_t col2 = strand->getPixelColor(i);
				uint8_t colors[3];
				float avgCol = 0, avgCol2 = 0;
				for (int k = 0; k < 3; k++) {
					colors[k] = split(&col, k) * damp * pow(eq->volume / eq->maxVol, 2);
					avgCol += colors[k];
					avgCol2 += split(&col2, k);
				}
				avgCol /= 3.0, avgCol2 /= 3.0;
				if (avgCol > avgCol2) strand->setPixelColor(i, strand->Color(colors[0], colors[1], colors[2]));
			}
		}
		strand->show();
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

double cdefo::smoothVol(uint8_t last, uint8_t volume)
{
	double oldVol = last;
	double newVol = volume;
	if (oldVol < newVol)
	{
		newVol = (newVol * RISE_RATE) + (oldVol * (1 - RISE_RATE));
		// limit how quickly volume can rise from the last value
	}

	else
	{
		newVol = (newVol * FALL_RATE) + (oldVol * (1 - FALL_RATE));
		// limit how quickly volume can fall from the last value
	}

	return newVol;
}

uint8_t cdefo::split(uint32_t * color, uint8_t i)
{

	//0 = Red, 1 = Green, 2 = Blue

	if (i == 0) return *color >> 16;
	if (i == 1) return *color >> 8;
	if (i == 2) return *color >> 0;
	return -1;
}

void cdefo::fade(Adafruit_NeoPixel *strand, double damper)
{
	//"damper" must be between 0 and 1, or else you'll end up brightening the lights or doing nothing.

	for (int i = 0; i < strand->numPixels(); i++) {

		//Retrieve the color at the current position.
		uint32_t col = strand->getPixelColor(i);

		//If it's black, you can't fade that any further.
		if (col == 0) continue;

		float colors[3]; //Array of the three RGB values

						 //Multiply each value by "damper"
		for (int j = 0; j < 3; j++) colors[j] = split(&col, j) * damper;

		//Set the dampened colors back to their spot.
		strand->setPixelColor(i, strand->Color(colors[0], colors[1], colors[2]));
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

uint32_t cdefo::Rainbow(Adafruit_NeoPixel *strand, unsigned int *i) {
	if (*i > 1529) {
		unsigned int datain = *i % 1530;
		return Rainbow(strand, &datain);
	}
	if (*i > 1274) return strand->Color(255, 0, 255 - (*i % 255));   //violet -> red
	if (*i > 1019) return strand->Color((*i % 255), 0, 255);         //blue -> violet
	if (*i > 764) return strand->Color(0, 255 - (*i % 255), 255);    //aqua -> blue
	if (*i > 509) return strand->Color(0, 255, (*i % 255));          //green -> aqua
	if (*i > 255) return strand->Color(255 - (*i % 255), 255, 0);    //yellow -> green
	return strand->Color(255, *i, 0);                               //red -> yellow
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

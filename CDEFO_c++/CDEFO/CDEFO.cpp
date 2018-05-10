#include "CDEFO.h"


cdefo::cdefo() = default;

uint16_t cdefo::thresholds[] = {1529, 1019, 764, 764, 764, 1274};

void cdefo::start_lights(Adafruit_NeoPixel* strip, uint32_t* start_col)
{
	uint32_t mood;
	if (*start_col == 0)
		mood = def_mood;
	else
		mood = *start_col;


	//split color for fade-in
	uint8_t colors_init[3];
	for (int k = 0; k < 3; k++)
	{
		colors_init[k] = split(mood, k); //scales down the initial lighting
	}

	//scale for fade


	for (int i = 0; i < 6; i++)
	{
		double scale = FADEIN_SCALE;
		for (int k = 0; k <= MOOD_LEDS / 6; k++)
		{
			for (int j = 0; j <= MOOD_LEDS / 12; j++)
			{
				uint32_t fader = Adafruit_NeoPixel::Color(colors_init[0] * scale, colors_init[1] * scale, colors_init[2] * scale);
				strip->setPixelColor((MOOD_LEDS / 2) - 2 - ((i * MOOD_LEDS / 12) + j), fader); // Draw new pixel
				strip->setPixelColor((MOOD_LEDS / 2) - 1 + ((i * MOOD_LEDS / 12) + j), fader); // Draw new pixel
				strip->show();
			}
			scale *= FADEIN_SCALE;
			delay(5);
		}
		delay(300);
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
	Serial.print(line);
	Serial.print("\n");
}

void cdefo::play_spotify(char* song)
{
	Serial.print(song);
	Serial.print("\n");
}

void cdefo::play_video(char* location)
{
	Serial.print(location);
	Serial.print("\n");
}

void cdefo::display_image(char* location)
{
	Serial.print(location);
	Serial.print("\n");
}

void cdefo::record_audio(char* location)
{
	Serial.print(location);
	Serial.print("\n");
}

void cdefo::play_audio(char* location)
{
	Serial.print(location);
	Serial.print("\n");
}

void cdefo::light_script(char* script, LED* led)
{
	//translate the script into actual lighting
	//starts at index = 4 because of the ":L:" and the start_col at the beginning
	char c = script[0];
	//Serial.println(c);
	if (c == 'R')
		//Red
		led->start_col = Adafruit_NeoPixel::Color(20, 0, 0);
	if (c == 'G')
		//Green
		led->start_col = Adafruit_NeoPixel::Color(0, 20, 0);
	if (c == 'B')
		//Blue
		led->start_col = Adafruit_NeoPixel::Color(0, 0, 20);
	if (c == 'Y')
		//Yellow
		led->start_col = Adafruit_NeoPixel::Color(20, 20, 0);
	if (c == 'T')
		//Turquoise
		led->start_col = Adafruit_NeoPixel::Color(0, 20, 20);
	if (c == 'P')
		//Purple
		led->start_col = Adafruit_NeoPixel::Color(20, 0, 20);
	if (c == 'W')
		//White
		led->start_col = Adafruit_NeoPixel::Color(20, 20, 20);

	script += 4;
	//used for indexing after the 
	int i = 0;
	while (*script != '\0')
	{
		//looks for the dividing character to split up the scripts
		if (*script == ';')
		{
			//dynamically allocates mini to be the right size to hold the script
			//+ 1 for the NULL character
			(led->mini)[led->light_number] = (char *)malloc(sizeof(char) * (i) + 1);
			//copy the proper mini string over to the 2d array
			strncpy((led->mini)[led->light_number], (script - i), i);
			(led->mini)[led->light_number][i] = '\0';

			Serial.println((led->mini)[led->light_number]);
			led->light_number++;
			//reset i so there is no overlap
			i = -1;
		}
		//increment the index
		i++;
		//move the pointer along
		script++;
	}
}

void cdefo::drive_eq(Adafruit_NeoPixel* strand, LED* eq)
{
	eq->volume = analogRead(AUDIO_PIN); //Record the volume level from the sound detector

	//This is where "gradient" is modulated to prevent overflow.
	if (eq->gradient > thresholds[eq->palette])
	{
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

	pulse(strand, eq); //Calls the appropriate visualization to be displayed with the globals as they are.

	(eq->gradient)++; //Increments gradient

	eq->last = eq->volume; //Records current volume for next pass
}

void cdefo::pulse(Adafruit_NeoPixel* strand, LED* eq)
{
	fade(strand, PULSE_FADE); //Listed below, this function simply dims the colors a little bit each pass of loop()

	//Advances the palette to the next noticeable color if there is a "bump"
	if (eq->bump)
		eq->gradient += thresholds[eq->palette] / 24;

	uint32_t init_col2 = strand->getPixelColor(LED_HALF + 2);
	uint32_t col = Rainbow(strand, &(eq->gradient)); //Our retrieved 32-bit color
	uint8_t colors_init[3];
	float init_Col = 0, init_Col2 = 0;
	for (int k = 0; k < 3; k++)
	{
		colors_init[k] = split(col, k) * .25; //scales down the initial lighting
		init_Col += colors_init[k];
		init_Col2 += split(init_col2, k);
	}
	init_Col /= 3.0, init_Col2 /= 3.0;
	if (init_Col > init_Col2)
	{
		strand->setPixelColor(0, strand->Color(colors_init[0], colors_init[1], colors_init[2]));
		strand->setPixelColor(MUSIC_LEDS - 1, strand->Color(colors_init[0], colors_init[1], colors_init[2]));
		strand->setPixelColor(1, strand->Color(colors_init[0] * .4, colors_init[1] * .4, colors_init[2] * .4));
		strand->setPixelColor(MUSIC_LEDS - 2, strand->Color(colors_init[0] * .4, colors_init[1] * .4, colors_init[2] * .4));
		strand->setPixelColor(2, strand->Color(colors_init[0] * .3, colors_init[1] * .3, colors_init[2] * .3));
		strand->setPixelColor(MUSIC_LEDS - 3, strand->Color(colors_init[0] * .3, colors_init[1] * .3, colors_init[2] * .3));
	}


	//If it's silent, we want the fade effect to take over, hence this if-statement
	if (eq->volume > 0)
	{
		double newVol = smoothVol(eq->last, eq->volume);
		//These variables determine where to start and end the pulse since it starts from the middle of the strand.
		//  The quantities are stored in variables so they only have to be computed once (plus we use them in the loop).
		int start = LED_HALF - (LED_HALF * (newVol / eq->maxVol));
		int finish = LED_HALF + (LED_HALF * (newVol / eq->maxVol)) + strand->numPixels() % 2;
		//Listed above, LED_HALF is simply half the number of LEDs on your strand. ↑ this part adjusts for an odd quantity.

		for (int i = start; i < finish; i++)
		{
			//      for (int i = 0; i < strip.numPixels() / 2, i++) {

			//"damp" creates the fade effect of being dimmer the farther the pixel is from the center of the strand.
			//  It returns a value between 0 and 1 that peaks at 1 at the center of the strand and 0 at the ends.
			float damp = sin((i - start) * PI / float(finish - start));

			//Squaring damp creates more distinctive brightness.
			damp = pow(damp, 2.0);

			uint32_t col2 = strand->getPixelColor(i);
			uint8_t colors[3];
			float avgCol = 0, avgCol2 = 0;
			for (int k = 0; k < 3; k++)
			{
				colors[k] = split(col, k) * damp * pow(eq->volume / eq->maxVol, 2);
				avgCol += colors[k];
				avgCol2 += split(col2, k);
			}
			avgCol /= 3.0, avgCol2 /= 3.0;
			if (avgCol > avgCol2) strand->setPixelColor(((i - LED_HALF) % MUSIC_LEDS),
			                                            strand->Color(colors[0], colors[1], colors[2]));
		}
	}
	strand->show();
}

void cdefo::palette_pulse(Adafruit_NeoPixel* strand, LED* eq)
{
	fade(strand, PULSE_FADE); //Listed below, this function simply dims the colors a little bit each pass of loop()

	//Advances the palette to the next noticeable color if there is a "bump"
	if (eq->bump)
		eq->gradient += thresholds[eq->palette] / 24;

	uint32_t col = Rainbow(strand, &(eq->gradient)); //Our retrieved 32-bit color
	uint8_t colors_init[3];
	for (int k = 0; k < 3; k++)
	{
		colors_init[k] = split(col, k) * .5; //scales down the initial lighting
	}
	strand->setPixelColor(LED_HALF, strand->Color(colors_init[0], colors_init[1], colors_init[2]));
	strand->setPixelColor(LED_HALF - 1, strand->Color(colors_init[0], colors_init[1], colors_init[2]));
	strand->setPixelColor(LED_HALF + 1, strand->Color(colors_init[0] * .5, colors_init[1] * .5, colors_init[2] * .5));
	strand->setPixelColor(LED_HALF - 2, strand->Color(colors_init[0] * .5, colors_init[1] * .5, colors_init[2] * .5));
	strand->setPixelColor(LED_HALF + 2, strand->Color(colors_init[0] * .25, colors_init[1] * .25, colors_init[2] * .25));
	strand->setPixelColor(LED_HALF - 3, strand->Color(colors_init[0] * .25, colors_init[1] * .25, colors_init[2] * .25));

	//If it's silent, we want the fade effect to take over, hence this if-statement
	if (eq->volume > 0)
	{
		double newVol = smoothVol(eq->last, eq->volume);
		//These variables determine where to start and end the pulse since it starts from the middle of the strand.
		//  The quantities are stored in variables so they only have to be computed once (plus we use them in the loop).
		int start = LED_HALF - (LED_HALF * (newVol / eq->maxVol));
		int finish = LED_HALF + (LED_HALF * (newVol / eq->maxVol)) + strand->numPixels() % 2;

		//Listed above, LED_HALF is simply half the number of LEDs on your strand. ↑ this part adjusts for an odd quantity.

		for (int i = start; i < finish; i++)
		{
			//      for (int i = 0; i < strip.numPixels() / 2, i++) {

			//"damp" creates the fade effect of being dimmer the farther the pixel is from the center of the strand.
			//  It returns a value between 0 and 1 that peaks at 1 at the center of the strand and 0 at the ends.
			float damp = sin((i - start) * PI / float(finish - start));

			//Squaring damp creates more distinctive brightness.
			damp = pow(damp, 2.0);

			//This is the only difference from Pulse(). The color for each pixel isn't the same, but rather the
			//  entire gradient fitted to the spread of the pulse, with some shifting from "gradient".
			uint16_t val = thresholds[eq->palette] * (i - start) / (finish - start);
			val += eq->gradient;
			uint32_t col = (val < 0) ? Rainbow(strand, &eq->gradient) : Rainbow(strand, &val);

			uint32_t col2 = strand->getPixelColor(i);
			uint8_t colors[3];
			float avgCol = 0, avgCol2 = 0;
			for (int k = 0; k < 3; k++)
			{
				colors[k] = split(col, k) * damp * pow(eq->volume / eq->maxVol, 2);
				avgCol += colors[k];
				avgCol2 += split(col2, k);
			}
			avgCol /= 3.0, avgCol2 /= 3.0;
			if (avgCol > avgCol2) strand->setPixelColor(i, strand->Color(colors[0], colors[1], colors[2]));
		}
	}
	strand->show();
}

void cdefo::drive_lights(Adafruit_NeoPixel* strip, LED* mood)
{
	//stores the next lighting pattern after the previous one is completely finished
	if (mood->light_pointer <= mood->light_number && mood->finish)
	{
		//Serial.println(mood->light_pointer);
		//Serial.println()
		if (mood->light_pointer == mood->light_number - 1)
		{
			//Serial.println("Completed");
			mood->light2_pointer = 1;
			mood->light_pointer = 0;
			mood->finish = 0;
			mood->finish2 = 1;
		}
		else
		{
			//Serial.println("Reset");
			mood->light2_pointer = 1;
			//prepares to read the next line
			mood->light_pointer++;
			mood->finish = 0;
			mood->finish2 = 1;
		}
	}

		//actually runs the lights
	else if (!mood->finish)
	{
		//stores the next color after the previous color is finished
		if (mood->finish2)
		{
			char c = mood->mini[mood->light_pointer][mood->light2_pointer];
			//Serial.print(mood->light2_pointer);
			//Serial.print(mood->mini[2]);
			//Serial.println(mood->light2_pointer);
			//Serial.print(c);
			//if it has reached the end of the line, move to the next one
			if (c == '\0')
			{
				//Serial.println("NULL");
				//Serial.println(mood->mini[mood->light_pointer]);
				mood->finish = 1;
			}

			else if (mood->mini[mood->light_pointer][0] == 'c')
			{
				//Serial.println(c);
				//Serial.println(" chase");
				if (c == 'R')
					//Red
					mood->c = Adafruit_NeoPixel::Color(255, 0, 0);
				else if (c == 'G')
					//Green
					mood->c = Adafruit_NeoPixel::Color(0, 255, 0);
				else if (c == 'B')
					//Blue
					mood->c = Adafruit_NeoPixel::Color(0, 0, 255);
				else if (c == 'Y')
					//Yellow
					mood->c = Adafruit_NeoPixel::Color(255, 255, 0);
				else if (c == 'T')
					//Turquoise
					mood->c = Adafruit_NeoPixel::Color(0, 255, 255);
				else if (c == 'P')
					//Purple
					mood->c = Adafruit_NeoPixel::Color(255, 0, 255);
				else
					//White
					mood->c = Adafruit_NeoPixel::Color(255, 255, 255);
			}

			else if (mood->mini[mood->light_pointer][0] == 'b')
			{
				//Serial.println(" breathe");
				if (c == 'R')
					//Red
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(5, 0, 0), k);
					}
				else if (c == 'G')
					//Green
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 5, 0), k);
					}
				else if (c == 'B')
					//Blue
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 0, 5), k);
					}
				else if (c == 'Y')
					//Yellow
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(5, 5, 0), k);
					}
				else if (c == 'T')
					//Turquoise
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 5, 5), k);
					}
				else if (c == 'P')
					//Purple
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(5, 0, 5), k);
					}
				else
					//White
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(5, 5, 5), k);
					}
			}

			else if (mood->mini[mood->light_pointer][0] == 'y')
			{
				//Serial.println(" cylon");
				if (c == 'R')
				{
					//Red
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(255, 0, 0), k);
					}
					mood->b = Adafruit_NeoPixel::Color(255, 0, 0);
				}
				else if (c == 'G')
				{
					//Green
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 255, 0), k);
					}
					mood->b = Adafruit_NeoPixel::Color(0, 255, 0);
				}
				else if (c == 'B')
				{
					//Blue
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 0, 255), k);
					}
					mood->b = Adafruit_NeoPixel::Color(0, 0, 255);
				}
				else if (c == 'Y')
				{
					//Yellow
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(255, 255, 0), k);
					}
					mood->b = Adafruit_NeoPixel::Color(255, 255, 0);
				}
				else if (c == 'T')
				{
					//Turquoise
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(0, 255, 255), k);
					}
					mood->b = Adafruit_NeoPixel::Color(0, 255, 255);
				}
				else if (c == 'P')
				{
					//Purple
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(255, 0, 255), k);
					}
					mood->b = Adafruit_NeoPixel::Color(255, 0, 255);
				}
				else
				{
					//White
					for (int k = 0; k < 3; k++)
					{
						mood->colors_init[k] = split(Adafruit_NeoPixel::Color(255, 255, 255), k);
					}
					mood->b = Adafruit_NeoPixel::Color(255, 255, 255);
				}
			}

			else if (mood->mini[mood->light_pointer][0] == 't')
			{
				int i = 0;
				while (mood->mini[mood->light_pointer][i + 1] != '\0') {
					i++; //retrieves the number of colors in the twinkle script
		
				}
				mood->twinkle_array = (uint32_t *)malloc(sizeof(uint32_t) * i);
				mood->twinkle_num = i;
				mood->light2_pointer += i -1;
				char t;
				for (int j = 0; j < i; j++)
				{
					char t = mood->mini[mood->light_pointer][j];
					if (t == 'R')
						//Red
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(40, 0, 0);
					else if (t == 'G')
						//Green
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(0, 40, 0);
					else if (c == 'B')
						//Blue
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(0, 0, 40);
					else if (c == 'Y')
						//Yellow
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(40, 40, 0);
					else if (c == 'T')
						//Turquoise
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(0, 40, 40);
					else if (c == 'P')
						//Purple
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(40, 0, 40);
					else
						//White
						mood->twinkle_array[j] = Adafruit_NeoPixel::Color(40, 40, 40);
				}

			}

			mood->finish2 = 0;
		}


		else if (mood->mini[mood->light_pointer][0] == 'c' && !mood->finish2)
		{
			chase(strip, mood);
		}

		else if (mood->mini[mood->light_pointer][0] == 'b' && !mood->finish2)
		{
			breathe(strip, mood);
		}

		else if (mood->mini[mood->light_pointer][0] == 'y' && !mood->finish2)
		{
			cylon_bounce(strip, mood);
		}

		else if (mood->mini[mood->light_pointer][0] == 't' && !mood->finish2)
		{
			twinkle_random(strip, mood);
		}
	}
}

void cdefo::chase(Adafruit_NeoPixel* strip, LED* mood)
{
	//here for ease, might need to remove later if memory issues arrise
	int i = mood->led_pointer;
	//chase out
	if (i < (MOOD_LEDS / 2))
	{
		strip->setPixelColor((MOOD_LEDS / 2) + i, mood->c); // Draw new pixel
		strip->setPixelColor((MOOD_LEDS / 2) - i, mood->c);
		if (i >= 8)
		{
			strip->setPixelColor((MOOD_LEDS / 2) + i - 8, mood->start_col);
			strip->setPixelColor((MOOD_LEDS / 2) - i + 8, mood->start_col);
		}
	}

		// chase in, we add MOOD_LEDS so that it will chase in after chasing out
		// without having to keep add another variable to keep track of whether chase out has completed
	else if (i < MOOD_LEDS + 8)
	{
		if (i < MOOD_LEDS) //include this so that it doesn't overwrite onto the strip when it finishes
		{
			strip->setPixelColor(i - (MOOD_LEDS / 2), mood->c); // Draw new pixel
			strip->setPixelColor((3 * MOOD_LEDS / 2) - i, mood->c);
		}
		if (i >= (MOOD_LEDS / 2) + 8)
		{
			strip->setPixelColor(i - (MOOD_LEDS / 2) - 8, mood->start_col); // Erase pixel a few steps back
			strip->setPixelColor((3 * MOOD_LEDS / 2) - i + 8, mood->start_col);
		}
	}

	else
	{
		mood->led_pointer = -1;
		mood->finish2 = 1;
		mood->light2_pointer++;
	}

	strip->show();
	mood->led_pointer++;
}

//The lights breathe, but it fades towards the edges
void cdefo::breathe(Adafruit_NeoPixel* strip, LED* mood)
{
	//Serial.println(scale);
	//breathe in
	if (mood->led_pointer < (MOOD_LEDS / 2))
	{
		//Serial.println("in");
		//the pixels in between get lit
		for (int j = mood->led_pointer; j <= MOOD_LEDS - mood->led_pointer - 1; j++)
		{
			strip->setPixelColor(j, Adafruit_NeoPixel::Color(mood->colors_init[0] * mood->scale, mood->colors_init[1] * mood->scale,
			                                                 mood->colors_init[2] * mood->scale));
			//Serial.print(colors_init[2]);
		}
		mood->scale *= BREATHE_SCALE;
		if (mood->led_pointer == (MOOD_LEDS / 2) - 1)
			mood->scale /= BREATHE_SCALE;
		//Serial.println('\n');
	}


		//breathe out
	else if (mood->led_pointer < (MOOD_LEDS))
	{
		mood->scale /= BREATHE_SCALE;
		double scale = mood->scale;
		//the pixels in between get lit
		for (int j = MOOD_LEDS - (mood->led_pointer + 1); j <= mood->led_pointer; j++)
		{
			strip->setPixelColor(j, Adafruit_NeoPixel::Color(mood->colors_init[0] * mood->scale, mood->colors_init[1] * mood->scale,
			                                                 mood->colors_init[2] * mood->scale));
			//Serial.print(j);
		}
		//Serial.println('\n');
	}

		//reset led_pointers and breathing scale
	else
	{
		mood->led_pointer = -1;
		mood->scale = BREATHE_SCALE;
		mood->finish2 = 1;
		mood->light2_pointer++;
	}
	strip->show();
	mood->led_pointer++;
}

void cdefo::cylon_bounce(Adafruit_NeoPixel* strip, LED* mood)
{
	if (mood->led_pointer < (MOOD_LEDS - 8))
	{
		//Serial.println("cylon right");
		for (int j = 1; j <= 6; j++)
		{
			strip->setPixelColor(mood->led_pointer + j, Adafruit_NeoPixel::Color(mood->colors_init[0],
			                                                                     mood->colors_init[1],
			                                                                     mood->colors_init[2]));
		}
		//Serial.println('\n');
		//Serial.println(mood->led_pointer);

		strip->setPixelColor(mood->led_pointer, mood->start_col); //wipes the LED trailing behind
		strip->setPixelColor(mood->led_pointer + 7,
		                     Adafruit_NeoPixel::Color(mood->colors_init[0] / 10, mood->colors_init[1] / 10,
		                                              mood->colors_init[2] / 10));
	}

	else if (mood->led_pointer < (2 * MOOD_LEDS - 16))
	{
		//Serial.println("cylon left");
		for (int j = 1; j <= 6; j++)
		{
			strip->setPixelColor((MOOD_LEDS - (mood->led_pointer + j)) % MOOD_LEDS, Adafruit_NeoPixel::Color(
				                     mood->colors_init[0],
				                     mood->colors_init[1],
				                     mood->colors_init[2]));
		}
		//Serial.println('\n');
		//Serial.println(mood->led_pointer);


		strip->setPixelColor((MOOD_LEDS - mood->led_pointer) % MOOD_LEDS, mood->start_col); //wipes the LED trailing behind
		strip->setPixelColor(MOOD_LEDS - (mood->led_pointer + 7) % MOOD_LEDS,
		                     Adafruit_NeoPixel::Color(mood->colors_init[0] / 10, mood->colors_init[1] / 10,
		                                              mood->colors_init[2] / 10));
	}

	else
	{
		mood->led_pointer = -1;
		mood->finish2 = 1;
		mood->light2_pointer++;

	}

	strip->show();
	mood->led_pointer++;
}

void cdefo::twinkle_random(Adafruit_NeoPixel* strip, LED * mood)
{
	//Serial.print(mood->mini[2]);
	int rand_index = random(mood->twinkle_num);
	uint32_t col = mood->twinkle_array[rand_index];
	int damper = random(1,6); //6 levels of brightness
	uint8_t colors_scaled[3];
	for (int k = 0; k < 3; k++) 
		colors_scaled[k] = split(col, k) * damper; //splits the color and then scales it

	uint32_t rand_col = Adafruit_NeoPixel::Color(colors_scaled[0], colors_scaled[1], colors_scaled[2]);
	//Serial.print(rand_index);
	//Serial.println(": Color #");

	if(mood->led_pointer < MOOD_LEDS*3 && (millis() > mood->time_now + 50))
	{
		mood->time_now = millis();
		int led_sel = random(MOOD_LEDS);
		//Serial.print(led_sel);
		//Serial.println(": LED #");
		double scale = 1 / 6;
		for(int i = 0; i<3; i++) //fades the colors in
		{
			strip->setPixelColor(led_sel, Adafruit_NeoPixel::Color(colors_scaled[0] * scale, colors_scaled[1] * scale, colors_scaled[2] * scale));
			scale += scale;
		}
		strip->show();
		mood->led_pointer++;
		
	}

	else if(mood->led_pointer >= MOOD_LEDS * 3)
	{
		mood->led_pointer = 0;
		mood->finish2 = 1;
		mood->light2_pointer++;
		//Serial.println("finished");
		free(mood->twinkle_array);
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

uint8_t cdefo::split(uint32_t color, uint8_t i)
{
	//0 = Red, 1 = Green, 2 = Blue

	if (i == 0) return color >> 16;
	if (i == 1) return color >> 8;
	if (i == 2) return color >> 0;
	return -1;
}

void cdefo::fade(Adafruit_NeoPixel* strand, double damper)
{
	//"damper" must be between 0 and 1, or else you'll end up brightening the lights or doing nothing.

	for (int i = 0; i < strand->numPixels(); i++)
	{
		//Retrieve the color at the current position.
		uint32_t col = strand->getPixelColor(i);

		//If it's black, you can't fade that any further.
		if (col == 0) continue;

		float colors[3]; //Array of the three RGB values

		//Multiply each value by "damper"
		for (int j = 0; j < 3; j++) colors[j] = split(col, j) * damper;

		//Set the dampened colors back to their spot.
		strand->setPixelColor(i, strand->Color(colors[0], colors[1], colors[2]));
	}
}

uint32_t cdefo::Rainbow(Adafruit_NeoPixel* strand, unsigned int* i)
{
	if (*i > 1529)
	{
		unsigned int datain = *i % 1530;
		return Rainbow(strand, &datain);
	}
	if (*i > 1274) return strand->Color(255, 0, 255 - (*i % 255)); //violet -> red
	if (*i > 1019) return strand->Color((*i % 255), 0, 255); //blue -> violet
	if (*i > 764) return strand->Color(0, 255 - (*i % 255), 255); //aqua -> blue
	if (*i > 509) return strand->Color(0, 255, (*i % 255)); //green -> aqua
	if (*i > 255) return strand->Color(255 - (*i % 255), 255, 0); //yellow -> green
	return strand->Color(255, *i, 0); //red -> yellow
}


void cdefo::stop(NfcAdapter* nfc)
{
	boolean message = true;
	while (nfc->tagPresent())
	{
		if (message)
		{
			Serial.print("Experience has finished executing, place the next tag :Stop:");
			message = false;
		}
		Serial.print("Still blocking...");
	}
}

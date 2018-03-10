#include "CDEFO.h"


cdefo::cdefo(NfcTag* tag)
{
	message = tag->getNdefMessage();
	record_count = message.getRecordCount();
	//music = Adafruit_NeoPixel(N_LEDS, MUSIC_PIN, NEO_GRB + NEO_KHZ800);
	mood_strip = Adafruit_NeoPixel(N_LEDS, MOOD_PIN, NEO_GRB + NEO_KHZ800);
	mood_strip.begin();
	//music.begin();
}

void cdefo::read_ndef()
{
	if (record_count > 0);
	{
		//POTENTIALY ADD MULTITHREAD METHODS HERE
		Serial.println("Reading NDEF");
		NdefRecord record = message.getRecord(rcounter++);

		int payloadLength = record.getPayloadLength();
		byte payload[payloadLength];
		record.getPayload(payload);

		String payloadAsString = ""; // Processes the message as a string vs as a HEX value
		for (int c = 3; c < payloadLength; c++)
		{
			//POTENTIALLY ADD MULTITHREAD METHODS HERE
			payloadAsString += (char)payload[c];
		}
		current_payload = payloadAsString;

		record_count--; //decrements the number of records as we iterate through them
	}
}

String cdefo::get_current_payload()
{
	return current_payload;
}

void cdefo::reset_payload()
{
	current_payload = "";
}


void cdefo::visit_website(String line)
{
	if (current_payload != "")
		Serial.print(line);
	current_payload = "";
}

void cdefo::play_spotify(String song)
{
	if (current_payload != "")
		Serial.print(song);
	current_payload = "";
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

void cdefo::mood_script(String script)
{
	if (script != "") //if the method is called without script, then that means it is being run as part of a thread
	{
		mood += script;
	}

	//resets the payload
	current_payload = "";

	if (mood != "") //if the Arduino found a lighting script
	{
		//translate the script into actual lighting
		//starts at i = 2 because of the ":L" at the beginning
		for (int i = 2; i < mood.length(); i++)
		{
			//String mini = "";
			//looks for the dividing character to split up the scripts
			if (mood.charAt(i) == ';')
			{
				mood.remove(0, i + 1);
				mini = mood.substring(0, mood.indexOf(';')); //makes a mini script from the first ";" to the next ";"
				//adjust the index for the next iteration
				i += mood.indexOf(';');
				mood.remove(0, mood.indexOf(';')); //cleans up the main script string
			}
		}
	}
}

void cdefo::drive_lights()
{
	//process the mini script
	for (int i = 1; i < mini.length(); i++)
	{
		//POTENTIALLY ADD MULTITHREAD METHODS HERE
		uint32_t c = 0;
		//Sets the color to be passed in
		if (mini.charAt(i) == 'R')
			//Red
			c = Adafruit_NeoPixel::Color(255, 0, 0);
		if (mini.charAt(i) == 'G')
			//Green
			c = Adafruit_NeoPixel::Color(0, 255, 0);
		if (mini.charAt(i) == 'B')
			//Blue
			c = Adafruit_NeoPixel::Color(0, 0, 255);
		if (mini.charAt(i) == 'Y')
			//Yellow
			c = Adafruit_NeoPixel::Color(255, 255, 0);
		if (mini.charAt(i) == 'T')
			//Turquoise
			c = Adafruit_NeoPixel::Color(0, 255, 255);
		if (mini.charAt(i) == 'P')
			//Purple
			c = Adafruit_NeoPixel::Color(255, 0, 255);
		if (mini.charAt(i) == 'W')
			//White
			c = Adafruit_NeoPixel::Color(255, 255, 255);

		//See which type of lighting script to plug the color into
		if (mini.startsWith("C"))
		{
			chase(c);
		}
		Serial.println("exiting");
	}
}


void cdefo::stop(NfcAdapter* nfc)
{
	boolean message = true;
	while (nfc->tagPresent())
	{
		//MULTITHREADFUNCTIONS GO IN HERE
		//drive_lights();
		if (message)
		{
			Serial.println("Experience has finished executing, place the next tag :Stop:");
			message = false;
		}
	}
}


void cdefo::write_records(NfcAdapter* nfc)
{
}

void cdefo::chase(uint32_t c)
{
	Serial.print(":C:");

	while (led_counter < mood_strip.numPixels() + 4)
	{
		unsigned long currentMillis = millis();
		if (currentMillis - led_millis > INTERVAL_LED)
		{
			led_millis = currentMillis;
			mood_strip.setPixelColor(led_counter, c); // Draw new pixel
			mood_strip.setPixelColor(led_counter - 4, 0); // Erase pixel a few steps back
			mood_strip.show();
			led_counter++;
		}
	}
	led_counter = 0;
}

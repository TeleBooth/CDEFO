---
layout: page
title: The CDEFO Library and Driver
category: tutorial
---

## What Are These?
At the very minimum, what you need to get started with CDEFO is an Arduino Uno with an NFC reader. 
From there, you should download the archive available on my github, extract the files to a working directory, 
and move the `CDEFO.h` and `CDEFO.cpp` files to their own folder in your `...\Arduino\libraries\` directory.

Ignore the Python Script for now...

For starters, we should cover the design of the library, and how the included driver sketch works.

The CDEFO class can simply be seen as a wrapper for several other Arduino Libraries, primarily the [don/NDEF](https://github.com/don/NDEF) and [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) libraries.

The purpose of this was so that I could abstract all of the various commands and loops that need to be used to run those libraries correctly.

With my library, the code to read the records from an NFC tag goes from...

{% highlight c++ %}
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
{% endhighlight %}

to...

{% highlight c++ %}
cdefo::read_ndef(NfcTag* tag, char*** payloads, int* depth)
{% endhighlight %}

with that triple character pointer representing a pointer to a 2D character array that we dynamiclly allocate to store all the "payloads".

Despite the fact that I tried to abstract as many functions of dependecnies as I could, it unfortunately isn't plug and play.
You will need to provide a driver sketch that can call the proper CDEFO functions based off of what was read from `cdefo::read_ndef()`.

Good news for you, though, is that I provide a driver sketch that works right out of the box. Bad news, though, this driver sketch is only set up to drive the specific hardware setup that I had, with 2 LEDs strips and a microphone,
It's the `driver_static.ino` file in the directory of the same name, and simply ignoring the components that you don't use has little demonstrable effec in my experience.
I will provide documentation on writing the driver as I add more functionality over the summer,
but those who are extra observant will notice that the driver is essentially a `for-loop` with several `if-else` statements 
that represent each possible command that the library recognizes.

The first thing you will notice about the driver are the `#include`'s, and that there is one that hasn't been talked about yet, called `LEDTimedAction.h`

{% highlight c++ %}
#include <CDEFO.h>
#include <LEDTimedAction.h>

#define MOOD_PIN  6
#define MUSIC_PIN  12
#define AUDIO_PIN A0  //Pin for the envelope of the sound detector
{% endhighlight %}

This is a simple library that I heavily modified from its original form which allows for the use of the functions that take arguments instead of noargs function pointers.
 so that I can run multiple LED strips concurrently. This is, in my opinion, one of the crowning achievments of this project, as I've essentially multithreding an Arduino with it.
 
 ## Using Them:
 Now that you have the Library and the Sketch in a work directory andyou understand a little bit about how they both work. Let's upload the provided `write_static.ino` sketch, open the serial console, and place an NFC tag on it.
 
 Once it tells you the `"Good Job, the experience is now ready!"` upload the `driver_static.ino` and look at the console. The first thing you'll notice are the weird lines like `:S:Rope Foo` and `G:L:yRGBPT;cRGBPT;`.
 
 Well, those are the commands that the CDEFO library uses to run the user experiences, which I will now teach you how to use.
 

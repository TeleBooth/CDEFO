---
layout: page
title: Setting Up Lights
category: documentation
---

In order to keep everything static, the LEDs have two components. There's the NeoPixel LED Object instantiated in the Driver Sketch like so...

{% highlight c++ %}
#include <CDEFO.h> % note that the AdaFruit_NeoPixel library is already included in CDEFO.h
#define MOOD_PIN  6
#define MOOD_LEDS 16

Adafruit_NeoPixel mood = Adafruit_NeoPixel(MOOD_LEDS, MOOD_PIN, NEO_GRB + NEO_KHZ800);
{% endhighlight %}

And there's the LED Structure, which contains pointers to all of the variables that the LED strips could possibly use. It looks something like this...
{% highlight c++ %}
typedef struct {
	//STRUCTURE FOR EQ
	uint16_t gradient; //Used to iterate and loop through each color palette gradually

	float maxVol;    //Holds the largest volume recorded thus far to proportionally adjust the visual's responsiveness.
	float avgBump;    //Holds the "average" volume-change to trigger a "bump."

	uint8_t volume;   //Holds the volume level read from the sound detector.
	uint8_t last;     //Holds the value of volume from the previous loop() pass.

	uint8_t palette;  //Holds the current color palette.

	bool bump;     //Used to pass if there was a "bump" in volume

	//STRUCTURE FOR MOOD
	uint32_t start_col; //used to store the "greeting" color for each experience
	char **mini; //used to store the lighting scripts

	int light_number; //stores the number of lighting scripts to store
	int light_pointer; //checks which script is executing
	int light2_pointer; //checks which portion of each script is executing


	uint32_t c; //stores which color the script is using
	uint32_t b; //breathe light
	int finish2; //stores the execution state of each individual colors in the script and whether its finished
	int finish; //stores the executiong state of each full script
	int led_pointer; //controls the location of the actual LED

	char lighting_pattern;

	double scale; //keeps track of the intensity of each pixel during breathe

	uint8_t *colors_init; // used to store each script color separately

	uint32_t *twinkle_array; //stores the colors to be used in twinkle
	uint32_t twinkle_num; //number of colors that twinkle

	unsigned long time_now; //used to bake in delays
} LED;

{% endhighlight %}

By having everything contained within this structure and being calling everything through their pointers insteaad of passing them by copies,
I am able to prevent the creation of new blocks of memory, 
and subsequently prevent massive amounts of internal memory fragmentation that could potentially crash the Arduino.
I have found that this massively improves stability at the cost of only a few extra clock cycles.

Hardware wise, you only need to make sure that the LED strips you're using have strong enough pads,
otherwise you'll end up with all sorts of loose ground connections, data connections.
Loose connections were by far was the biggest issue I ran into over the course of the project...

<figure>
        <img src="http://127.0.0.1:4000/_assets/images/badpads.jpg" {% if include.alt %} alt="{{ include.alt }}" {% endif %} {% if include.width %} width="{{ include.width }}" {% endif %}/>
        <figcaption>{{ include.caption }}</figcaption>
    </figure>


and resulted in easily 70 hours worth of debugging and lab time. This can be offset by soldering.

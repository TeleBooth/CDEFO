# Welcome to Computer Driven Experiences with Familiar Objects

The aim of this project is to help inform the direction of Ubiquitous Computing by providing the tools to create an UbiComp Environment in which NFC embedded objects control the flow of the room in a free-form, artistic manner.

The website is available through [this link!](https://telebooth.github.io/CDEFO/)

## Software Dependencies:
  #### _Arduino Libraries Required:_
  - [don/NDEF NFC Mifare Tag Library](https://github.com/don/NDEF)
  - [AdaFruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
  - A modified version of Alexander Brevig's TimedAction Library that I have provided

  #### _Python Script (not required):_
  - All dependencies included in the setup.py provided

## Hardware Requirements:
  #### _Minimum:_
  - Arduino Uno R3+
  - AdaFruit NFC Shield in I2C mode (Other PN532-based shields might also work)

  #### _Recommended:_
  - WS2812B Individually Addressable LEDs
  - 10A+ 5V Power Supply
  - [Read Adafruit's Neo Pixel wiring guide](https://learn.adafruit.com/adafruit-neopixel-uberguide/the-magic-of-neopixels)
  - Any analog sound detector
  	- [I used this one from SparkFun](https://www.sparkfun.com/products/12642?_ga=2.233181918.1518020675.1526440855-1533495399.1518571797)


**The tutorial will be divided into four parts:**
1. Setting up the Arduino Sketch
2. Writing your First Experience
3. Setting up the Python Script
4. Setting up Audio/Visual Content
    
### Credits:
#### Created Using
- Arduino IDE
- Visual Studio 2017 IDE + JetBrains Resharper C++
- PyCharm IDE
- Python Imaging Library for expanded imaging capabilities
- Spotipy for music playback
- Winsound for alternate audio playback
- VLC for video bindings

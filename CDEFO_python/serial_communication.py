import serial
import webbrowser
import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import pprint
import sounddevice as sd
import soundfile as sf
import numpy

import sys

ser = serial.Serial(
    port='COM3',
    baudrate=9600
)

out = ""

# default samplerate and channels
sd.default.samplerate = 44100
sd.default.channels = 2

recording = []

#used for recording to the soundfile
def callback(indata, frames, time, status):
    """This is called (from a separate thread) for each audio block."""
    if status:
        print(status, file=sys.stderr)
    q.put(indata.copy())



#Spotify credentials
client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                           client_secret='314f354ce98344ac9cb22f6a4c50af59')
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)

while 1:
    out = ser.readline().decode("utf-8")
    #out = input('Enter your input:')
    #print(ser.read().decode("utf-8"))

    if "Firmware ver. 1.6" in out:
        print(out, flush=True)
        out = ""

    if "place a tag." in out:
        print(out)
        out = ""

    if ":L:" in out:
        print("found lights")
        out = ""

    if ":W:" in out:
        print("parsing website:")
        out = ""

    if ":We:" in out:
        print("going to website:")
        print(out.replace(":We:", ""))
        webbrowser.open(out.replace(":We:", ""), new=0, autoraise=True)
        out = ""

    if ":S:" in out:
        print("searching for Spotify artist:")
        print(out.replace(":S:", ""))
        result = sp.search(out.replace(":S:", ""))
        for key in result['tracks']['items']:
            pprint.pprint(key['name'], depth = 1)
            pprint.pprint(key['external_urls']['spotify'])

        trim = result['tracks']['items'][0]['external_urls']['spotify']
        webbrowser.open(trim.replace("'", ""), new=0, autoraise=True)
        out = ""

    if ":R:" in out:
        duration = [int(s) for s in out.split(":") if s.isdigit()]
        print("Recording for " + str(duration[0]) + " seconds.")
        recording = sd.rec(int(duration[0] * 44100))
        sd.wait()
        print("Finished")
        out = ""

        with sf.SoundFile("CDEFO.wav", mode='x', samplerate = 44100, channels = 2, subtype = "PCM_24") as file:
            file.write(recording)

    if ":P:" in out:
        sd.play(recording)

    if ":Stop:" in out:
        print(out.replace(":Stop:",""))
        out = ""
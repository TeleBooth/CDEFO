import serial
from time import sleep
import webbrowser
import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import pprint
import json

ser = serial.Serial(
    port='COM3',
    baudrate=9600
)

out = ""

#Spotify credentials
client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                           client_secret='314f354ce98344ac9cb22f6a4c50af59')
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)

while 1:
    out = ser.read().decode("utf-8")
    #out = input('Enter you input:')

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
            pprint.pprint(key['album']['external_urls'], depth = 1)
        #pprint.pprint(result, depth=4)

    if ":Stop:" in out:
        print(out.replace(":Stop:",""))
        out = ""


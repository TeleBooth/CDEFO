import serial
from time import sleep
import webbrowser
import spotipy
import pprint

ser = serial.Serial(
    port='COM3',
    baudrate=9600
)

out = ""

sp = spotipy.Spotify()

while 1:
    out = out + ser.read().decode("utf-8")

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
        pprint.pprint(result)

    if ":Stop:" in out:
        print(out.replace(":Stop:",""))
        out = ""


import serial
import threading
from time import sleep
import webbrowser
import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import pprint
import sounddevice as sd
import soundfile as sf
import numpy
from tkinter import *

import sys

# ser = serial.Serial(
#     port='COM3',
#     baudrate=9600
# )

# default samplerate and channels
sd.default.samplerate = 44100
sd.default.channels = 2

out = ""
lock = threading.Lock()

# Serial read thread
# def handle_data(data):
#     global out
#     lock.acquire()
#     out = data
#     lock.release()
#
#
#
# def read_from_port(port):
#     while True:
#         reading = port.readline().decode("utf-8")
#         print("Reading: " + reading)
#         handle_data(reading)


#ser_thread = threading.Thread(target=read_from_port, args=(ser,))

# Spotify credentials
client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                                                      client_secret='314f354ce98344ac9cb22f6a4c50af59')
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)


# TKinter widgets
class Fullscreen_Window:

    def __init__(self):
        self.state = False
        self.tk = Tk()
        self.tk.configure(background = '#2B2B2B')
        self.frame = Frame(self.tk, height = 20, width = 20)
        self.frame.configure(background = '#313335')
        self.tk.attributes("-topmost", True)
        self.tk.attributes("-fullscreen", self.state)
        self.experience = Text(self.tk, height=1, width=50)
        self.experience.pack()
        self.experience.configure(background = '#3C3F41')
        self.experience.configure(fg = '#93BABA')


        self.frame.pack()
        self.state = False
        self.tk.bind("<F11>", self.toggle_fullscreen)
        self.tk.bind("<Escape>", self.end_fullscreen)

    def toggle_fullscreen(self, event=None):
        self.state = not self.state  # Just toggling the boolean
        self.tk.attributes("-fullscreen", self.state)
        return "break"

    def end_fullscreen(self, event=None):
        self.state = False
        self.tk.attributes("-fullscreen", False)
        return "break"


w = Fullscreen_Window()
#ser_thread.start()

while 1:
    # out = ser.readline(timeout=0).decode("utf-8")
    # out = ""
    # print(ser.readline().decode("utf-8"))
    w.tk.update()

    if "Firmware ver. 1.6" in out:
        w.experience.insert('0.0', out)
        lock.acquire
        out = ""
        lock.release

    if "place a tag." in out:
        lock.acquire
        out = ""
        lock.release

    if ":L:" in out:
        print("found lights")
        lock.acquire
        out = ""
        lock.release

    if ":W:" in out:
        print("parsing website:")
        lock.acquire
        out = ""
        lock.release

    if ":We:" in out:
        w.experience.insert('0.0', "going to website:" + out.replace(":We:", ""))
        webbrowser.open(out.replace(":We:", ""), new=1, autoraise=False)
        lock.acquire
        out = ""
        lock.release

    if ":S:" in out:
        w.experience.insert('0.0', "Searching for Spotify artist:" + out.replace(":S:", ""))
        # print("searching for Spotify artist:")
        # print(out.replace(":S:", ""))
        result = sp.search(out.replace(":S:", ""))
        for key in result['tracks']['items']:
            pprint.pprint(key['name'], depth=1)
            pprint.pprint(key['external_urls']['spotify'])

        trim = result['tracks']['items'][0]['external_urls']['spotify']
        webbrowser.open(trim.replace("'", ""), new=1, autoraise=False)
        lock.acquire
        out = ""
        lock.release

    if ":R:" in out:
        duration = [int(s) for s in out.split(":") if s.isdigit()]
        print(duration)
        print("Recording for " + str(duration[0]) + " seconds.")
        print("Finished")
        # if ":P:" in out:
        #     sd.wait()

        # with sf.SoundFile("CDEFO.wav", mode='x', samplerate=44100, channels=2, subtype="PCM_24") as file:
        #     file.write(recording)
        lock.acquire
        out = ""
        lock.release

    if ":End:" in out:
        lock.acquire
        out = ""
        lock.release

    if "Cleanup finished :Stop:" in out:
        w.experience.insert('0.0', out)
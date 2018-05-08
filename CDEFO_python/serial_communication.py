import serial
from threading import *
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

ser = serial.Serial(
    port='COM3',
    baudrate=9600
)

# default samplerate and channels
sd.default.samplerate = 44100
sd.default.channels = 2

# Lock for the GUI
gui_lock = Lock()
# timer thread used for recording
class RecThread(Thread):
    def __init__(self, event):
        Thread.__init__(self)
        self.stopped = event

    def run(self):
        while not self.stopped.wait(1):
            # call a function
            gui_lock.acquire()
            w.experience.insert(END, out)
            gui_lock.release()



# lock and global for serial reading
out = ""
lock = Lock()

# Serial read thread
def handle_data(data):
    global out
    lock.acquire()
    out = data
    lock.release()



def read_from_port(port):
    while True:
        reading = port.readline().decode("utf-8")
        print("Reading: " + reading)
        handle_data(reading)


ser_thread = Thread(target=read_from_port, args=(ser,))

# Spotify credentials
client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                                                      client_secret='314f354ce98344ac9cb22f6a4c50af59')
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)


# TKinter widgets
class Fullscreen_Window:

    def __init__(self):
        self.state = False
        self.tk = Tk()
        self.tk.geometry("1280x720")

        # Windows frame
        self.window_frame = Frame(self.tk)
        self.window_frame.configure(background = '#2B2B2B')
        self.window_frame.pack_propagate(0)
        Grid.rowconfigure(self.tk, 0, weight=1)
        Grid.columnconfigure(self.tk, 0, weight=1)
        self.window_frame.grid(row=0, columnspan=2, sticky=N + S + E + W)


        # frame that holds the title
        self.title_frame = Frame(self.window_frame)
        Grid.rowconfigure(self.window_frame, 0, weight=1)
        Grid.columnconfigure(self.window_frame, 0, weight=1)
        self.title_frame.configure(background ='#313335')
        self.title_frame.pack_propagate(0)
        self.title_frame.grid(row = 0, columnspan = 2, padx = 10, pady = 10, sticky = N+S+E+W)

        # frame that holds the text
        self.frame = Frame(self.window_frame, height=200, width=200)
        Grid.rowconfigure(self.window_frame, 1, weight=2)
        Grid.columnconfigure(self.window_frame, 1, weight=1)
        self.frame.configure(background='#313335')
        self.frame.pack_propagate(0)
        self.frame.grid(row = 1, column = 1, padx = 10, pady = 10, sticky = N+S+E+W)

        # self.title_frame.pack()
        self.experience = Text(self.frame, height=1, width=1)
        # self.experience.grid(in_ =  self.frame, padx = 20, pady = 20)
        self.experience.pack(fill = BOTH, expand = 1, padx = 20, pady = 20)
        self.experience.configure(background = '#3C3F41')
        self.experience.configure(foreground = '#93BABA')
        self.experience.configure(borderwidth = 1)

        # frame that holds videos and images
        self.AV = Frame(self.window_frame, height=200, width=200)
        Grid.columnconfigure(self.window_frame, 0, weight=1)
        self.AV.configure(background='#313335')
        self.AV.grid(row=1, column=0, padx=10, pady=10, sticky=N + S + E + W)
        # images are drawn on the canvas
        Grid.columnconfigure(self.AV, 0, weight = 1)
        Grid.rowconfigure(self.AV, 0, weight = 10)
        Grid.rowconfigure(self.AV, 1, weight = 1)
        self.canvas = Canvas(self.AV).grid(row=0, columnspan=4, padx=10, pady=10, sticky=N + S + E + W)

        ctrlpanel = Frame(self.AV)
        pause = Button(ctrlpanel, text="Pause")#, command=self.OnPause)
        play = Button(ctrlpanel, text="Play")#, command=self.OnPlay)
        stop = Button(ctrlpanel, text="Stop")#, command=self.OnStop)
        volume = Button(ctrlpanel, text="Volume")#, command=self.OnSetVolume)
        pause.grid(row=1, column = 0, padx=5, pady=5)
        play.grid(row=1, column = 1, padx=5, pady=5)
        stop.grid(row=1, column = 2, padx=5, pady=5)
        volume.grid(row=1, column = 3, padx=5, pady=5)

        self.tk.attributes("-topmost", True)
        self.tk.attributes("-fullscreen", self.state)
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
ser_thread.start()

while 1:
    # out = ser.readline(timeout=0).decode("utf-8")
    # out = ""
    # print(ser.readline().decode("utf-8"))
    w.tk.update()

    if "Firmware ver. 1.6" in out:
        gui_lock.acquire()
        w.experience.insert(END, out)
        gui_lock.release()
        lock.acquire()
        out = ""
        lock.release()

    if "place a tag." in out:
        lock.acquire()
        out = ""
        lock.release()

    if ":L:" in out:
        print("found lights")
        lock.acquire()
        out = ""
        lock.release()

    if ":W:" in out:
        print("parsing website:")
        lock.acquire()
        out = ""
        lock.release()

    if ":We:" in out:
        gui_lock.acquire()
        w.experience.insert(END, "going to website:" + out.replace(":We:", ""))
        gui_lock.release()
        webbrowser.open(out.replace(":We:", ""), new=1, autoraise=False)
        lock.acquire()
        out = ""
        lock.release()

    if ":S:" in out:
        gui_lock.acquire()
        w.experience.insert(END, "Searching for Spotify artist:" + out.replace(":S:", ""))
        gui_lock.release()
        # print("searching for Spotify artist:")
        # print(out.replace(":S:", ""))
        result = sp.search(out.replace(":S:", ""))
        for key in result['tracks']['items']:
            pprint.pprint(key['name'], depth=1)
            pprint.pprint(key['external_urls']['spotify'])

        trim = result['tracks']['items'][0]['external_urls']['spotify']
        webbrowser.open(trim.replace("'", ""), new=1, autoraise=False)
        lock.acquire()
        out = ""
        lock.release()

    if ":R:" in out:
        duration = [int(s) for s in out.split(":") if s.isdigit()]
        print(duration)
        print("Recording for " + str(duration[0]) + " seconds...")
        print("Finished")
        # if ":P:" in out:
        #     sd.wait()

        # with sf.SoundFile("CDEFO.wav", mode='x', samplerate=44100, channels=2, subtype="PCM_24") as file:
        #     file.write(recording)
        lock.acquire()
        out = ""
        lock.release()

    if ":End:" in out:
        lock.acquire()
        out = ""
        lock.release()

    if "Cleanup finished :Stop:" in out:
        gui_lock.acquire()
        w.experience.delete(1.0, END)
        gui_lock.release()
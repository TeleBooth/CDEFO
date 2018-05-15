from tkinter import Event
from typing import Union

import serial
import webbrowser
import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import pprint
import vlc
import sys

import winsound
from tkinter import *
from PIL import ImageTk, Image

from threading import Thread, Event, Lock


# Class to read from the Serial Port
class SerialThread(Thread):

    def __init__(self, port):
        Thread.__init__(self)
        self.reset_flag = Event()
        self.port = port
        self.reading = ""

    def run(self):
        while not self.reset_flag.is_set():
            self.read_from_port()

    def reset(self):
        self.reset_flag.set()

    # lock and global for serial reading
    # Serial read thread

    def read_from_port(self):
        while True:
            try:
                self.reading = self.port.readline().decode("utf-8")
                print("Reading: " + self.reading)
                self.handle_data()
            except IOError:
                while True:
                    try:
                        self.port.close()
                        self.port.open()
                    except IOError:
                        pass
                    else:
                        break

    def handle_data(self):
        out = self.reading
        if "Firmware ver. 1.6" in out:
            gui_lock.acquire()
            w.title.insert(END, "Welcome to the Capstone Fair!")
            w.experience.insert(END, "Maybe it'd be interesting to put one of the objects on the receiver?")
            gui_lock.release()
            out = ""

        if "place a tag." in out:
            out = ""

        if ":L:" in out:
            print("found lights")
            out = ""

        # the text that describes the experience is updated when the videos and images are
        if ":V:" in out:
            file_name = ((out.replace(":V:", "text_")).split(".")[0]).rstrip() + ".txt"
            with open(file_name, 'r') as f:
                gui_lock.acquire()
                w.experience.delete("1.0", END)
                w.experience.insert(INSERT, f.read())
                w.toggle_video(out.replace(":V:", "").rstrip())
                gui_lock.release()

        if ":P:" in out:
            file_name = ((out.replace(":P:", "text_")).split(".")[0]).rstrip() + ".txt"
            with open(file_name, 'r') as f:
                gui_lock.acquire()
                w.experience.delete("1.0", END)
                w.experience.insert(INSERT, f.read())
                w.toggle_picture(out.replace(":P:", "").rstrip())
                gui_lock.release()

        if ":Pl:" in out:
            winsound.PlaySound((out.replace(":Pl:", "")).rstrip(), winsound.SND_ASYNC)

        if ":W:" in out:
            print("parsing website:")
            out = ""

        if ":We:" in out:
            gui_lock.acquire()
            w.experience.insert(END, "going to website:" + out.replace(":We:", ""))
            gui_lock.release()
            webbrowser.open(out.replace(":We:", ""), autoraise=False)
            out = ""

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

            x = lambda: webbrowser.open_new(trim.replace("'", ""))
            t = Thread(target=x)
            t.start()
            out = ""

        # if ":R:" in out:
        #     duration = [int(s) for s in out.split(":") if s.isdigit()]
        #     print(duration)
        #     print("Recording for " + str(duration[0]) + " seconds...")
        #     print("Finished")
        #     # if ":P:" in out:
        #     #     sd.wait()
        #
        #     # with sf.SoundFile("CDEFO.wav", mode='x', samplerate=44100, channels=2, subtype="PCM_24") as file:
        #     #     file.write(recording)
        #     lock.acquire()
        #     out = ""
        #     lock.release()

        if ":End:" in out:
            out = ""

        if "Cleanup finished :Stop:" in out:
            gui_lock.acquire()
            w.experience.delete(1.0, END)
            gui_lock.release()


# Class that represents the GUI
class Fullscreen_Window:

    def __init__(self):
        self.state = False
        self.tk = Tk()
        self.tk.geometry("1280x720")

        # Windows frame
        self.window_frame = Frame(self.tk)
        self.window_frame.configure(background='#2B2B2B')
        self.window_frame.pack_propagate(0)
        Grid.rowconfigure(self.tk, 0, weight=1)
        Grid.columnconfigure(self.tk, 0, weight=1)
        self.window_frame.grid(row=0, column=0, sticky=N + S + E + W)

        # allocate the grid
        Grid.rowconfigure(self.window_frame, 0, weight=1)
        Grid.columnconfigure(self.window_frame, 0, weight=1)
        Grid.rowconfigure(self.window_frame, 1, weight=1)
        Grid.columnconfigure(self.window_frame, 1, weight=1)

        # frame that holds the title
        self.title_frame = Frame(self.window_frame)
        self.title_frame.configure(background='#313335')
        self.title_frame.pack_propagate(0)
        self.title_frame.grid(row=0, columnspan=2, padx=10, pady=10, sticky=N + S + E + W)

        self.title = Text(self.title_frame)
        self.title.pack(fill=BOTH, expand=1, padx=20, pady=20)
        self.title.configure(background='#3C3F41')
        self.title.configure(foreground='#93BABA')
        self.title.configure(borderwidth=1)
        self.title.configure(font=("Helvetica", 40))

        # frame that holds the text
        self.frame = Frame(self.window_frame, height=200, width=200)
        self.frame.configure(background='#313335')
        self.frame.pack_propagate(0)
        self.frame.grid(row=1, column=1, padx=10, pady=10, sticky=N + S + E + W)

        self.experience = Text(self.frame)
        self.experience.pack(fill=BOTH, expand=1, padx=20, pady=20)
        self.experience.configure(background='#3C3F41')
        self.experience.configure(foreground='#93BABA')
        self.experience.configure(borderwidth=1)
        self.experience.configure(font=("TkDefaultFont", 14))

        # frame that holds videos and images
        self.AV = Frame(self.window_frame, height=200, width=200)
        Grid.columnconfigure(self.window_frame, 0, weight=1)
        Grid.rowconfigure(self.window_frame, 1, weight=2)
        self.AV.configure(background='#313335')
        self.AV.grid(row=1, column=0, padx=10, pady=10, sticky=N + S + E + W)

        self.canvas = Canvas(self.AV)
        self.player = None

        self.tk.attributes("-topmost", True)
        self.tk.attributes("-fullscreen", self.state)
        self.state = False
        self.tk.bind("<F11>", self.toggle_fullscreen)
        self.tk.bind("<Escape>", self.end_fullscreen)

    def toggle_video(self, location):
        # images are drawn on the canvas
        Grid.columnconfigure(self.AV, 0, weight=1)
        # holds the video frame
        Grid.rowconfigure(self.AV, 0, weight=15)
        # holds the button frame
        Grid.rowconfigure(self.AV, 1, weight=1)
        # video player elements
        self.canvas.grid(row=0, column=0, padx=10, pady=10, sticky=N + S + E + W)

        # VLC player controls
        self.Instance = vlc.Instance()
        self.player = self.Instance.media_player_new()
        # below is a test, now use the File->Open file menu
        media = self.Instance.media_new(location)
        self.player.set_media(media)
        self.player.play()  # hit the player button
        self.player.video_set_deinterlace(vlc.str_to_bytes('yadif'))

        self.player.set_hwnd(self.GetHandle())  # for windows, OnOpen does does this

    def toggle_picture(self, location):
        Grid.rowconfigure(self.AV, 0, weight=1)
        Grid.columnconfigure(self.AV, 0, weight=1)
        self.original = Image.open(location)
        self.display = Canvas(self.AV, bd=0, highlightthickness=0, relief=SUNKEN, background='#313335')

        # resizing
        self.display.grid(row=0, padx=10, pady=10, sticky=N + S + E + W)
        self.tk.update()

        # size = (self.AV.winfo_width(), self.AV.winfo_height())
        size = (
        int(self.display.winfo_height() * (self.original.width / self.original.height)), self.display.winfo_height())
        resized = self.original.resize(size, Image.ANTIALIAS)

        self.img = ImageTk.PhotoImage(resized)

        self.display.create_image(int(self.display.winfo_width() / 2), int(self.display.winfo_height() / 2),
                                  anchor=CENTER,
                                  image=self.img
                                  , tags="IMG")

    def toggle_fullscreen(self, event=None):
        self.state = not self.state  # Just toggling the boolean
        self.tk.attributes("-fullscreen", self.state)
        return "break"

    def end_fullscreen(self, event=None):
        self.state = False
        self.tk.attributes("-fullscreen", False)
        return "break"

    def GetHandle(self):
        return self.canvas.winfo_id()

    # def OnPause(self, evt):
    def OnPause(self):
        """Pause the player.
        """
        self.player.pause()

    def OnStop(self):
        """Stop the player.
        """
        self.player.stop()

    def errorDialog(self, errormessage):
        """Display a simple error dialog.
        """
        Tk.tkMessageBox.showerror(self, 'Error', errormessage)


if __name__ == "__main__":

    # waits for the serial port to open
    while True:
        try:
            ser = serial.Serial(
                port='COM3',
                baudrate=9600
            )
            ser.isOpen()
            print("port is opened!")

        except IOError:
            pass
        else:
            break

    # Spotify credentials
    client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                                                          client_secret='314f354ce98344ac9cb22f6a4c50af59')
    sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)

    gui_lock = Lock()

    ser_thread = SerialThread(ser)
    ser_thread.start()

    w = Fullscreen_Window()
    w.tk.mainloop()

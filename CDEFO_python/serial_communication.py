import serial
import webbrowser
import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import pprint
import vlc
import sys

# import sounddevice as sd
# import soundfile as sf
# import numpy
from tkinter import *
from tkinter.filedialog import askopenfilename
from PIL import ImageTk, Image

from threading import Thread, Event, Lock
import os
import pathlib
import time
import platform


# a timer thread that is used for the time slider which won't be implemented
class ttkTimer(Thread):
    """a class serving same function as wxTimer... but there may be better ways to do this
    """

    def __init__(self, callback, tick):
        Thread.__init__(self)
        self.callback = callback
        self.stopFlag = Event()
        self.tick = tick
        self.iters = 0

    def run(self):
        while not self.stopFlag.wait(self.tick):
            self.iters += 1
            self.callback()

    def stop(self):
        self.stopFlag.set()

    def get(self):
        return self.iters


# TKinter widgets
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

        # frame that holds videos and images
        self.AV = Frame(self.window_frame, height=200, width=200)
        Grid.columnconfigure(self.window_frame, 0, weight=1)
        Grid.rowconfigure(self.window_frame, 1, weight = 2)
        self.AV.configure(background='#313335')
        self.AV.grid(row=1, column=0, padx=10, pady=10, sticky=N + S + E + W)

        self.tk.attributes("-topmost", True)
        self.tk.attributes("-fullscreen", self.state)
        self.state = False
        self.tk.bind("<F11>", self.toggle_fullscreen)
        self.tk.bind("<Escape>", self.end_fullscreen)

    def toggle_video(self, location, event=None):
        # images are drawn on the canvas
        Grid.columnconfigure(self.AV, 0, weight=1)
        # holds the video frame
        Grid.rowconfigure(self.AV, 0, weight=15)
        # holds the time slider frame
        Grid.rowconfigure(self.AV, 1, weight=1)
        # holds the button frame
        Grid.rowconfigure(self.AV, 2, weight=1)
        # video player elements
        self.canvas = Canvas(self.AV).grid(row=0, column=0, padx=10, pady=10, sticky=N + S + E + W)

        # buttons for the player
        ctrlpanel = Frame(self.AV)
        ctrlpanel.configure(background='#313335')
        pause = Button(ctrlpanel, text="Pause", command=self.OnPause)
        pause.configure(foreground='#93BABA', background='#3C3F41')
        play = Button(ctrlpanel, text="Play", command=self.OnPlay)
        play.configure(foreground='#93BABA', background='#3C3F41')
        stop = Button(ctrlpanel, text="Stop", command=self.OnStop)
        stop.configure(foreground='#93BABA', background='#3C3F41')
        volume = Button(ctrlpanel, text="Volume", command=self.OnSetVolume)
        volume.configure(foreground='#93BABA', background='#3C3F41')
        Grid.columnconfigure(ctrlpanel, 0, weight=1)
        Grid.columnconfigure(ctrlpanel, 1, weight=1)
        Grid.columnconfigure(ctrlpanel, 2, weight=1)
        Grid.columnconfigure(ctrlpanel, 3, weight=1)
        Grid.rowconfigure(ctrlpanel, 0, weight=1)
        Grid.rowconfigure(ctrlpanel, 1, weight=0)
        pause.grid(row=0, column=0, padx=5, pady=5)
        play.grid(row=0, column=1, padx=5, pady=5)
        stop.grid(row=0, column=2, padx=5, pady=5)
        volume.grid(row=0, column=3, padx=5, pady=5)

        # time slider for VLC video
        ctrlpanel2 = Frame(self.AV)
        ctrlpanel2.configure(background='#313335')
        scale_var = DoubleVar()
        self.timeslider_last_val = ""
        timeslider = Scale(ctrlpanel2, variable=scale_var, command=self.scale_sel(),
                                from_=0, to=1000, orient=HORIZONTAL, length=500)
        timeslider.configure(foreground='#93BABA', background='#3C3F41', activebackground='#3C3F41',
                                  highlightbackground='#3C3F41', troughcolor='#93BABA')
        timeslider.pack(side=BOTTOM, fill=X, expand=1, padx=10)
        self.timeslider_last_update = time.time()

        ctrlpanel.grid(row=2, column=0, sticky=N + S + E + W)
        ctrlpanel2.grid(row=1, column=0, sticky=N + S + E + W)

        # VLC player controls
        self.Instance = vlc.Instance()
        self.player = self.Instance.media_player_new()
        # below is a test, now use the File->Open file menu
        media = self.Instance.media_new(location)
        self.player.set_media(media)
        self.player.play()  # hit the player button
        self.player.video_set_deinterlace(vlc.str_to_bytes('yadif'))
        self.timer = ttkTimer(self.OnTimer, 1.0)
        self.timer.start()
        # self.player.set_hwnd(self.GetHandle()) # for windows, OnOpen does does this

    def toggle_picture(self, location):
        Grid.rowconfigure(self.AV, 0, weight=1)
        Grid.columnconfigure(self.AV, 0, weight=1)
        self.original = Image.open(location)
        self.display = Canvas(self.AV, bd=0, highlightthickness=0, relief = SUNKEN)

        # resizing
        self.tk.update()

        print(self.AV.winfo_width())
        size = (self.AV.winfo_width(), self.AV.winfo_height())
        resized = self.original.resize(size, Image.ANTIALIAS)

        self.img = ImageTk.PhotoImage(resized)
        self.display.create_image(0, 0, image=self.img, anchor=CENTER
                                  , tags="IMG")
        self.display.grid(row=0, padx = 10, pady = 10, sticky=W + E + N + S)
        pass

    def toggle_fullscreen(self, event=None):
        self.state = not self.state  # Just toggling the boolean
        self.tk.attributes("-fullscreen", self.state)
        return "break"

    def end_fullscreen(self, event=None):
        self.state = False
        self.tk.attributes("-fullscreen", False)
        return "break"

    def OnExit(self, evt):
        """Closes the window.
        """
        self.Close()

    def OnOpen(self):
        """Pop up a new dialow window to choose a file, then play the selected file.
        """
        # if a file is already running, then stop it.
        self.OnStop()

        # Create a file dialog opened in the current home directory, where
        # you can display all kind of files, having as title "Choose a file".
        p = pathlib.Path(os.path.expanduser("~"))
        fullname = askopenfilename(initialdir=p, title="choose your file",
                                   filetypes=(("all files", "*.*"), ("mp4 files", "*.mp4")))
        if os.path.isfile(fullname):
            dirname = os.path.dirname(fullname)
            filename = os.path.basename(fullname)
            # Creation
            self.Media = self.Instance.media_new(str(os.path.join(dirname, filename)))
            self.player.set_media(self.Media)
            # Report the title of the file chosen
            # title = self.player.get_title()
            #  if an error was encountred while retriving the title, then use
            #  filename
            # if title == -1:
            #    title = filename
            # self.SetTitle("%s - tkVLCplayer" % title)

            # set the window id where to render VLC's video output
            if platform.system() == 'Windows':
                self.player.set_hwnd(self.GetHandle())
            else:
                self.player.set_xwindow(self.GetHandle())  # this line messes up windows
            self.OnPlay()

            # set the volume slider to the current volume
            # self.volslider.SetValue(self.player.audio_get_volume() / 2)
            self.volslider.set(self.player.audio_get_volume())

    def OnPlay(self):
        """Toggle the status to Play/Pause.
        If no file is loaded, open the dialog window.
        """
        # check if there is a file to play, otherwise open a
        # Tk.FileDialog to select a file
        if not self.player.get_media():
            self.OnOpen()
        else:
            # Try to launch the media, if this fails display an error message
            if self.player.play() == -1:
                self.errorDialog("Unable to play.")

    def GetHandle(self):
        return self.videopanel.winfo_id()

    # def OnPause(self, evt):
    def OnPause(self):
        """Pause the player.
        """
        self.player.pause()

    def OnStop(self):
        """Stop the player.
        """
        self.player.stop()
        # reset the time slider
        self.timeslider.set(0)

    def OnTimer(self):
        """Update the time slider according to the current movie time.
        """
        if self.player is None:
            return
        # since the self.player.get_length can change while playing,
        # re-set the timeslider to the correct range.
        length = self.player.get_length()
        dbl = length * 0.001
        self.timeslider.config(to=dbl)

        # update the time on the slider
        tyme = self.player.get_time()
        if tyme == -1:
            tyme = 0
        dbl = tyme * 0.001
        self.timeslider_last_val = ("%.0f" % dbl) + ".0"
        # don't want to programatically change slider while user is messing with it.
        # wait 2 seconds after user lets go of slider
        if time.time() > (self.timeslider_last_update + 2.0):
            self.timeslider.set(dbl)

    def scale_sel(self, evt):
        if self.player is None:
            return
        nval = self.scale_var.get()
        sval = str(nval)
        if self.timeslider_last_val != sval:
            # this is a hack. The timer updates the time slider.
            # This change causes this rtn (the 'slider has changed' rtn) to be invoked.
            # I can't tell the difference between when the user has manually moved the slider and when
            # the timer changed the slider. But when the user moves the slider tkinter only notifies
            # this rtn about once per second and when the slider has quit moving.
            # Also, the tkinter notification value has no fractional seconds.
            # The timer update rtn saves off the last update value (rounded to integer seconds) in timeslider_last_val
            # if the notification time (sval) is the same as the last saved time timeslider_last_val then
            # we know that this notification is due to the timer changing the slider.
            # otherwise the notification is due to the user changing the slider.
            # if the user is changing the slider then I have the timer routine wait for at least
            # 2 seconds before it starts updating the slider again (so the timer doesn't start fighting with the
            # user)
            self.timeslider_last_update = time.time()
            mval = "%.0f" % (nval * 1000)
            self.player.set_time(int(mval))  # expects milliseconds

    def volume_sel(self, evt):
        if self.player == None:
            return
        volume = self.volume_var.get()
        if volume > 100:
            volume = 100
        if self.player.audio_set_volume(volume) == -1:
            self.errorDialog("Failed to set volume")

    def OnToggleVolume(self, evt):
        """Mute/Unmute according to the audio button.
        """
        is_mute = self.player.audio_get_mute()

        self.player.audio_set_mute(not is_mute)
        # update the volume slider;
        # since vlc volume range is in [0, 200],
        # and our volume slider has range [0, 100], just divide by 2.
        self.volume_var.set(self.player.audio_get_volume())

    def OnSetVolume(self):
        """Set the volume according to the volume sider.
        """
        volume = self.volume_var.get()
        # vlc.MediaPlayer.audio_set_volume returns 0 if success, -1 otherwise
        if volume > 100:
            volume = 100
        if self.player.audio_set_volume(volume) == -1:
            self.errorDialog("Failed to set volume")

    def errorDialog(self, errormessage):
        """Display a simple error dialog.
        """
        Tk.tkMessageBox.showerror(self, 'Error', errormessage)


# lock and global for serial reading
# Serial read thread
def handle_data(data):
    global out
    out = data
    if "Firmware ver. 1.6" in out:
        gui_lock.acquire()
        w.experience.insert(END, "Welcome to the Capstone Fair!")
        gui_lock.release()
        out = ""

    if "place a tag." in out:
        out = ""

    if ":L:" in out:
        print("found lights")
        out = ""

    if ":V:" in out:
        gui_lock.acquire()
        w.toggle_video(out.replace(":V:", ""))
        gui_lock.release()

    if ":P:" in out:
        gui_lock.acquire()
        w.toggle_picture(out.replace(":P:", ""))
        gui_lock.release()

    if ":W:" in out:
        print("parsing website:")
        out = ""

    if ":We:" in out:
        gui_lock.acquire()
        w.experience.insert(END, "going to website:" + out.replace(":We:", ""))
        gui_lock.release()
        webbrowser.open(out.replace(":We:", ""), new=1, autoraise=False)
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
        webbrowser.open(trim.replace("'", ""), new=1, autoraise=False)
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


def read_from_port(port):
    while True:
        try:
            reading = port.readline().decode("utf-8")
            print("Reading: " + reading)
            handle_data(reading)
        except IOError:
            while True:
                try:
                    port.close()
                    port.open()
                except IOError:
                    pass
                else:
                    break


if __name__ == "__main__":

    # # waits for the serial port to open
    # while True:
    #     try:
    #         ser = serial.Serial(
    #             port='COM3',
    #             baudrate=9600
    #         )
    #         ser.isOpen()
    #         print("port is opened!")
    #
    #     except IOError:
    #         pass
    #     else:
    #         break

    # Spotify credentials
    client_credentials_manager = SpotifyClientCredentials(client_id='0f65027d2c8e42bc9499eb12e885cb62',
                                                          client_secret='314f354ce98344ac9cb22f6a4c50af59')
    sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)

    gui_lock = Lock()

    # ser_thread = Thread(target=read_from_port, args=(ser,))
    # ser_thread.start()

    output = input("Accepting keyboard input: ")
    w = Fullscreen_Window()
    w.toggle_picture(output.replace(":P:", ""))

    w.tk.mainloop()

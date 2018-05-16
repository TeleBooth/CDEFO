---
layout: page
title: Setting Up the Python Script
category: tutorial
---

After ignoring the Python script for so long, we're finally going to use it.

Assuming you have Python installed already, all you need to do is enter your command line of choice, `cd \path\to\the\script\and\setup.py`, and run, `python setup.py install`. 
For anyone knew to python, what this does is sets up the Python script along with all of its dependencies as a module that can run on its own (it still uses the PYthon interpreter, though),
It just makes it so all of the dependencies it needs are in its directory and you don't need to worry about installing them yourself or making a virtual environment.

Again, the more observant among you will note that this python script has nearly the same format as the driver. 
The Python Script has one continuously running Thread Class that reads from the serial port and has a series of if statements that each line can satisfy.

Any other threads that I create are used to run things like Webbrowsers and GUI elements without blocking the read.

**Warning: If you modify the Python Script to use blocking functions, it will stop the Arduino from executing,
since the Arduino will wait for its Serial.output to be read before continuing, no matter what its doing.**

Here's the `handle_data` function from the `SerialThread` class which demonstrates how closely the driver and script resemble each other.

{% highlight python %}

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

        if ":End:" in out:
            out = ""

        if "Cleanup finished :Stop:" in out:
            gui_lock.acquire()
            w.experience.delete(1.0, END)
            gui_lock.release()

{% endhighlight %}

The Python script here pretty much handles everything that the Arduino can't,
such as Audio/Video playback, and serving large amounts of text that would normally crash an Arduino

When it comes to this script, there are only three things to note:
1. Running the script with the Arduino plugged acts like a Serial Console and reset the Arduino, this is useful if there is an unexpected crash, or you wish to sync the Arduino and script together (which is absolutely intended)
2. Any files that you plan on serving with the script **must** be in the same directory as the script. The naming scheme is also very strict, and the `file_name` should be kept short so as to fit on an NFC tag.
3. Text files are displayed alongside photos or videos (`:P:` and `:V:` respectively) and they must follow the naming convention "text_{AV_file_name}.txt"

An example of the Python script working together the Arduino can be seen in one of the demonstrations I prepared for the Capstone Fair.
Here is the story of John Fitzell's Hedgehog.
{% include youtubePlayer.html id="SxYcZkiHW9E" %}

Looking at the directory tree where I had the script running, along with the commands I had written to the NFC tag help to demystify what (3) in particular means.

<figure>
        <img src="{{ site.baseurl }}/_assets/images/Directory_Python.jpg"/>
        <figcaption>See, looks how bad these pads got. They literally have holes in them and you don't even know it until you pry them off manually</figcaption>
</figure>

{% highlight c++ %}

void loop() {
    Serial.println("\nPlace an NFC Tag that you want to Record these Messages on!"); // Command for the Serial Monitor
    if (nfc.tagPresent()) {
        NdefMessage message = NdefMessage();
        message.addTextRecord(":Pl:TurtleA.wav");
        message.addTextRecord(":V:TurtleV.mp4");
        message.addTextRecord("B:L:bBT;yTB;tTB");
        message.addTextRecord(":Stop:");
        boolean success = nfc.write(message);
        if (success) {
            Serial.println("Good Job, the experience is now ready!"); // if it works you will see this message 
        } else {
            Serial.println("Write failed"); // If the the rewrite failed you will see this message
        }
    }
    delay(5000);
}
{% endhighlight %}

The second `TextRecord` recorded in this experience is `":V:TurtleV.mp4"` and according to the naming convention defined in (3),
this means that the text that the script will try to serve will be named `"text_TurtleV.txt"`.

Other than this, it's really only pertinent to let you know that the Python script is signficantly more stable than the Arduino,
simply because of how fragile the Arduino are and how picky they are when it comes to the proper working conditions. This last experience I showed you was recorded moments after this...

{% include youtubePlayer.html id="YiPeeaX3jfE" %}

In this experience, I have an EQ running (covered in the docs) along with the regular Mood Lighting scripts and the Python script. 
You can see at the beginning the script actually restarts in the middle because in the process of running the LEDs and the NFC readers at the same time, 
the Arduino hard resets for an unknown reason. From what I've gathered, though, it has something to do with the quality of the ground on the DC Power Supply that I use to power the LEDs 
(which is odd, since the Arduino isn't even connected to the power through there, it's only grounded through it.





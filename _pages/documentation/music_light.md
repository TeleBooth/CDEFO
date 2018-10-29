---
layout: page
title: What Went Wrong?
category: documentation
---

So, in the days leading up to the Capstone Fair, I was getting very weird hardware bugs that couldn't be reliably reproduced.
The first bug was the EQ LED strip freaking out, as thought it weren't grounded, however it only occured when the NFC reader was being used.

Here, the music visualizer works flawlessly when the NFC Reader isn't active
{% include youtubePlayer.html id="OJe4WWOKSy0" %}

And here, the moment the NFC reads the tag on the object, the EQ flips out, and it actually causes the entire system to hard reboot 
(you can see because it goes from yellow to blue twice before reaching the end, when it's only ever called once in the whole program)
{% include youtubePlayer.html id="YiPeeaX3jfE" %}

Normally, this wouldn't be much of an issue, but if you look at the video, you'll notice that it actually crashes the Arduino, restarts it,
and then restarts the python script, resulting in both of them stalling as they wait for NFC commands, even though the NFC Reader isn't functional because it wasn't properly restarted.
Afterwards the Arduino will simply stop communicating over Serial and return 
`Read Failed 10` while only reading the latter half of the payloads, as though after it restarted it didn't wipe its memory.

Recently, the only way to fix the `Read Failed 10` error is to run the Arduino without the DC power plugged in (even though the DC power isn't connected to the NFC reader)
Sometimes, I'll even get a new `Read Failed 14` error or whatever this is
<figure>
        <img src="{{ site.baseurl }}/_assets/images/programmer.png"/>
        <figcaption>`avrdude` errors usually happen at compilation, but this happened in the middle of runtime.</figcaption>
</figure>

It seems like whatever tiny hardware bug was present at the beginning, it has taken its toll on the rest of the electronics, at this point, I might try everything with a different Arduino, since perhaps mine has been fried.


arbot
=====
So what do you do, when you got these:


![](http://i.imgur.com/F8vw2tr.jpg) 
![](http://i.imgur.com/L4eA7Kb.jpg)
![](http://i.imgur.com/PEcMKbY.jpg)
![](http://i.imgur.com/A1QYEZi.jpg)
![](http://i.imgur.com/O1ZV7nG.jpg)


You build a robot of course! Well kind of. 
Maybe the proper term is IR remote controlled car with potential to become a robot.

### The software

The parts I have came in my posession randomly through different projects. But building a robot - one size doesn't fit all.
Maybe in the future I'm going to change something - remplace the platform, the motorboard or the way the robot communicates with the outside world.

From the software design point of view it makes sense to make it modular, and modular means interfaces and implementations -at least as long as the features of the modules remain similar.
That's why the arbot is implemented in c++, with classes and inheritance.

At the moment there are just 4 classes:
- ArbotPlatform       - the abstract class (interface) defining the platform
- ArbotVMA03Platform  - the platform impementation for the Velleman VMA03 motor shield
- ArbotController     - not actually implemented yet, will be when I add another controller
- IrController        - IR implementaton of the controller

For simplicity and because I'm used to java, all class implementations are in .h files. If there are any errors or bad practices exuse me, this is my firs c++ project after more than a decade of using more modern stuff.

### The IR
For IR decoding you'll need [Arduino-IRemote](https://github.com/shirriff/Arduino-IRremote) and ofcourse an IR sensor.
Those sensors come for different frequencies. Most commonly it'll be 38kHz or 36kHz - make sure you have the one wich mathces your remote.
Chances are it will work either way, but maybe not as reliable.

How does IR actually work? On the most basic level the transmitter (IR LED) either transmits or doesn't transmit. But while it transmits,
it's not turned on the whole time - it blinks (turns on and off) rapidly, 38.000 times per second!

If you want to transmit information to your friend by turning light on and off, you need to agree in advanced how the information
is going to be encoded. For example, you could use the morse code.

The same way, the transmitter (the remote) and the receiver need to agree on the protocol to be able to exchange information. One such protocol
is [RC-5](http://en.wikipedia.org/wiki/RC-5). 

In RC5, bit is represended by _space_ folowed by _mark_. Space means "light off" and mark "light on". Both signals are 889 microseconds long - that's less than 1 millisecond, which is like, really short time. By RC5 standard, the frequency of the blinking should be 36kHz, but many implementations out there us 38kHz.

|space mark| represents bit 1

|mark space| represents bit 0

That's as simple as encoding can be. But sending 1 and 0 doesn't meen much if you don't know what to expect. That's why RC5 code has standardised message protocol, which you cann look up on wiki page.

The Arduino Library we are using is doing all the hard work for us and transforms the incomming blinking into a RC5 code.
In my case, I'm using:
- 0x820 and 0x20 for forwards
- 0x810 and 0x10 for left
- 0x821 and 0x21 for right
- 0x811 and 0x11 for backwards

Why those? Because they are assigned the "cursor keys" on my remote. You can use IR library to display codes on your computer and adjust them as needed.

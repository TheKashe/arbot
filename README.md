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

For more info about IR and a tool to generate RC5 codes, see my [iRwaver mini project](https://github.com/jernejkase/iRwaver/tree/master)

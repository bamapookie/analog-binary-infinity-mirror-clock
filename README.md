# Analog Binary Infinity Mirror Clock

Plans, parts, and code for building your very own clock that no one can read. (Except you... maybe.)

## The Concept

Everyone knows analog clocks--2 or 3 hands moving around a clock face, indicating hours, minutes, and sometimes
seconds. It is easy to get a sense of time progressing by watching the hands move around, or gauging the position of one
hand in relation to the numbers (or even just the angles) to fill in an appropriate sense of precision.

Most people have seen binary clocks--Several obscure lights flickering on and off in a seemingly random pattern with
no markings to indicate number groupings, then having to decode the lights into a usable number. Some people cheat and
use Binary Coded Decimal in order to only have to decode one digit at a time.

What the world really needs is a way to combine the obscurity and complexity of decoding a binary number into a usable
format with the intrinsic sense of progress you get from the movement of the hands of an analog clock.

Thus began my quest to build a Binary Analog Clock! 11 hands moving around a clock face with only a '0' and a '1' to
adorn it. Now how the #^%\$% am I going to build a clock with 11 hands? I searched Google. Couldn't find anything
remotely resembling a kit for building an 11 handed clock. (IKR?) So I sat on it for a while, then I saw an infinity
mirror with NeoPixels in it, and it clicked. The infinity mirror effect could look like a line, and radiating in from
pixels on the outer ring, it could look like a hand. I finally had my plan. I dropped a boatload of money on Adafruit
and my local MicroCenter (See the materials list below, and thanks LadyAda for having such a fantastic operation.) and
started to work.

## The Plan

#### Basic plan

Basically, an infinity mirror is made with two mirrors facing each other. The mirror closest to you is semi-transparent.
This allows some of the light through, but the rest of the light bounces back and repeats the process. This makes the
space between the mirrors appear to extend beyond the back mirror. Between the mirrors, we will place 6 concentric
circles of DotStar pixels facing inward. Each of these will be a "hand" representing one bit of the binary number.
All six rings will be used to indicate minutes (2^6 = 64 > 60). The five innermost circles will also indicate hours
(2^5 = 32 > 24), using a slightly wider hand to distinguish it from the minute hands. I also have a plan for indicating
seconds by having the seconds "ripple" out from the corresponding minute hands, making the clock even more unreadable.

#### Intermediate plan

We want the "mechanics" to be small, so we'll use an Itsy Bitsy M4 to drive everything with a few other chips to bring up
the logic voltage to the DotStars. We'll print PCBs to mount it all on and stick it behind the clock.

#### Complex Plan

This is a clock. Clocks, in their default state, only blink 001100:000000 over and over again. We don't want that! We
want a clock that keeps its time. Moreover, we don't want to have to set it. Ever. Let's throw a GPS on there and we
won't ever have to set it! We'll just need to spend and extra few weeks figuring out the GPS code, how to set the time
from it, and wait... GPS time isn't the same as UTC time? UTC time has leap seconds? The standard time library
doesn't handle leap seconds or milliseconds? This won't do.

#### Overly Complex Plan

Let's rewrite the Arduino Time libraries, the GPS libraries, and the TimeZone libraries to handle leap seconds and
milliseconds. And because I laugh at my own jokes, we'll make it blink 001100:000000 for a little while when we turn
it on.

Okay. Maybe that is overdoing it. How about we keep track of the seconds separately, and just use the time libraries
for minute-level precision. We will increment the seconds using the PPS signal and data from the GPS on when leap
seconds occur, then update the GPS time when we determine a minute rollover has occured. This will also handle the
delay between the actual start of a second and the time that the NMEA message containing the time arrives from the GPS.

## The Implementation

Parts acquired... let's go!

While the final product will use an Itsy Bitsy M4 Express, I'm prototyping it with a Feather M4 Express. When I get the
IBM4, I'll use the Feather M4 Express along with an OLED display and another chip to measure the power usage of the
DotStars to make sure I have the right size power supply.

So now I have the breadboard out with one freshly soldered Feather M4 Express, one freshly soldered GPS Breakout board,
and a 1 meter length DotStar Strip.

#### Phase 1 - Testing the components

Using the default strandtest.ino from Adafruit with minor modifications, I tested out the DotStar strips. (See the extras
folder for strandtest.ino.)

PPS Test

GPS Test

#### Phase 2 - Hardware

Designing the PCB
Designing the Frame

#### Phase 3 - Writing the code

## Useful Links

##### Parts Lists

- https://www.adafruit.com/wishlists?wid=482342

##### Microcontrollers

- https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51/overview
- https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4/overview

##### Programming (C, C++)

- https://www.arduino.cc/reference/en/#functions
- http://www.gammon.com.au/forum/?id=11488
- https://forum.arduino.cc/index.php?topic=503368.0
- https://create.arduino.cc/projecthub/GadgetsToGrow/don-t-delay-use-an-arduino-task-scheduler-today-215cfe
- https://github.com/arkhipenko/TaskScheduler

##### DotStars

- https://learn.adafruit.com/adafruit-dotstar-leds/overview
- https://github.com/adafruit/Adafruit_DotStar
- https://wp.josh.com/2015/12/22/getting-real-using-blind-send-spi-to-turbocharge-the-adafruit-dotstar-library/

##### FastLED

- https://fastled.io/
- https://github.com/FastLED/FastLED
- https://www.reddit.com/r/FastLED/

##### GPS

- http://www.hhhh.org/wiml/proj/nmeaxor.html
- http://aprs.gids.nl/nmea/
- http://arduiniana.org/libraries/tinygpsplus/
- https://github.com/mikalhart/TinyGPSPlus
- http://www.flytron.com/pdf/MTK_commands.pdf
- https://en.wikipedia.org/wiki/NMEA_0183

###### Adafruit Ultimate GPS Breakout Board

- https://github.com/adafruit/Adafruit_GPS
- https://learn.adafruit.com/adafruit-ultimate-gps/overview
- https://forums.adafruit.com/viewtopic.php?f=19&t=148591&p=734292#p733660
- https://cdn-shop.adafruit.com/datasheets/GlobalTop-FGPMMOPA6C-Datasheet-V0A-Preliminary.pdf
- https://cdn-shop.adafruit.com/datasheets/PMTK_A11.pdf
- http://www.alphamicrowireless.com/franchises/globaltop-technology/pa6h.aspx

###### SparkFun GPS Breakout - XA1110

- https://www.sparkfun.com/products/14414
- https://www.sparkfun.com/datasheets/GPS/Modules/PMTK_Protocol.pdf

##### Components
- https://learn.sparkfun.com/tutorials/capacitors/all

##### Time

- https://www.pjrc.com/teensy/td_libs_Time.html
- https://www.davidpilling.com/wiki/index.php/GPS#a6
- https://github.com/PaulStoffregen/Time
- https://forum.arduino.cc/index.php?topic=503368.0
- https://github.com/JChristensen/Timezone

##### PCBs

- https://www.autodesk.com/products/eagle/free-download
- https://learn.adafruit.com/making-pcbs-with-oshpark-and-eagle/ordering-and-assembly
- https://oshpark.com/
- http://kicad-pcb.org/download/windows/
- https://cadlab.io ([My project](https://cadlab.io/project/2407)). 


##### Soldering

- https://www.youtube.com/watch?v=VxMV6wGS3NY

##### Random

- https://arduino.stackexchange.com/questions/348/how-to-organize-my-arduino-projects-structure-for-easy-source-control
- http://www.gammon.com.au/forum/?id=11488

## Addendum
#### Leap Seconds
During testing of the GPS is when I discovered the possible occurrence of leap seconds.  Every couple of years, or so, 
an extra second (or 2) is added to (or removed from) the last minute of June or December, UTC.  So instead of the 
seconds counting 58, 59, 60/0, 1, 2, etc., they might add an extra second (59, 60, 61/0, 1, 2), an extra 2 seconds 
(59, 60, 61, 62/0, 1, 2), or take away a second (58, 59/0, 1, 2).  All of this is to keep UTC in sync with UT1, which is 
the actual time based on the position of the earth in relation to the position of the stars.   UT1 drifts due to tidal 
forces changing the speed that the earth rotates.  Whenever the difference between UTC and UT1 reaches .9 seconds, leap 
seconds are used to correct the imbalance.

Unix clocks usually just ignore this.  They just leave the time at 23:59:59 until 00:00:00 eventually gets here.  All of
the time libraries used in Arduino environments also ignore this, and make no attempt to handle it.
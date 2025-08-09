Driver for vibrating / shocking butt plug. 

Device Specifications:
* Two sizes:
  * S: 72mm high, 28mm max diameter, 14mm stem
  * M: 81mm hight, 34mm max diameter, 16mm stem.
* 10 vibration modes / patterns:
  1.  low power constant
  2.  medium power constant
  3.  high power constant
  4.  2 sec low to high sawtooth
  5.  1 sec sinewave
  6.  2 sec sinewave
  7.  1/4 sec pulse
  8.  1/8? sec fast pulse
  9.  6 sec progression from slow(ish) to fast pulse
  10. 1 sec short-short-long pulses
* 6 levels of shock.
* Three button (power, vibrate, shock) 433 MHz remote, sending five commands:
  * Press and hold power button to turn device off (needs to be tuened
     on at device).
  * Short press power bitton to turn off vibrator.
  * Press vibrator button to turn on or cycle through vibrator speeds.
  * Press and hold shock button to turn off shocks,
  * Short press shock button to cycle through shock levels.
* Waterproof
* Magnetic USB charger connection and cable (to USB-A plug).

Device found here: https://www.aliexpress.com/item/1005007136131971.html


-- Protocol --

Radio control of the device can be done with a simple 433 MHz CW transmitter,
e.g. https://www.aliexpress.com/item/32608684824.html

The on-air protocol used a 20-bit command packet:

* Each command packet starts with ~ 700 us mark, and 4,300 us space.
* This is followed by 20 bits of data, encoded:
  * 1 is encoded as 700 us mark, followed by 300 us space;
  * 0 is encoded as 300 us mark, followed by 700 us space.
* The 20 bits consist of:
  * 8 bits of command. Commands correspond with button functions as described
    above, respectively:
    * 10001000 (0x88): Power off device
    * 11000011 (0xC3): Stop vibrator
    * 01011010 (0x5A): Cycle vibrator to next step (wrap to step 1 after 10)
    * 01100110 (0x66): Stop shocks
    * 00111100 (0x3C): Cycle shock to next level (wrap to step 1 after 6)
  * 8 constant bits 10111101 (0xBD)
  * 4 sequence number bits, used to indicate whether a command packet is a
    repeat or a new command. 

Each command packet is sent ten times, so a single command takes 250ms
to send (25ms per command packet). Commands can be send back to back, no delay 
required between either command repeats or new commands.


-- Arduino Library --

Synopsis:

	#include <ButtPlug.h>
	ButtPlug plug;

The methods available are:


void begin(char pin)

Initialises the object and sets up pin for transmitting data, Typically
called from setup().


void vib(char level)

Sets the level/pattern of the vibrator. Values are:
* 1-10 correspond to the patterns listed above. 
* 0 turns the vibrator off.
* -1 also turns off the vibrator, but the command is sent uncondtionally.

Note that the minimum number of commands are sent, so to go from 2 to 4,
two cycle commands wil be sent; to then go to level 3, a stop command
will be send, followed by three cycle commands.

Commands will not be sent if the vibraror is already at that level,
unless -1 is specified. Toset a level if you are unsure what state the
vibratior is in call vib(-1) followed by the level you want.


void zap(char level)

Sets the shock level. Values are:
* 1-6 is the power level (1 is lowest, 6 is highest).
* 0 to stop the shocks.
* -1 as above but sent unconditionally.

This command behaves as for vib().


void off()

Turn the device off. Note that device can not be turned on remotely.


void keepalive()

Call this frequently (e.g. inside loop()) to ensure the pulug does not
turn off during long periods of inactivity. The device stays alive as
long as it is running (and the battery holds out), but if both the
shocker and vibrator off, will shut down after half an hour of
inactivity. The keepalive gives a quick low power vibrator burst about
once every elevent minutes if it is otherwise idle.


void command(unsigned char code [, char count]) 

Low level command interface to send a command packet (repeated count
times, default 10). Specify the command value as code. Codes can be:
* BUTTPLUG_PWROFF (0x88): Power off device
* BUTTPLUG_VIBOFF (0xC3): Turn off/reset vibrator
* BUTTPLUG_VIB    (0x5A): Step (or turn on) vibrator
* BUTTPLUG_ZAPOFF (0x66): Turn off/reset zapper
* BUTTPLUG_ZAP    (0x3C): Step (or turn on) zapper


-- Author --

(C) 2025 Ruru, ruru67@yahoo.com

Non-commercial personal use and modification of this work is permitted.
Do not distribute this or derived work without permission.

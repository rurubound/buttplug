#include <Arduino.h>
#include <ButtPlug.h>

// Debug code
//#define DEBUG( dbg ) dbg		// Enable debug code
#define DEBUG( dbg ) 			// Disable debug code

// millis() >> 16 changes every 65.536 seconds
// Idle timeout after 30 mins or so.
// Sends a keepalive after 10-11 minutes of idle time.
//
#define KALV_TICK (millis() >> 16);	// Keepalive timer "minutes"
#define KALV_TICKS 10			// Keepalive after 10 mins

// Initialise plug structure
// Enable the control pin to the radio
//
void ButtPlug::begin(char pin) {
	radiopin = pin;
	zaplevel = viblevel = -2;	// Uninitialised
	kalv	 = KALV_TICK;
	seq	 = 0;
	pinMode(radiopin, OUTPUT);
	digitalWrite(radiopin, LOW);
}

// Low-level protocol driver
// Construct a command packet and transmit it
// Repeat command count times.
// 
void ButtPlug::command(unsigned char code, char count) {
	unsigned long bits = ((unsigned long)code << 12)   // 8-bit command
			   | ((unsigned long) 0xBD << 4)   // 8-bit constant
			   | ((++seq) & 0x0f);		   // 4-bit sequence
	char i;
	DEBUG(	Serial.print("Command: ");
		Serial.print((int) code, HEX);
		Serial.print(" bits ");
		for(i = 19; i >= 0; i--)
			Serial.print((bits >> i) & 1);
		Serial.println(); )
	while(count-- > 0) {
		for(i = 20; i >= 0; i--) {
			int m = 700, s = 300;		// Default to set bit
			if(i == 20)			// Lead-in
				s = 4300;
			else if(!((bits >> i) & 1)) {	// Cleared bit
				m = 300;
				s = 700;
			}
			digitalWrite(radiopin, HIGH);
			delayMicroseconds(m);
			digitalWrite(radiopin, LOW);
			delayMicroseconds(s);
		}
	}
}

// Vibrator steps:
// -1:   force off
//  0:   off
//  1-3: low/med/high constant
//  4:   2 sec sawtooth
//  5-8: Sinwave/pulse, 2 sec, 1 sec, 1/4 sec, fast
//  9:   6 sec progression from slow(ish) to fast pulses
// 10:   1 sec short-short-short-long pulses
//
void ButtPlug::vib(char lvl) {
	char curr = viblevel;
	DEBUG(	Serial.print("Vib level: ");
		Serial.print((int) curr);
		Serial.print(" -> ");
		Serial.println((int) lvl); )
	if(lvl == curr) return;
	if(lvl < 0 || lvl > 10)
		lvl = 0;
	viblevel = lvl;
	if(lvl > curr && curr >= 0) 
		lvl -= curr;
	else	command(BUTTPLUG_VIBOFF);
	for(; lvl > 0; lvl--)
		command(BUTTPLUG_VIB);
	if(curr) kalv = KALV_TICK;	// Prevent immediate keepalive
}

// Shock intensity:
// -1:   force off
//  0:   off
//  1-6: set shock intsensity, 1=low, 6=high
//
void ButtPlug::zap(char lvl) {
	char curr = zaplevel;
	DEBUG(	Serial.print("Zap level: ");
		Serial.print((int) curr);
		Serial.print(" -> ");
		Serial.println((int) lvl); )
	if(lvl == curr) return;
	if(lvl < 0 || lvl > 6)
		lvl = 0;
	zaplevel = lvl;
	if(lvl > curr && curr >= 0) 
		lvl -= curr;
	else	command(BUTTPLUG_ZAPOFF);
	for(; lvl > 0; lvl--)
		command(BUTTPLUG_ZAP);
	if(curr) kalv = KALV_TICK;	// Prevent immdiate keepalive
}

// Power off plug
//
void ButtPlug::off(void) {
	char curr = zaplevel;
	DEBUG(	Serial.println("Power off"); )
	command(BUTTPLUG_PWROFF);
	zaplevel = viblevel = -2;
}


// Send a keepalive every so often.
// Only send a keepalive if nothining interesting is haoppening 
// (keepalives reset the vib / zap, so don't if in use)
//
void ButtPlug::keepalive(void) {
	unsigned char k = KALV_TICK;
	if((unsigned char)(k - kalv) >= (unsigned char)KALV_TICKS) {
		kalv = k;
		DEBUG(	Serial.print("Keepalive k=");
			Serial.println((int) k); )
		if(viblevel <= 0 && zaplevel <= 0) {
			command(BUTTPLUG_VIB);
			command(BUTTPLUG_VIBOFF);
		}
	}
}

//eof

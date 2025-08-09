#ifndef ButtPlug_h
#define ButtPlug_h

// Butt Plug protocol driver.
// Control vibrating / shocking butt plug
//
// Transmit on 433 MHz, pulse width modulated, 20 bit commands. 1 kbps.
// 25 ms per command word, with 10 repeats, 250 ms per command.
// Lead-in:	700 us mark, 4300 us space (5 ms)
// Data bits 1: 700 us mark,  300 us space (1 ms)
//	     0: 300 us mark,  700 us space (1 ms)
// Commands: 20 bits after lead-in:
//		x____ cccccccc kkkkkkkk ssss
//	x____     5 bit-time lead-in
//	cccccccc: 8 bit command, see BUTTPLUG_xxx #defines below
//	kkkkkkkk: 8 bit constant 0xBD (10111101)
//	ssss:     4 bit sequence number to differentiate retransmissions from
//		  next command
// Commands repeated 10 times (approx 250 ms all up), Sequence incremented
// between commands. Commands may be sent back-to-back (qith lead-ins).
// All values sent high order bit first.
//
// Remote commands:		   Function			Button
#define BUTTPLUG_PWROFF	0x88	// Power off device	 	Hold power
#define BUTTPLUG_VIB	0x5A	// Step (or turn on) vibrator	Press buzz
#define BUTTPLUG_VIBOFF	0xC3	// Turn off/reset vibratior	Press power
#define BUTTPLUG_ZAP	0x3C	// Step (or turn on) zapper	Press zap
#define BUTTPLUG_ZAPOFF	0x66	// Turn off/reset zapper	Hold zap

class ButtPlug {
private:
	char radiopin;		// Output pin
	char zaplvl;		// Current zap level
	char viblvl;		// Current vibrator level
	unsigned char seq;	// Command sequence number
	unsigned char kalv;	// Keepalive timer

public:
	void begin(char pin);	// Initialise
	void command(unsigned char code, char count);
				// Send low-level command. Note: does not
				// track power levels.
	void command(unsigned char code) { command(code, 10); }
				// Default to 10 repeats.
	void zap(char lvl);	// Set zap level 0-6
				// 0: off, -1: force off
				// 1-6: shock power level, 1=min, 6=max,
	void vib(char lvl);	// Set vib level 0-10
				// 0: off, -1: force off
				// 1-10=power/patterns:
				//  1: low power constant
				//  2: medium power constant
				//  3: high power constant
				//  4: 2 sec low to high sawtooth
				//  5: 1 sec sinewave
				//  6: 2 sec sinewave
				//  7: 1/4 sec pulse
				//  8: 1/8? sec fast pulse
				//  9: 6 sec progression from slow(ish) to
				//     fast pulses
				// 10: 1 sec short-short-long pulses
	void off(void);		// Turn plug off		
	void keepalive(void);	// Call frequenly to keep toy alive
};

#endif // ButtPlug_h

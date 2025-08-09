// Test sketch for ButtPlug library
//
// Input commands:
// 1-9, 0   Set vibrator power/pattern to specified value (0=10)
// -        Turn vibrator off
// a-f      Set shock power level values 1-6 (a=1, b=2 ... f=6).
// Space    Turn shocker off
// X        Turn device off (can not be turned on remotely)
//

#include <ButtPlug.h>
ButtPlug plug;

void setup() {
	Serial.begin(9600);	// Serial reqiured...
	plug.begin(14);		// 433 MHz radio on pin 14
}


void loop() {
	plug.keepalive();	// Keep plug alive

	if(Serial.available()) { // Input commands
		char c = Serial.read();
		if(c < ' ') return;
		Serial.println(c);
		if(c >= '1' && c <= '9')
			plug.vib(c - '0');
		else if(c == '0')
			plug.vib(10);
		else if(c == '-')
			plug.vib(-1);
		else if(c >= 'a' && c <= 'f')
			plug.zap(c - 'a' + 1);
		else if(c == ' ')
			plug.zap(-1);
		else if(c == 'X')
			plug.off();
		else	Serial.println("Unrecognised command");
	}
}

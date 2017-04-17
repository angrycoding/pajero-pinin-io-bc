#ifndef KLine_h
#define KLine_h

#include <SoftwareSerial.h>

namespace KLine_private {

	uint8_t pinIn;
	uint8_t pinOut;
	SoftwareSerial *serial;

}

namespace KLine {

	void init(uint8_t pinIn, uint8_t pinOut) {
		using namespace KLine_private;
		pinMode(KLine_private::pinIn = pinIn, INPUT);
		pinMode(KLine_private::pinOut = pinOut, OUTPUT);
		serial = new SoftwareSerial(pinIn, pinOut);
	}

}

#endif
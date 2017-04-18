#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

namespace KL_private {

	uint8_t pinIn;
	uint8_t pinOut;
	SoftwareSerial *serial;

}

namespace KL {

	void init(uint8_t pinIn, uint8_t pinOut) {
		using namespace KL_private;
		pinMode(KL_private::pinIn = pinIn, INPUT);
		pinMode(KL_private::pinOut = pinOut, OUTPUT);
		serial = new SoftwareSerial(pinIn, pinOut);
	}

}

#endif
#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

namespace KL_private {

	uint8_t pinRX;
	uint8_t pinTX;
	SoftwareSerial *serial;

}

namespace KL {

	void init(uint8_t pinRX, uint8_t pinTX) {
		using namespace KL_private;
		pinMode(KL_private::pinRX = pinRX, INPUT);
		pinMode(KL_private::pinTX = pinTX, OUTPUT);
		serial = new SoftwareSerial(pinRX, pinTX);
	}

}

#endif
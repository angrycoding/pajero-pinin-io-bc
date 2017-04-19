#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

namespace KL_private {

	uint8_t pinRX;
	uint8_t pinTX;
	SoftwareSerial *klSerial;

}

namespace KL {

	void init(uint8_t pinRX, uint8_t pinTX) {
		using namespace KL_private;
		pinMode(KL_private::pinRX = pinRX, INPUT);
		pinMode(KL_private::pinTX = pinTX, OUTPUT);
		klSerial = new SoftwareSerial(pinRX, pinTX);
	}

	void connect() {
		using namespace KL_private;
		
		// wakeup
		digitalWrite(pinTX, HIGH);
		delay(300);

		// startbit
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit0
		digitalWrite(pinTX, HIGH);
		delay(200);

		// bit1
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit2
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit3
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit4
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit5
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit6
		digitalWrite(pinTX, LOW);
		delay(200);

		// bit7
		digitalWrite(pinTX, LOW);
		delay(200);

		// stopbit
		digitalWrite(pinTX, HIGH);
		delay(200);


		klSerial->begin(15625);

		uint32_t time = millis();

		for (;;) {
			if (millis() - time > 3000) break;
			while (klSerial->available()) {
				Serial.println(klSerial->read(), HEX);
			}
		}

		klSerial->end();

	}

}

#endif
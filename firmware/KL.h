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
		uint8_t position = 0;
		bool success = true;

		for (;;) {
			if (millis() - time > 3000) break;
			while (klSerial->available()) {
				uint8_t value = klSerial->read();
				switch (position++) {
					case 0: if (value != 0xC0) success = false; break;
					case 1: if (value != 0x55) success = false; break;
					case 2: if (value != 0xEF) success = false; break;
					case 3: if (value != 0x85) success = false; break;
					default: success = false;
				}
				Serial.println(value, HEX);
			}
		}

		if (success) {
			delay(60);
			position = 0;
			time = millis();
			int voltage = 0;

			// voltage
			klSerial->write(0x14);

			for (;;) {
				if (millis() - time > 3000) break;
				while (klSerial->available()) {
					uint8_t value = klSerial->read();
					if (position == 1) voltage = int(value) * 73 / 100;
					position++;
					Serial.println(value, HEX);
				}
			}

			Serial.print("VOLTAGE = ");
			Serial.println(voltage);

		}

		klSerial->end();

	}

}

#endif
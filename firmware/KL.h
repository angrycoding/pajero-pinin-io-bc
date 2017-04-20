#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

/*
#define KL_STATE_B0_START 0
#define KL_STATE_STARTBIT 1
#define KL_STATE_BIT0 2
#define KL_STATE_BIT1_7 3
#define KL_STATE_STOPBIT 4
#define KL_STATE_SERIAL_BEGIN 5
#define KL_STATE_SYNC 6
#define KL_STATE_REQUEST_PID 7
*/

namespace KL_private {

	uint8_t PIN_RX;
	uint8_t PIN_TX;
	// uint8_t state;
	// uint32_t actionTime;
	SoftwareSerial *klSerial;

}

namespace KL {

	void init(uint8_t PIN_RX, uint8_t PIN_TX) {
		using namespace KL_private;
		// actionTime = 0;
		// state = KL_STATE_B0_START;
		pinMode(KL_private::PIN_RX = PIN_RX, INPUT);
		pinMode(KL_private::PIN_TX = PIN_TX, OUTPUT);
		klSerial = new SoftwareSerial(PIN_RX, PIN_TX);
	}

	/*
	void update() {
		using namespace KL_private;
		switch (state) {

			case KL_STATE_ERROR:
				klSerial->end();
				state = KL_STATE_B0_START;

			case KL_STATE_B0_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B0_END;

			case KL_STATE_B0_END:
				if (millis() - actionTime < 300) break;
				state = KL_STATE_B1_START;

			case KL_STATE_B1_START:
				digitalWrite(PIN_TX, LOW);
				actionTime = millis();
				state = KL_STATE_B1_END;

			case KL_STATE_B1_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B2_START;

			case KL_STATE_B2_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B2_END;

			case KL_STATE_B2_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B3_START;

			case KL_STATE_B3_START:
				digitalWrite(PIN_TX, LOW);
				actionTime = millis();
				state = KL_STATE_B3_END;

			case KL_STATE_B3_END:
				if (millis() - actionTime < 1400) break;
				state = KL_STATE_B4_START;

			case KL_STATE_B4_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B4_END;

			case KL_STATE_B4_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B5_START;

			case KL_STATE_B5_START:
				klSerial->begin(15625);
				actionTime = millis();
				state = KL_STATE_B5_END;

			case KL_STATE_B5_END:
				
				if (millis() - actionTime > 1000 || (klSerial->available() == 3 && (
					klSerial->read() != 0x55 ||
					klSerial->read() != 0xEF ||
					klSerial->read() != 0x85
				))) {
					state = KL_STATE_ERROR;
					break;
				}

				if (klSerial->available() != 3) break;
				state = KL_STATE_B6_START;

		}
	}
	*/

	void connect() {
		using namespace KL_private;

		// wakeup
		digitalWrite(PIN_TX, HIGH);
		delay(300);

		// startbit
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit0
		digitalWrite(PIN_TX, HIGH);
		delay(200);

		// bit1
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit2
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit3
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit4
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit5
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit6
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// bit7
		digitalWrite(PIN_TX, LOW);
		delay(200);

		// stopbit
		digitalWrite(PIN_TX, HIGH);
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
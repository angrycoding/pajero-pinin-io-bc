#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

#define KL_STATE_ERROR 0
#define KL_STATE_RECONNECT_START 1
#define KL_STATE_RECONNECT_END 2
#define KL_STATE_B0_START 3
#define KL_STATE_B0_END 4
#define KL_STATE_B1_START 5
#define KL_STATE_B1_END 6
#define KL_STATE_B2_START 7
#define KL_STATE_B2_END 8
#define KL_STATE_B3_START 9
#define KL_STATE_B3_END 10
#define KL_STATE_B4_START 11
#define KL_STATE_B4_END 12
#define KL_STATE_B5_START 13
#define KL_STATE_B5_END 14
#define KL_STATE_PID_START 15
#define KL_STATE_PID_END 16

// скорость обмена после установки соединения
#define KL_SERIAL_SPEED 15625
// минимальная задержка перед новой попыткой подключения после ошибки
#define KL_DELAY_RECONNECT 3000
// минимальная задержка между PID запросами
#define KL_DELAY_NEXT_PID 60
// максимальный период ожидания ответа ЭБУ
#define KL_READ_TIMEOUT 1000

#define KL_PID_COOLANT_TEMP 0x10
#define KL_PID_VOLTAGE 0x14
#define KL_PID_THROTTLE 0x17
#define KL_PID_RPM 0x21
#define KL_PID_SPEED 0x2F


namespace KL_private {

	uint8_t PIN_RX;
	uint8_t PIN_TX;
	uint8_t state;
	uint32_t actionTime;
	uint8_t pidIndex;
	bool connected;
	uint8_t buffer[3];
	SoftwareSerial *klSerial;

	uint16_t rpm = 0;
	uint16_t speed = 0;
	uint16_t voltage = 0;
	uint16_t coolantTemp = 0;
	uint16_t throttlePosition = 0;

	uint8_t PIDS[] = {
		KL_PID_VOLTAGE,
		KL_PID_COOLANT_TEMP,
		KL_PID_RPM,
		KL_PID_SPEED,
		KL_PID_THROTTLE
	};

}

namespace KL {

	void init(uint8_t PIN_RX, uint8_t PIN_TX) {
		using namespace KL_private;
		connected = false;
		state = KL_STATE_B0_START;
		pinMode(KL_private::PIN_RX = PIN_RX, INPUT);
		pinMode(KL_private::PIN_TX = PIN_TX, OUTPUT);
		klSerial = new SoftwareSerial(PIN_RX, PIN_TX);
	}

	bool update() {
		using namespace KL_private;
		switch (state) {

			case KL_STATE_ERROR:
				connected = false;
				klSerial->end();
				state = KL_STATE_RECONNECT_START;

			case KL_STATE_RECONNECT_START:
				actionTime = millis();
				state = KL_STATE_RECONNECT_END;
				break;

			case KL_STATE_RECONNECT_END:
				if (millis() - actionTime < KL_DELAY_RECONNECT) break;
				state = KL_STATE_B0_START;

			case KL_STATE_B0_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B0_END;
				break;

			case KL_STATE_B0_END:
				if (millis() - actionTime < 300) break;
				state = KL_STATE_B1_START;

			case KL_STATE_B1_START:
				digitalWrite(PIN_TX, LOW);
				actionTime = millis();
				state = KL_STATE_B1_END;
				break;

			case KL_STATE_B1_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B2_START;

			case KL_STATE_B2_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B2_END;
				break;

			case KL_STATE_B2_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B3_START;

			case KL_STATE_B3_START:
				digitalWrite(PIN_TX, LOW);
				actionTime = millis();
				state = KL_STATE_B3_END;
				break;

			case KL_STATE_B3_END:
				if (millis() - actionTime < 1400) break;
				state = KL_STATE_B4_START;

			case KL_STATE_B4_START:
				digitalWrite(PIN_TX, HIGH);
				actionTime = millis();
				state = KL_STATE_B4_END;
				break;

			case KL_STATE_B4_END:
				if (millis() - actionTime < 200) break;
				state = KL_STATE_B5_START;

			case KL_STATE_B5_START:
				klSerial->begin(KL_SERIAL_SPEED);
				actionTime = millis();
				state = KL_STATE_B5_END;

			case KL_STATE_B5_END:
				if (millis() - actionTime > KL_READ_TIMEOUT) { state = KL_STATE_ERROR; break; }
				if (klSerial->available() < 3) break;
				if (klSerial->available() > 3) {
					state = KL_STATE_ERROR;
					break;
				}

				klSerial->readBytes(buffer, 3);

				if (buffer[0] != 0x55 || buffer[1] != 0xEF || buffer[2] != 0x85) {
					state = KL_STATE_ERROR;
					break;
				}

				pidIndex = 0;
				actionTime = 0;
				connected = true;
				state = KL_STATE_PID_START;

			case KL_STATE_PID_START:
				if (actionTime && (millis() - actionTime < KL_DELAY_NEXT_PID)) break;
				if (pidIndex == sizeof(PIDS)) pidIndex = 0;
				while (klSerial->available()) klSerial->read();
				klSerial->write(PIDS[pidIndex]);
				actionTime = millis();
				state = KL_STATE_PID_END;

			case KL_STATE_PID_END:
				if (millis() - actionTime > KL_READ_TIMEOUT) { state = KL_STATE_ERROR; break; }
				if (klSerial->available() < 2) break;
				if (klSerial->available() > 2 || klSerial->read() != PIDS[pidIndex]) {
					state = KL_STATE_ERROR;
					break;
				}

				switch (PIDS[pidIndex]) {
					case KL_PID_SPEED: speed = uint16_t(klSerial->read()) * 2; break;
					case KL_PID_VOLTAGE: voltage = uint16_t(klSerial->read()) * 73 / 100; break;
					case KL_PID_COOLANT_TEMP: coolantTemp = uint16_t(klSerial->read()) - 40; break;
					case KL_PID_THROTTLE: throttlePosition = uint16_t(klSerial->read()) * 100 / 255; break;
					case KL_PID_RPM: rpm = klSerial->read(); rpm = uint16_t(rpm) * 31 + uint16_t(rpm) / 4; break;
				}

				actionTime = millis();
				state = KL_STATE_PID_START;
				pidIndex++;
				return true;

		}
		return false;
	}

	uint16_t getRPM() {
		using namespace KL_private;
		return rpm;
	}

	uint16_t getSpeed() {
		using namespace KL_private;
		return speed;
	}

	uint16_t getVoltage() {
		using namespace KL_private;
		return voltage;
	}

	uint16_t getCoolantTemp() {
		using namespace KL_private;
		return coolantTemp;
	}

	uint16_t getThrottlePosition() {
		using namespace KL_private;
		return throttlePosition;
	}

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
#ifndef KL_h
#define KL_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define KL_STATE_ERROR 0
#define KL_STATE_RECONNECT_START 1
#define KL_STATE_RECONNECT_END 2

#define KL_STATE_BITBANG 3

#define KL_STATE_RESPWAIT_START 11
#define KL_STATE_RESPWAIT_END 12

#define KL_STATE_SEND_KW2_BEGIN 13
#define KL_STATE_SEND_KW2_END 14
#define KL_STATE_WAIT_CC 15

// скорость обмена после установки соединения
#define KL_SERIAL_SPEED 10400
// адрес ECU
#define KL_ECU_ADDRESS 0x33
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
	uint8_t bitIndex;
	uint8_t pidIndex;
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

	bool bitBangSendBit(bool value, uint8_t duration) {
		static uint32_t time = 0;
		if (time == 0) {
			digitalWrite(PIN_TX, value);
			time = millis();
		} else if (millis() - time >= duration) {
			time = 0;
			return true;
		}
		return false;
	}

	bool bitBang() {
		static uint8_t state = 9;
		switch (state) {

			// drive K-line HIGH for 300ms
			case 9:
				if (!bitBangSendBit(HIGH, 305)) break;
				state = 10;

			// send startbit
			case 10:
				if (!bitBangSendBit(LOW, 205)) break;
				state = 0;

			// send ECU address
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				if (!bitBangSendBit(KL_ECU_ADDRESS & 1 << state, 205) || ++state != 8) break;

			// send stopbit
			case 8:
				if (!bitBangSendBit(HIGH, 205)) break;
				state = 9;
				return true;

		}
		return false;
	}

}

namespace KL {

	void init(uint8_t PIN_RX, uint8_t PIN_TX) {
		using namespace KL_private;
		state = KL_STATE_RECONNECT_START;
		pinMode(KL_private::PIN_RX = PIN_RX, INPUT);
		pinMode(KL_private::PIN_TX = PIN_TX, OUTPUT);
		klSerial = new SoftwareSerial(PIN_RX, PIN_TX);
		digitalWrite(PIN_TX, HIGH);
	}

	bool update() {
		using namespace KL_private;
		switch (state) {

			case KL_STATE_ERROR:
				klSerial->end();
				state = KL_STATE_RECONNECT_START;

			case KL_STATE_RECONNECT_START:
				digitalWrite(PIN_TX, LOW);
				actionTime = millis();
				state = KL_STATE_RECONNECT_END;
				break;

			case KL_STATE_RECONNECT_END:
				if (millis() - actionTime < KL_DELAY_RECONNECT) break;
				state = KL_STATE_BITBANG;

			case KL_STATE_BITBANG:
				if (!bitBang()) break;
				state = KL_STATE_RESPWAIT_START;

			case KL_STATE_RESPWAIT_START:
				klSerial->begin(KL_SERIAL_SPEED);
				actionTime = millis();
				state = KL_STATE_RESPWAIT_END;

			case KL_STATE_RESPWAIT_END:

				if (millis() - actionTime > KL_READ_TIMEOUT) {
					Serial.println("KL_STATE_RESPWAIT_END TIMEOUT");
					Serial.print("available = ");
					Serial.println(klSerial->available());
					while (klSerial->available())
						Serial.println(klSerial->read(), HEX);
					state = KL_STATE_ERROR;
					break;
				}

				if (klSerial->available() < 3) break;

				if (klSerial->available() > 3) {
					Serial.println("KL_STATE_RESPWAIT_END > 3");
					Serial.print("available = ");
					while (klSerial->available())
						Serial.println(klSerial->read(), HEX);
					state = KL_STATE_ERROR;
					break;
				}

				klSerial->readBytes(buffer, 3);

				if (buffer[0] != 0x55 || buffer[1] != 0x08 || buffer[2] != 0x08) {
					Serial.println("KL_STATE_RESPWAIT_END mismatch");
					for (uint8_t c = 0; c < 3; c++)
						Serial.println(buffer[c], HEX);
					state = KL_STATE_ERROR;
					break;
				}

				state = KL_STATE_SEND_KW2_BEGIN;

			case KL_STATE_SEND_KW2_BEGIN:
				actionTime = millis();
				state = KL_STATE_SEND_KW2_END;
				break;

			case KL_STATE_SEND_KW2_END:
				if (millis() - actionTime < 28) break;
				klSerial->write(0xF7);
				actionTime = millis();
				state = KL_STATE_WAIT_CC;
				break;


			case KL_STATE_WAIT_CC:

				if (millis() - actionTime > KL_READ_TIMEOUT) {
					Serial.println("KL_STATE_WAIT_CC TIMEOUT");
					Serial.print("available = ");
					Serial.println(klSerial->available());
					while (klSerial->available())
						Serial.println(klSerial->read(), HEX);
					state = KL_STATE_ERROR;
					break;
				}

				if (klSerial->available() < 1) break;

				if (klSerial->available() > 1) {
					Serial.println("KL_STATE_WAIT_CC > 1");
					Serial.print("available = ");
					while (klSerial->available())
						Serial.println(klSerial->read(), HEX);
					state = KL_STATE_ERROR;
					break;
				}

				if (klSerial->peek() != 0xCC) {
					Serial.println("KL_STATE_WAIT_CC != 0xCC");
					Serial.print("available = ");
					while (klSerial->available())
						Serial.println(klSerial->read(), HEX);
					state = KL_STATE_ERROR;
				}

				klSerial->read();


				break;


			/*






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

			*/

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

}

#endif
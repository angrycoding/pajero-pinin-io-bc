#ifndef KL_h
#define KL_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define KL_STATE_ERROR 0
#define KL_STATE_BITBANG 1
#define KL_STATE_SYNC 2
#define KL_STATE_GETPID 3

// скорость обмена после установки соединения
#define KL_SERIAL_SPEED 15625
// адрес ECU
#define KL_ECU_ADDRESS 0x00

#define KL_PID_COOLANT_TEMP 0x10
#define KL_PID_VOLTAGE 0x14
#define KL_PID_THROTTLE 0x17
#define KL_PID_RPM 0x21
#define KL_PID_SPEED 0x2F


namespace KL_private {

	uint8_t PIN_RX;
	uint8_t PIN_TX;
	uint8_t state;
	uint8_t pidIndex;
	uint8_t pidResponse;
	SoftwareSerial *klSerial;



	uint8_t PIDS[] = {
		KL_PID_VOLTAGE,
		KL_PID_COOLANT_TEMP,
		KL_PID_RPM,
		KL_PID_SPEED,
		KL_PID_THROTTLE
	};


	uint16_t rpm = 0;
	uint16_t speed = 0;
	uint16_t voltage = 0;
	uint16_t coolantTemp = 0;
	uint16_t throttlePosition = 0;



	bool sendBit(bool value, uint32_t duration) {
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

	bool bitBang(bool wait3s) {
		static int8_t state = -3;
		switch (state) {

			// keep K-line LOW for 3s
			case -3:
				if (wait3s && !sendBit(LOW, 3000)) break;
				state++;

			// drive K-line HIGH for 300ms
			case -2:
				if (!sendBit(HIGH, 305)) break;
				state++;

			// send startbit
			case -1:
				if (!sendBit(LOW, 205)) break;
				state++;

			// send ECU address
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				if (!sendBit(KL_ECU_ADDRESS & 1 << state, 205) || ++state != 8) break;

			// send stopbit
			case 8:
				if (!sendBit(HIGH, 205)) break;
				state = -3;
				return true;

		}
		return false;
	}

	int8_t waitForSync() {
		static uint8_t state = 0;
		static uint32_t time = 0;

		switch (state) {

			case 0:
				time = millis(), state = 1;

			case 1:
				// first byte should come within 60ms..300ms
				if (millis() - time > 300) break;
				if (!klSerial->available()) return 0;
				if (klSerial->read() != 0x55) break;
				time = millis(), state = 2;

			case 2:
				// second byte should come within 5ms..20ms
				if (millis() - time > 20) break;
				if (!klSerial->available()) return 0;
				if (klSerial->read() != 0xEF) break;
				time = millis(), state = 3;

			case 3:
				// third byte should come within 0ms..20ms
				if (millis() - time > 20) break;
				if (!klSerial->available()) return 0;
				if (klSerial->read() != 0x85) break;
				state = 0;
				return 1;

		}

		state = 0;
		return -1;
	}


	int8_t doPIDRequest(uint8_t pid) {

		static uint8_t state = 0;
		static uint32_t time = 0;

		switch (state) {

			case 0:
				klSerial->write(pid);
				time = millis(), state = 1;

			case 1:
				// wait for the first byte to come ?ms
				if (millis() - time > 130) { state = 0; return -1; }
				if (!klSerial->available()) return 0;
				if (klSerial->read() != pid) { state = 0; return -1; }
				time = millis(), state = 2;

			case 2:
				// wait for the second byte to come ?ms
				if (millis() - time > 130) { state = 0; return -1; }
				if (!klSerial->available()) return 0;
				pidResponse = klSerial->read();
				time = millis();
				state = 3;
				return 1;

			case 3:
				// interval between PID requests?
				if (millis() - time >= 60) state = 0;
				return 0;

		}
	}






}

namespace KL {

	void init(uint8_t PIN_RX, uint8_t PIN_TX) {
		using namespace KL_private;
		state = KL_STATE_BITBANG;
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
				state = KL_STATE_BITBANG;

			case KL_STATE_BITBANG:
				if (!bitBang(true)) return false;
				klSerial->begin(KL_SERIAL_SPEED);
				pidIndex = 0;
				state = KL_STATE_SYNC;

			case KL_STATE_SYNC:
				switch (waitForSync()) {
					case -1: state = KL_STATE_ERROR;
					case 0: return false;
				}
				state = KL_STATE_GETPID;

			case KL_STATE_GETPID:

				switch (doPIDRequest(PIDS[pidIndex])) {
					case -1: state = KL_STATE_ERROR;
					case 0: return false;
				}

				switch (PIDS[pidIndex]) {
					case KL_PID_SPEED: speed = uint16_t(pidResponse) * 2; break;
					case KL_PID_VOLTAGE: voltage = uint16_t(pidResponse) * 73 / 100; break;
					case KL_PID_COOLANT_TEMP: coolantTemp = uint16_t(pidResponse) - 40; break;
					case KL_PID_THROTTLE: throttlePosition = uint16_t(pidResponse) * 100 / 255; break;
					case KL_PID_RPM: rpm = pidResponse; rpm = uint16_t(rpm) * 31 + uint16_t(rpm) / 4; break;
				}

				if (++pidIndex == sizeof(PIDS)) pidIndex = 0;
				return true;

		}
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
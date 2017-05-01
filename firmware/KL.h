#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

// адрес ECU
#define KL_ECU_ADDRESS 0x00
// скорость обмена
#define KL_SERIAL_SPEED 15625
// задержка перед попыткой установки соединения после ошибки
#define KL_RECONNECT_DELAY 5000
// время удержания "1" перед посылкой адреса
#define KL_INIT_DELAY 2
// задержка между посылками отдельных битов на скорости 5 бит/с
#define KL_INTERBIT_DELAY 205
// таймаут ожидания ответа от ЭБУ
#define KL_RESPONSE_TIMEOUT 400


#define KL_STATE_DISCONNECT -3
#define KL_STATE_WAKEUP -2
#define KL_STATE_STARTBIT -1
#define KL_STATE_A0BIT 0
#define KL_STATE_A1BIT 1
#define KL_STATE_A2BIT 2
#define KL_STATE_A3BIT 3
#define KL_STATE_A4BIT 4
#define KL_STATE_A5BIT 5
#define KL_STATE_A6BIT 6
#define KL_STATE_A7BIT 7
#define KL_STATE_STOPBIT 8
#define KL_STATE_SYNC 9
#define KL_STATE_REQUEST 10
#define KL_STATE_RESPONSE 11
#define KL_STATE_NEXT_REQUEST 12



namespace KL_private {

	uint8_t PIN_RX;
	uint8_t PIN_TX;
	int8_t state = -2;
	uint8_t pidResponse;
	SoftwareSerial *klSerial;
	uint32_t asyncDelayTime = 0;
	uint32_t lastRequestTime = 0;
	uint32_t disconnectCount = 0;



	int8_t asyncDelay(uint32_t delay) {

		if (asyncDelayTime == 0) {
			asyncDelayTime = millis();
			return -1;
		}

		if (millis() - asyncDelayTime >= delay) {
			asyncDelayTime = 0;
			return 1;
		}

		return 0;
	}

	bool sendBit(bool value, uint32_t duration) {
		if (asyncDelayTime == 0) {
			digitalWrite(PIN_TX, value);
			asyncDelayTime = millis();
		} else if (millis() - asyncDelayTime >= duration) {
			asyncDelayTime = 0;
			return false;
		}
		return true;
	}

	int8_t waitForBytes(uint8_t count) {
		static uint32_t time = 0;
		if (time == 0) time = millis();
		if (millis() - time > KL_RESPONSE_TIMEOUT) { time = 0; return -1; }
		uint8_t available = klSerial->available();
		if (available < count) return 0;
		time = 0;
		return (available == count ? 1 : -1);
	}

}

namespace KL {

	void init(uint8_t PIN_RX, uint8_t PIN_TX) {
		using namespace KL_private;
		pinMode(KL_private::PIN_RX = PIN_RX, INPUT);
		pinMode(KL_private::PIN_TX = PIN_TX, OUTPUT);
		klSerial = new SoftwareSerial(PIN_RX, PIN_TX);
		klSerial->begin(KL_SERIAL_SPEED);
	}

	bool request(uint8_t pid) {

		using namespace KL_private;

		switch (state) {

			// delay for reconnection after error
			case KL_STATE_DISCONNECT:
				switch (asyncDelay(KL_RECONNECT_DELAY)) {
					case -1: disconnectCount++;
					case 0: return false;
					case 1: state = KL_STATE_WAKEUP;
				}

			// before the initialization, the line K shall be logic "1" for the time period W0 (2ms..INF)
			case KL_STATE_WAKEUP:
				if (sendBit(HIGH, KL_INIT_DELAY)) return false;
				state = KL_STATE_STARTBIT;

			// send startbit
			case KL_STATE_STARTBIT:
				if (sendBit(LOW, KL_INTERBIT_DELAY)) return false;
				state = KL_STATE_A0BIT;

			// send ECU address
			case KL_STATE_A0BIT:
			case KL_STATE_A1BIT:
			case KL_STATE_A2BIT:
			case KL_STATE_A3BIT:
			case KL_STATE_A4BIT:
			case KL_STATE_A5BIT:
			case KL_STATE_A6BIT:
			case KL_STATE_A7BIT:
				if (sendBit(KL_ECU_ADDRESS & 1 << state, KL_INTERBIT_DELAY) || ++state != KL_STATE_STOPBIT) {
					return false;
				}

			// send stopbit
			case KL_STATE_STOPBIT:
				if (sendBit(HIGH, KL_INTERBIT_DELAY)) return false;
				while (klSerial->available()) klSerial->read();
				state = KL_STATE_SYNC;

			// wait for synchronization pattern
			case KL_STATE_SYNC:
				switch (waitForBytes(3)) {
					case 1: if (klSerial->read() == 0x55 &&
						klSerial->read() == 0xEF &&
						klSerial->read() == 0x85) {
						state = KL_STATE_REQUEST;
						break;
					}
					case -1: state = KL_STATE_DISCONNECT;
					case 0: return false;
				}

			// request given PID
			case KL_STATE_REQUEST:
				klSerial->write(pid);
				state = KL_STATE_RESPONSE;

			// wait for response and process it
			case KL_STATE_RESPONSE:
				switch (waitForBytes(2)) {
					case -1: state = KL_STATE_DISCONNECT;
					case 0: return false;
					case 1:
						klSerial->read();
						pidResponse = klSerial->read();
						asyncDelay(60);
						state = KL_STATE_NEXT_REQUEST;
						return true;
				}

			case KL_STATE_NEXT_REQUEST:
				if (asyncDelay(60) == 1)
					state = KL_STATE_REQUEST;
				return false;

		}
	}

	uint8_t response() {
		using namespace KL_private;
		return pidResponse;
	}

}

#endif
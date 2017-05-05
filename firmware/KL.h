#ifndef KL_h
#define KL_h

#include <SoftwareSerial.h>

// адрес ECU
#define KL_ECU_ADDRESS 0x00
// скорость обмена
#define KL_SERIAL_SPEED 15625
// задержка перед попыткой установки соединения после ошибки
// используется также в качестве таймаута ожидания ответа от ЭБУ
#define KL_RECONNECT_DELAY 5000
// время удержания "1" перед посылкой адреса
#define KL_INIT_DELAY 2
// задержка между посылками отдельных битов на скорости 5 бит/с
#define KL_INTERBIT_DELAY 205
// задержка между ответом и следующим запросом
#define KL_REQUEST_INTERVAL 20


//
#define KL_REQUEST_SYNC 3
#define KL_REQUEST_PID 2


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
	uint8_t pidResponse;
	SoftwareSerial *klSerial;
	uint32_t asyncDelayTime = 0;
	uint32_t disconnectCount = 0;
	int8_t state = KL_STATE_WAKEUP;

	bool asyncDelay(uint32_t delay) {
		if (asyncDelayTime == 0) asyncDelayTime = millis();
		else if (millis() - asyncDelayTime >= delay) {
			asyncDelayTime = 0;
			return false;
		}
		return true;
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

	uint8_t request(uint8_t count) {

		if (asyncDelayTime == 0) {
			asyncDelayTime = millis();
			return 1;
		}

		if (millis() - asyncDelayTime >= KL_RECONNECT_DELAY) {
			asyncDelayTime = 0;
			state = KL_STATE_WAKEUP;
			return 2;
		}

		uint8_t available = klSerial->available();

		if (available < count) return 3;

		if (available > count || (
			count == KL_REQUEST_SYNC &&
			(klSerial->read() != 0x55 || klSerial->read() != 0xEF || klSerial->read() != 0x85)
		)) {
			state = KL_STATE_DISCONNECT;
			return 4;
		}

		asyncDelayTime = 0;
		return 0;
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

	uint8_t write(uint8_t pid) {

		using namespace KL_private;

		switch (state) {

			// ожидаем отключения от ЭБУ
			case KL_STATE_DISCONNECT: {
				if (asyncDelay(KL_RECONNECT_DELAY)) return 0;
				state = KL_STATE_WAKEUP;
			}

			// до посылки адреса линия должна быть "1" в течении двух миллисекунд
			case KL_STATE_WAKEUP: {
				if (sendBit(HIGH, KL_INIT_DELAY)) return 0;
				disconnectCount++;
				state = KL_STATE_STARTBIT;
			}

			// посылаем стартовый бит
			case KL_STATE_STARTBIT: {
				if (sendBit(LOW, KL_INTERBIT_DELAY)) return 0;
				state = KL_STATE_A0BIT;
			}

			// посылаем адрес ЭБУ
			case KL_STATE_A0BIT:
			case KL_STATE_A1BIT:
			case KL_STATE_A2BIT:
			case KL_STATE_A3BIT:
			case KL_STATE_A4BIT:
			case KL_STATE_A5BIT:
			case KL_STATE_A6BIT:
			case KL_STATE_A7BIT: {
				if (sendBit(KL_ECU_ADDRESS & 1 << state, KL_INTERBIT_DELAY) || ++state != KL_STATE_STOPBIT) {
					return 0;
				}
			}

			// посылаем стоповый бит
			case KL_STATE_STOPBIT: {
				if (sendBit(HIGH, KL_INTERBIT_DELAY)) return 0;
				while (klSerial->available()) klSerial->read();
				state = KL_STATE_SYNC;
			}

			// ожидаем синхронизации
			case KL_STATE_SYNC: {
				if (request(KL_REQUEST_SYNC)) return 0;
				state = KL_STATE_REQUEST;
			}

			// запрашиваем переданный PID
			case KL_STATE_REQUEST: {
				klSerial->write(pid);
				state = KL_STATE_RESPONSE;
			}

			// ожидаем ответа ЭБУ и обрабатываем его
			case KL_STATE_RESPONSE: {
				switch (request(KL_REQUEST_PID)) {

					case 0:
						pidResponse = (klSerial->read(), klSerial->read());
						asyncDelay(KL_REQUEST_INTERVAL);
						state = KL_STATE_NEXT_REQUEST;
						return 1;
					
					// timeout
					case 2:
						return 2;

					default: return 0;
				}
			}

			// ожидаем истечения таймаута между запросами
			case KL_STATE_NEXT_REQUEST: {
				if (!asyncDelay(KL_REQUEST_INTERVAL))
					state = KL_STATE_REQUEST;
				return 0;
			}

		}
	}

	uint8_t read() {
		using namespace KL_private;
		return pidResponse;
	}

}

#endif
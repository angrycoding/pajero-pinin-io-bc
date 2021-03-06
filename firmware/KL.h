#ifndef KL_h
#define KL_h

#include "AltSoftSerial.h"

// пин контроллирующий отправку данных в K-line
#define KL_PIN_TX 9
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

// константы результата выполнения KL::write
#define KL_WRITE_FAIL 0
#define KL_WRITE_SUCCESS 1
#define KL_WRITE_INTERMEDIATE 2

// стэйты
#define KL_STATE_DISCONNECT -4
#define KL_STATE_DISCONNECTED -3
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

	uint32_t REQUEST_INTERVAL;
	uint8_t pidResponse[3];
	AltSoftSerial *klSerial;
	uint32_t waitingTime = 0;
	int8_t state = KL_STATE_WAKEUP;

	// при первом вызове возвращает 1
	// при втором и последующем вызовах, в случае если время переданное аргументом
	// не истекло с момента первого вызова - возвращает 2, в ином случае 0
	// если в качестве delay передать 0, то сбросит таймер и вернет 0
	uint8_t waiting(uint32_t delay) {
		return (
			// сброс таймера
			(delay == 0) ? (waitingTime = 0) :
			// первый раз = 1
			(waitingTime == 0) ? (waitingTime = millis(), 1) :
			// время истекло = 0
			(millis() - waitingTime >= delay) ? (waitingTime = 0) :
			// время не истекло = 2
			2
		);
	}

	// при первом вызове, устанавливает соотв. значение и возвращает true
	// при втором и последующем вызовах, в случае если время переаданное аргументом
	// не истекло с момента первого вызова - возвращает true, в ином случае false
	bool sendingBit(bool value, uint32_t duration) {
		switch (waiting(duration)) {
			// первый раз
			case 1: digitalWrite(KL_PIN_TX, value);
			// время не истекло
			case 2: return true;
			// время истекло
			case 0: return false;
		}
	}

	// ожидает получения соотв. количества байт, возврващает false в случае,
	// если указанное кол-во байт было принято в течении временного интервала
	// между первым вызовом и KL_RECONNECT_DELAY и true в любом другом случае
	// в случае таймаута, соотв. образом изменяет стэйт
	bool waitingForBytes(uint8_t count) {
		return (
			// таймаут, отправляем на стэйт который вернет признак ошибки
			!waiting(KL_RECONNECT_DELAY) ? (state = KL_STATE_DISCONNECTED, true) :
			// количество принятых байт не соответствует тому, что требуется
			klSerial->available() != count ? true :
			// сбрасываем таймер и считываем принятое в буфер
			(waiting(0), klSerial->readBytes(pidResponse, count), false)
		);
	}

}

namespace KL {

	const uint8_t WRITE_FAIL = KL_WRITE_FAIL;
	const uint8_t WRITE_SUCCESS = KL_WRITE_SUCCESS;

	void init(uint32_t requestInterval) {
		using namespace KL_private;
		pinMode(KL_PIN_TX, OUTPUT);
		KL_private::REQUEST_INTERVAL = requestInterval;
		klSerial = new AltSoftSerial();
		klSerial->begin(KL_SERIAL_SPEED);
	}

	uint8_t write(uint8_t pid) {

		using namespace KL_private;

		switch (state) {

			// выжидаем KL_RECONNECT_DELAY и переходим на следующий стэйт
			case KL_STATE_DISCONNECT: {
				if (waiting(KL_RECONNECT_DELAY)) break;
				state = KL_STATE_DISCONNECTED;
			}

			// нет ответа от ЭБУ, возвращаем флаг ошибки и готовимся к подключению
			case KL_STATE_DISCONNECTED: {
				state = KL_STATE_WAKEUP;
				return KL_WRITE_FAIL;
			}

			// до посылки адреса линия должна быть "1" в течении двух миллисекунд
			case KL_STATE_WAKEUP: {
				if (sendingBit(HIGH, KL_INIT_DELAY)) break;
				state = KL_STATE_STARTBIT;
			}

			// посылаем стартовый бит
			case KL_STATE_STARTBIT: {
				if (sendingBit(LOW, KL_INTERBIT_DELAY)) break;
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
				if (sendingBit(KL_ECU_ADDRESS & 1 << state, KL_INTERBIT_DELAY) || ++state != KL_STATE_STOPBIT) {
					break;
				}
			}

			// посылаем стоповый бит
			case KL_STATE_STOPBIT: {
				if (sendingBit(HIGH, KL_INTERBIT_DELAY)) break;
				// очищаем приемный буфер от мусора
				while (klSerial->available()) klSerial->read();
				state = KL_STATE_SYNC;
			}

			// ожидаем синхронизации
			case KL_STATE_SYNC: {
				if (waitingForBytes(3)) break;
				// сравниваем паттерн синхронизации
				if (pidResponse[0] != 0x55 || pidResponse[1] != 0xEF || pidResponse[2] != 0x85) {
					// неверный синхропаттерн, отключаемся
					state = KL_STATE_DISCONNECT;
					break;
				}
				state = KL_STATE_REQUEST;
			}

			// запрашиваем переданный PID
			case KL_STATE_REQUEST: {
				klSerial->write(pid);
				state = KL_STATE_RESPONSE;
			}

			// ожидаем ответа ЭБУ и обрабатываем его
			case KL_STATE_RESPONSE: {
				if (waitingForBytes(2)) break;
				// проверяем, нужен ли интервал между запросами
				if (REQUEST_INTERVAL) {
					// включаем счетчик ожидания минимального интервала между запросами
					waiting(REQUEST_INTERVAL);
					state = KL_STATE_NEXT_REQUEST;
				} else state = KL_STATE_REQUEST;
				return KL_WRITE_SUCCESS;
			}

			// ожидаем истечения таймаута между запросами
			case KL_STATE_NEXT_REQUEST: {
				if (!waiting(REQUEST_INTERVAL))
					state = KL_STATE_REQUEST;
				break;
			}

		}

		// возвращаем промежуточный результат
		return KL_WRITE_INTERMEDIATE;
	}

	uint8_t read() {
		using namespace KL_private;
		return pidResponse[1];
	}

}

#endif
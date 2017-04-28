#ifndef KL_h
#define KL_h

#include <Arduino.h>
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
#define KL_INTERBIT_DELAY 200
// таймаут ожидания ответа от ЭБУ
#define KL_RESPONSE_TIMEOUT 300


#define KL_STATE_RECONNECT -3


// температура охлаждающей жидкости
#define KL_PID_COOLANT_TEMP 0x10
// температура воздуха на впуске
#define KL_PID_INTAKE_AIR 0x11
// напряжение аккумулятора
#define KL_PID_VOLTAGE 0x14
// положение дроссельной заслонки
#define KL_PID_THROTTLE 0x17
// обороты двигателя
#define KL_PID_RPM 0x21
// длительность впрыска
#define KL_PID_IPW 0x29
// текущая скорость
#define KL_PID_SPEED 0x2F
// какой то барометр
#define KL_PID_BAROMETER 0x15
// какая то температура
#define KL_PID_TEMPERATURE 0x3A

// проверить
#define KL_PID_39 0x39
#define KL_PID_49 0x49
#define KL_PID_84 0x84
#define KL_PID_87 0x87
#define KL_PID_88 0x88
#define KL_PID_9A 0x9A
#define KL_PID_A8 0xA8
#define KL_PID_AA 0xAA
#define KL_PID_B3 0xB3
#define KL_PID_B4 0xB4
#define KL_PID_B8 0xB8



namespace KL_private {

	uint8_t PIN_RX;
	uint8_t PIN_TX;
	int8_t state = -2;
	uint8_t pidIndex = 0;
	SoftwareSerial *klSerial;
	uint32_t asyncDelayTime = 0;

	uint8_t PIDS[] = {
		KL_PID_COOLANT_TEMP,
		KL_PID_VOLTAGE,
		KL_PID_RPM,
		KL_PID_SPEED,
		KL_PID_INTAKE_AIR,
		KL_PID_THROTTLE,
		KL_PID_IPW,
		KL_PID_BAROMETER,
		KL_PID_TEMPERATURE,

		KL_PID_39,
		KL_PID_49,
		KL_PID_84,
		KL_PID_87,
		KL_PID_88,
		KL_PID_9A,
		KL_PID_A8,
		KL_PID_AA,
		KL_PID_B3,
		KL_PID_B4,
		KL_PID_B8
	};

	uint16_t rpm = 0;
	uint16_t speed = 0;
	float barometer = 0;
	int16_t coolantTemp = 0;
	int8_t ambientTemp = 0;
	float batteryVoltage = 0;
	float injPulseWidth = 0;
	int16_t intakeAirTemp = 0;
	uint8_t throttlePosition = 0;

	uint8_t pid39 = 0;
	uint8_t pid49 = 0;
	uint8_t pid84 = 0;
	uint8_t pid87 = 0;
	uint8_t pid88 = 0;
	uint8_t pid9A = 0;
	uint8_t pidA8 = 0;
	uint8_t pidAA = 0;
	uint8_t pidB3 = 0;
	uint8_t pidB4 = 0;
	uint8_t pidB8 = 0;

	int8_t asyncDelay(uint32_t delay = 0) {
		
		if (delay == 0) {
			asyncDelayTime = 0;
			return 1;
		}

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
		switch (asyncDelay(duration)) {
			case -1: digitalWrite(PIN_TX, value);
			case 0: return true;
			case 1: return false;
		}
	}

	bool waitForBytes(uint8_t count) {

		uint8_t available = klSerial->available();

		if (asyncDelay(KL_RESPONSE_TIMEOUT) == 1 || available > count) {
			asyncDelay();
			state = KL_STATE_RECONNECT;
			return true;
		}

		if (available < count) return true;

		if (available == count) {
			asyncDelay();
			return false;
		}
	}

	void updatePIDValue(uint8_t pid, uint8_t value) {
		switch (pid) {
			case KL_PID_RPM: rpm = round(31.25 * value); break;
			case KL_PID_SPEED: speed = (value * 2); break;
			case KL_PID_COOLANT_TEMP: coolantTemp = (value - 40); break;
			case KL_PID_VOLTAGE: batteryVoltage = round(value * 10) / 10; break;
			case KL_PID_IPW: injPulseWidth = (value / 1000); break;
			case KL_PID_INTAKE_AIR: intakeAirTemp = (value - 40); break;
			case KL_PID_THROTTLE: throttlePosition = round(value * 100 / 255); break;
			case KL_PID_BAROMETER: barometer = round(0.49 * value * 100) / 100; break;
			case KL_PID_TEMPERATURE: ambientTemp = (value - 40); break;

			case KL_PID_39: pid39 = value; break;
			case KL_PID_49: pid49 = value; break;
			case KL_PID_84: pid84 = value; break;
			case KL_PID_87: pid87 = value; break;
			case KL_PID_88: pid88 = value; break;
			case KL_PID_9A: pid9A = value; break;
			case KL_PID_A8: pidA8 = value; break;
			case KL_PID_AA: pidAA = value; break;
			case KL_PID_B3: pidB3 = value; break;
			case KL_PID_B4: pidB4 = value; break;
			case KL_PID_B8: pidB8 = value; break;
		}
	}

	bool mutLoop() {

		switch (state) {

			// delay for reconnection after error
			case KL_STATE_RECONNECT:
				if (asyncDelay(KL_RECONNECT_DELAY) != 1) return false;
				state++;

			// before the initialization, the line K shall be logic "1" for the time period W0 (2ms..INF)
			case -2:
				if (sendBit(HIGH, KL_INIT_DELAY)) return false;
				state++;

			// send startbit
			case -1:
				if (sendBit(LOW, KL_INTERBIT_DELAY)) return false;
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
				if (sendBit(KL_ECU_ADDRESS & 1 << state, KL_INTERBIT_DELAY) || ++state != 8) return false;

			// send stopbit
			case 8:
				if (sendBit(HIGH, KL_INTERBIT_DELAY)) return false;
				while (klSerial->available()) klSerial->read();
				state++;

			case 9:
				if (waitForBytes(3)) return false;
				
				if (klSerial->read() != 0x55 ||
					klSerial->read() != 0xEF ||
					klSerial->read() != 0x85) {
 					state = KL_STATE_RECONNECT;
 					return false;
 				}

				state++;
				pidIndex = 0;


			
			case 10:
				if (pidIndex == sizeof(PIDS))
					pidIndex = 0;
				klSerial->write(PIDS[pidIndex]);
				state++;

			case 11:
				if (waitForBytes(2)) return false;
				klSerial->read();
				updatePIDValue(PIDS[pidIndex++], klSerial->read());
				state = 10;
				return true;


		}
		return false;
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

}

#endif
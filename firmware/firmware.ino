#include "BC.h"
#include "KL.h"
#include "RPC.h"
#include <avr/wdt.h>



// температура охлаждающей жидкости
#define KL_PID_COOLANT_TEMP 0x10
// температура воздуха на впуске
#define KL_PID_INTAKE_AIR 0x11

// напряжение аккумулятора
#define KL_PID_VOLTAGE 0x14

// датчик барометрического давления (0.49 * x = КПА)
#define KL_PID_BAROMETER 0x15

// положение дроссельной заслонки
#define KL_PID_THROTTLE 0x17
// обороты двигателя
#define KL_PID_RPM 0x21
// длительность впрыска (в микросекундах)
#define KL_PID_IPW 0x29
// текущая скорость
#define KL_PID_SPEED 0x2F


// пин контроллирующий кнопку режима
#define PIN_BC_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BC_RESET 6

// пин контроллирующий прием данных из K-line
#define PIN_KL_RX 5
// пин контроллирующий отправку данных в K-line
#define PIN_KL_TX 4

// интервал watch-dog таймера
#define WDT_INTERVAL WDTO_4S
// скорость последовательного порта
#define SERIAL_SPEED 115200
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 2000

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66




uint8_t pidIndex = 0;
uint8_t PIDS[] = {
	KL_PID_COOLANT_TEMP,
	KL_PID_VOLTAGE,
	KL_PID_RPM,
	KL_PID_SPEED,
	KL_PID_INTAKE_AIR,
	KL_PID_THROTTLE,
	KL_PID_IPW,
	KL_PID_BAROMETER,

	0x40,
	0x41,
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4A,
	0x4B,
	0x4C,
	0x4D,
	0x4E,
	0x4F,
	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5A,
	0x5B,
	0x5C,
	0x5D,
	0x5E,
	0x5F
};

uint16_t rpm = 0;
uint16_t speed = 0;
float barometer = 0;
int16_t coolantTemp = 0;
float batteryVoltage = 0;
float injPulseWidth = 0;
int16_t intakeAirTemp = 0;
uint8_t throttlePosition = 0;

void sendPID(uint8_t pidIndex, uint8_t value) {
	setLine(pidIndex + 1);
	uint8_t pid = PIDS[pidIndex];
	switch (pid) {

		case KL_PID_RPM:
			Serial.print("RPM: ");
			Serial.print(rpm = round(31.25 * value));
			break;

		case KL_PID_SPEED:
			Serial.print("SPEED: ");
			Serial.print(speed = (value * 2));
			break;

		case KL_PID_COOLANT_TEMP:
			Serial.print("COOLANT: ");
			Serial.print(coolantTemp = (value - 40));
			break;

		case KL_PID_VOLTAGE:
			Serial.print("BATTERY: ");
			Serial.print(batteryVoltage = (0.07333 * value));
			break;

		case KL_PID_BAROMETER:
			Serial.print("BAROMETER: ");
			Serial.print(barometer = (0.49 * value));
			break;

		case KL_PID_IPW:
			Serial.print("IPW: ");
			Serial.print(injPulseWidth = value);
			break;

		case KL_PID_THROTTLE:
			Serial.print("THROTTLE: ");
			Serial.print(throttlePosition = round(value * 100 / 255));
			break;

		case KL_PID_INTAKE_AIR:
			Serial.print("INTAKE_AIR: ");
			Serial.print(intakeAirTemp = (value - 40));
			break;

		default:
			Serial.print(pid, HEX);
			Serial.print(": ");
			Serial.print(value);

	}
}

void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	KL::init(PIN_KL_RX, PIN_KL_TX);
	BC::init(PIN_BC_MODE, PIN_BC_RESET, BC_UPDATE_INTERVAL_MS);
	// otherwise there is a risk to send something several times in millis() - time
	delay(1);
	clearAndHome();
}

void serialEvent() {
	if (RPC::process()) switch (RPC::getCommand()) {
		case CMD_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_RESET_CONSUMPTION: BC::resetConsumption(); break;
	}
}

void clearAndHome() {
	Serial.write(27);
	Serial.print("[2J"); // clear screen
	Serial.write(27); // ESC
	Serial.print("[H"); // cursor to home
}

void setLine(uint8_t y) {
	Serial.write(27);
	Serial.print("[");
	Serial.print(y);
	Serial.print(";0");
	Serial.print("H");
	Serial.write(27);
	Serial.print("[K");
}

void loop() {

	wdt_reset();



	// if (true || BC::update()) {

	// 	setLine(1);
	// 	Serial.print("getTemperature: ");
	// 	Serial.print(BC::getTemperature());

	// 	setLine(2);
	// 	Serial.print("getFuel: ");
	// 	Serial.print(BC::getFuel());

	// 	setLine(3);
	// 	Serial.print("getSpeed: ");
	// 	Serial.print(BC::getSpeed());

	// 	setLine(4);
	// 	Serial.print("getConsumption: ");
	// 	Serial.println(BC::getConsumption());
	// }

	if (true || KL::request(PIDS[pidIndex])) {
		sendPID(pidIndex++, KL::response());
		if (pidIndex == sizeof(PIDS)) pidIndex = 0;
	}


}

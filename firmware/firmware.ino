#include "BC.h"
#include "KL.h"
#include "RPC.h"
#include <avr/wdt.h>

// пин контроллирующий кнопку режима
#define PIN_BC_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BC_RESET 6
// пин контроллирующий прием данных из K-line
#define PIN_KL_RX 4
// пин контроллирующий отправку данных в K-line
#define PIN_KL_TX 5

// интервал watch-dog таймера
#define WDT_INTERVAL WDTO_1S
// скорость последовательного порта
#define SERIAL_SPEED 115200
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 5000

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66
#define CMD_KL_CONNECT 67


void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	KL::init(PIN_KL_RX, PIN_KL_TX);
	BC::init(PIN_BC_MODE, PIN_BC_RESET, BC_UPDATE_INTERVAL_MS);
}

void serialEvent() {
	if (RPC::process()) switch (RPC::getCommand()) {

		case CMD_RESET_SPEED:
			BC::resetSpeed();
			break;

		case CMD_RESET_CONSUMPTION:
			BC::resetConsumption();
			break;

		case CMD_KL_CONNECT:
			wdt_disable();
			KL::connect();
			wdt_enable(WDT_INTERVAL);
			break;
	}
}

void loop() {

	wdt_reset();
	bool xUpd = BC::update();
	bool yUpd = KL::update();

	if (xUpd || yUpd) {

		Serial.print("$TIME: ");
		Serial.println(BC::getTime());

		Serial.print("TEMPERATURE: ");
		Serial.println(BC::getTemperature());

		Serial.print("FUEL_KM: ");
		Serial.println(BC::getFuel());

		Serial.print("SPEED_KMH: ");
		Serial.println(BC::getSpeed());

		Serial.print("CONSUMPTION_L100KM: ");
		Serial.println(BC::getConsumption());



		Serial.print("RPM: ");
		Serial.println(KL::getRPM());

		Serial.print("SPEED: ");
		Serial.println(KL::getSpeed());

		Serial.print("VOLTAGE: ");
		Serial.println(KL::getVoltage());

		Serial.print("COOLANT_TEMP: ");
		Serial.println(KL::getCoolantTemp());

		Serial.print("THROTTLE_POSITION: ");
		Serial.println(KL::getThrottlePosition());

	}

}

#include "BC.h"
#include "RPC.h"
#include "KLine.h"
#include <avr/wdt.h>

// интервал watch-dog таймера
#define WDT_INTERVAL WDTO_4S
// скорость последовательного порта
#define SERIAL_SPEED 115200
// пин контроллирующий кнопку режима
#define PIN_BUTTON_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BUTTON_RESET 3
// минимальный интервал между обновлениями БК
#define BC_UPDATE_INTERVAL_MS 5000

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66

void setup() {
	wdt_enable(WDT_INTERVAL);
	Serial.begin(SERIAL_SPEED);
	BC::init(PIN_BUTTON_MODE, PIN_BUTTON_RESET, BC_UPDATE_INTERVAL_MS);
}

void serialEvent() {
	if (RPC::process()) switch (RPC::getCommand()) {
		case CMD_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_RESET_CONSUMPTION: BC::resetConsumption(); break;
	}
}

void loop() {

	wdt_reset();

	if (BC::update()) {

		Serial.print("TIME: ");
		Serial.println(BC::getTime());

		Serial.print("TEMPERATURE: ");
		Serial.println(BC::getTemperature());

		Serial.print("FUEL_KM: ");
		Serial.println(BC::getFuel());

		Serial.print("SPEED_KMH: ");
		Serial.println(BC::getSpeed());

		Serial.print("CONSUMPTION_L100KM: ");
		Serial.println(BC::getConsumption());
	}

}

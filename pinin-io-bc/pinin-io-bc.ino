#include "BC.h"
#include "RPC.h"

// пин контроллирующий кнопку режима
#define PIN_BUTTON_MODE 2
// пин контроллирующий кнопку сброса
#define PIN_BUTTON_RESET 3

#define CMD_RESET_SPEED 65
#define CMD_RESET_CONSUMPTION 66

void setup() {
	Serial.begin(115200);
	BC::init(PIN_BUTTON_MODE, PIN_BUTTON_RESET);
}

void serialEvent() {
	if (RPC::process()) switch (RPC::getCommand()) {
		case CMD_RESET_SPEED: BC::resetSpeed(); break;
		case CMD_RESET_CONSUMPTION: BC::resetConsumption(); break;
	}
}

void loop() {

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

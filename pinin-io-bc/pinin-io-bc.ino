#include "BC.h"
#include "RPC.h"

void setup() {
	Serial.begin(115200);
	BC::init();
}

void serialEvent() {
	if (RPC::process()) {
		Serial.print("RECEIVED_COMMAND: ");
		Serial.println(RPC::getCommand());
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

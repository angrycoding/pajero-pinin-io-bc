#include "BC.h"

void setup() {
	Serial.begin(115200);
	BC::init();
}


void loop() {

	if (BC::update()) {

		Serial.print("NEW_TIME: ");
		Serial.println(BC::TIME);

		Serial.print("NEW_TEMPERATURE: ");
		Serial.println(BC::TEMPERATURE);

		Serial.print("NEW_FUEL: ");
		Serial.println(BC::FUEL);

		Serial.print("NEW_FUEL: ");
		Serial.println(BC::FUEL);

		Serial.print("NEW_SPEED: ");
		Serial.println(BC::SPEED);

		Serial.print("NEW_CONSUMPTION: ");
		Serial.println(BC::CONSUMPTION);
	}

}

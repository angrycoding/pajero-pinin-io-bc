#include "BC.h"

void setup() {
	Serial.begin(115200);
	BC::init();
}


void loop() {

	if (BC::update()) {

		Serial.print("TIME: ");
		Serial.println(BC::TIME);

		Serial.print("TEMPERATURE: ");
		Serial.println(BC::TEMPERATURE);

		Serial.print("FUEL_KM: ");
		Serial.println(BC::FUEL_KM);

		Serial.print("SPEED_KMH: ");
		Serial.println(BC::SPEED_KMH);

		Serial.print("CONSUMPTION_L100KM: ");
		Serial.println(BC::CONSUMPTION_L100KM);
	}

}
